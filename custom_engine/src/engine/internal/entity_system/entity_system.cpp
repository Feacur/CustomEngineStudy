#include "custom_pch.h"

#include "engine/api/internal/parsing.h"
#include "engine/api/internal/entity_system.h"
#include "engine/api/internal/types_names_lookup.h"
#include "engine/impl/array.h"

namespace custom {

//  @Note: initialize compile-time structs:
template struct Array<Entity>;

//  @Note: initialize compile-time statics:
Gen_Pool               Entity::generations;
Array<Entity>          Entity::instances;
Array<Ref>             Entity::components;
Strings_Storage        Entity::strings;

#if defined(ENTITY_COMPONENTS_DENSE)
Array<u32>             Entity::component_types;
Array<u32>             Entity::component_entity_ids;
#endif

Array<ref_void_func *> Entity::component_constructors;
Array<void_ref_func *> Entity::component_destructors;
Array<bool_ref_func *> Entity::component_containers;
Array<entity_from_to_func *> Entity::component_copiers;
Array<entity_loading_func *> Entity::component_cleaners;
Array<serialization_read_func *> Entity::component_serialization_readers;

}

//
// strings API
//

namespace custom {

u32 Entity::store_string(cstring data, u32 length) {
	return strings.store_string(data, length);
}

cstring Entity::get_string(u32 id) {
	return strings.get_string(id);
}

}

//
// entity
//

namespace custom {
namespace serialization {

void serialization_read_Entity_block(Entity & entity, cstring * source);
void serialization_read_Child_block(Entity & entity, cstring * source);

}}

namespace custom {

static u32 find_instance(u32 entity) {
	for (u32 i = 0; i < Entity::instances.count; ++i) {
		if (Entity::instances[i].id != entity) { continue; }
		return i;
	}
	return custom::empty_index;
}

Entity Entity::create(bool is_instance) {
	Entity entity = {Entity::generations.create()};
	if (is_instance) { instances.push(entity); }
	return entity;
}

Entity Entity::serialization_read(cstring * source) {
	CUSTOM_TRACE("serialization read start");
	Entity entity = create(false);

	// @Note: component readers are assumed to early out upon discovery of
	//        any unrecognized non-whitespace sequence

	while (**source) {
		cstring line_end = (parse_void(source), *source); skip_to_eol(&line_end);

		if (**source == '~') { break; };

		if (**source == '!') {
			serialization::serialization_read_Entity_block(entity, source);
			continue;
		}

		if (**source == '>') {
			serialization::serialization_read_Child_block(entity, source);
			continue;
		};

		u32 type = custom::component_names.get_id(*source, (u32)(line_end - *source));
		if (type != custom::empty_index) {
			Ref component_ref = entity.add_component(type);
			(*Entity::component_serialization_readers[type])(entity, component_ref, source);
			continue;
		}

		// @Note: any unrecognized line is silently skipped
		skip_to_eol(source); parse_eol(source);
	}

	CUSTOM_TRACE("serialization read end");
	return entity;
}

void Entity::override(cstring * source) {
	CUSTOM_TRACE("override start");

	while (**source) {
		cstring line_end = (parse_void(source), *source); skip_to_eol(&line_end);

		if (**source == '~') { break; };

		if (**source == '!') {
			serialization::serialization_read_Entity_block(*this, source);
			continue;
		}

		if (**source == '>') {
			serialization::serialization_read_Child_block(*this, source);
			continue;
		};

		u32 type = custom::component_names.get_id(*source, (u32)(line_end - *source));
		if (type != custom::empty_index) {
			Ref component_ref = has_component(type) ? get_component(type) : add_component(type);
			(*Entity::component_serialization_readers[type])(*this, component_ref, source);
			continue;
		}

		// @Note: any unrecognized line is silently skipped
		skip_to_eol(source); parse_eol(source);
	}

	CUSTOM_TRACE("override end");
}

void Entity::override(Entity const & entity) {
	CUSTOM_TRACE("override start");

	for (u32 type = 0; type < Entity::component_containers.count; ++type) {
		Ref const from_component_ref = entity.get_component(type);
		if ((*Entity::component_containers[type])(from_component_ref)) {
			Ref to_component_ref = has_component(type) ? get_component(type) : add_component(type);
			(*Entity::component_copiers[type])(*this, from_component_ref, to_component_ref);
		}
	}

	CUSTOM_TRACE("override end");
}

void Entity::destroy(void) {
	if (!exists()) { CUSTOM_ASSERT(false, "entity doesn't exist"); return; }

	u32 entity_offset = id * Entity::component_destructors.count;
	if (entity_offset < Entity::components.capacity) {
		for (u32 type = 0; type < Entity::component_destructors.count; ++type) {
			Ref component_ref = Entity::components.get(entity_offset + type);
			if ((*Entity::component_containers[type])(component_ref)) {
				(*Entity::component_cleaners[type])(*this, component_ref, true);
				(*Entity::component_destructors[type])(component_ref);
			}
		}
	}

	Entity::generations.destroy(*this);

	for (u32 i = 0; i < instances.count; ++i) {
		if (instances[i] != *this) { continue; }
		instances.remove_at(i);
		break;
	}
}

bool Entity::is_instance() const {
	return find_instance(id) != custom::empty_index;
}

Entity Entity::copy(bool force_instance) const {
	if (!exists()) { CUSTOM_ASSERT(false, "entity doesn't exist"); return {custom::empty_ref}; }

	Entity entity = create(is_instance() || force_instance);

	for (u32 type = 0; type < Entity::component_containers.count; ++type) {
		Ref const from_component_ref = get_component(type);
		if ((*Entity::component_containers[type])(from_component_ref)) {
			Ref to_component_ref = entity.add_component(type);
			(*Entity::component_copiers[type])(entity, from_component_ref, to_component_ref);
		}
	}

	return entity;
}

void Entity::promote_to_instance(void) {
	if (is_instance()) { CUSTOM_ASSERT(false, "prefab is an instance already"); return; }
	instances.push(*this);
}

}

//
// entity components
//

#if defined(ENTITY_COMPONENTS_DENSE)

namespace custom {

static u32 find(u32 type, u32 entity) {
	for (u32 i = 0; i < Entity::component_types.count; ++i) {
		if (Entity::component_types[i] != type) { continue; }
		if (Entity::component_entity_ids[i] == entity) { return i; }
	}
	return custom::empty_index;
}

Ref Entity::add_component(u32 type) {
	if (!exists()) { CUSTOM_ASSERT(false, "entity doesn't exist"); return custom::empty_ref; }

	Ref component_ref = custom::empty_ref;

	u32 component_index = find(type, ref.id);
	if (component_index != custom::empty_index) { component_ref = Entity::components[component_index]; }

	if (component_ref.id == custom::empty_ref.id || !(*Entity::component_containers[type])(component_ref)) {
		if (component_index != custom::empty_index) {
			Entity::components.remove_at(component_index);
			Entity::component_entity_ids.remove_at(component_index);
			Entity::component_types.remove_at(component_index);
		}

		component_ref= (*Entity::component_constructors[type])();
		Entity::components.push(component_ref);
		Entity::component_entity_ids.push(ref.id);
		Entity::component_types.push(type);
	}
	// @Todo: check explicitly?
	//else { CUSTOM_ASSERT(false, "component already exists"); }

	return component_ref;
}

void Entity::rem_component(u32 type) {
	// @Note: duplicates `Component::destroy` code
	Ref component_ref = custom::empty_ref;

	u32 component_index = find(type, ref.id);
	if (component_index != custom::empty_index) {
		component_ref = Entity::components[component_index];
		Entity::components.remove_at(component_index);
		Entity::component_entity_ids.remove_at(component_index);
		Entity::component_types.remove_at(component_index);
	}

	if ((*Entity::component_containers[type])(component_ref)) {
		(*Entity::component_cleaners[type])(*this, component_ref, false);
		(*Entity::component_destructors[type])(component_ref);
	}
	else { CUSTOM_ASSERT(false, "component doesn't exist"); }
}

Ref Entity::get_component(u32 type) const {
	if (!exists()) { CUSTOM_ASSERT(false, "entity doesn't exist"); return custom::empty_ref; }

	u32 component_index = find(type, ref.id);
	if (component_index == custom::empty_index) { return custom::empty_ref; }

	return Entity::components[component_index];
}

bool Entity::has_component(u32 type) const {
	if (!exists()) { CUSTOM_ASSERT(false, "entity doesn't exist"); return false; }

	u32 component_index = find(type, ref.id);
	if (component_index == custom::empty_index) { return false; }

	Ref component_ref = Entity::components[component_index];
	return (*Entity::component_containers[type])(component_ref);
}

}

#else

namespace custom {

Ref Entity::add_component(u32 type) {
	if (!exists()) { CUSTOM_ASSERT(false, "entity doesn't exist"); return custom::empty_ref; }

	// entity_components_ensure_capacity
	u32 capacity_before = Entity::components.capacity;
	Entity::components.ensure_capacity((id + 1) * Entity::component_constructors.count);
	for (u32 i = capacity_before; i < Entity::components.capacity; ++i) {
		Entity::components.data[i] = custom::empty_ref;
	}

	u32 component_index = id * Entity::component_constructors.count + type;
	Ref component_ref = Entity::components.get(component_index);

	if (!(*Entity::component_containers[type])(component_ref)) {
		component_ref = (*Entity::component_constructors[type])();
		Entity::components.get(component_index) = component_ref;
	}
	else { CUSTOM_ASSERT(false, "component already exists"); }

	return component_ref;
}

void Entity::rem_component(u32 type) {
	if (!exists()) { CUSTOM_ASSERT(false, "entity doesn't exist"); return; }

	u32 component_index = id * Entity::component_destructors.count + type;
	if (component_index >= Entity::components.capacity) {
		CUSTOM_ASSERT(false, "component doesn't exist"); return;
	}

	Ref component_ref = Entity::components.get(component_index);

	if ((*Entity::component_containers[type])(component_ref)) {
		(*Entity::component_cleaners[type])(*this, component_ref, false);
		(*Entity::component_destructors[type])(component_ref);
	}
	else { CUSTOM_ASSERT(false, "component doesn't exist"); }
}

Ref Entity::get_component(u32 type) const {
	if (!exists()) { CUSTOM_ASSERT(false, "entity doesn't exist"); return custom::empty_ref; }

	u32 component_index = id * Entity::component_containers.count + type;
	if (component_index >= Entity::components.capacity) { return custom::empty_ref; }

	return Entity::components.get(component_index);
}

bool Entity::has_component(u32 type) const {
	if (!exists()) { CUSTOM_ASSERT(false, "entity doesn't exist"); return false; }

	u32 component_index = id * Entity::component_containers.count + type;
	if (component_index >= Entity::components.capacity) { return false; }

	Ref component_ref = Entity::components.get(component_index);
	return (*Entity::component_containers[type])(component_ref);
}

}

#endif

//
// component ref
//

// @Note: experimental, mimics Asset; in case one needs fully self-contained ref
namespace custom {

Ref Component::get(void) {
	CUSTOM_ASSERT(entity.get_component(type) == ref, "component ref is corrupted");
	return ref;
}

bool Component::exists(void) const {
	CUSTOM_ASSERT(entity.get_component(type) == ref, "component ref is corrupted");
	return (*Entity::component_containers[type])(ref);
}

void Component::destroy(void) {
	// @Note: duplicates `Entity::rem_component` code
	CUSTOM_ASSERT(entity.get_component(type) == ref, "component ref is corrupted");
	if ((*Entity::component_containers[type])(ref)) {
		(*Entity::component_cleaners[type])(entity, ref, false);
		(*Entity::component_destructors[type])(ref);
	}
	else { CUSTOM_ASSERT(false, "component doesn't exist"); }

	#if defined(ENTITY_COMPONENTS_DENSE)
	for (u32 i = 0; i < Entity::components.count; ++i) {
		if (Entity::component_types[i] != type) { continue; }
		if (Entity::component_entity_ids[i] != entity.ref.id) { continue; }
		if (Entity::components[i] != ref) { continue; }
		Entity::components.remove_at(i);
		Entity::component_entity_ids.remove_at(i);
		Entity::component_types.remove_at(i);
		break;
	}
	#endif
}

}
