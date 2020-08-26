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
Array<from_to_func *>  Entity::component_copiers;
Array<serialization_read_func *> Entity::component_serialization_readers;

}

//
// entity
//

namespace custom {

Entity Entity::create(bool is_instance) {
	Entity entity = {Entity::generations.create()};
	if (is_instance) { instances.push(entity); }
	return entity;
}

Entity Entity::serialization_read(Array<u8> const & file, bool is_instance) {
	Entity entity = create(is_instance);

	cstring source = (cstring)file.data;
	cstring const end = (cstring)file.data + file.count;
	while (source < end) {
		parse_void(&source);
		for (u32 i = 0; i < Entity::component_constructors.count; ++i) {
			if (strncmp(source, custom::component_names[i], strlen(custom::component_names[i])) != 0) { continue; }
			Ref ref = entity.add_component(i);
			(*Entity::component_serialization_readers[i])(ref, &source, end);
		}
		skip_to_eol(&source); parse_eol(&source);
	}

	return entity;
}

u32 Entity::store_string(cstring data, u32 length) {
	return strings.store_string(data, length);
}

cstring Entity::get_string(u32 id) {
	return strings.get_string(id);
}

void Entity::destroy(void) {
	CUSTOM_ASSERT(exists(), "entity doesn't exist");

	u32 entity_offset = id * Entity::component_destructors.count;
	if (entity_offset < Entity::components.capacity) {
		for (u32 i = 0; i < Entity::component_destructors.count; ++i) {
			Ref const & ref = Entity::components.get(entity_offset + i);
			if ((*Entity::component_containers[i])(ref)) {
				(*Entity::component_destructors[i])(ref);
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

Entity Entity::copy() const {
	CUSTOM_ASSERT(exists(), "entity doesn't exist");

	Entity entity = create(true);

	u32 entity_offset = id * Entity::component_destructors.count;
	if (entity_offset < Entity::components.capacity) {
		for (u32 i = 0; i < Entity::component_containers.count; ++i) {
			Ref const & ref = Entity::components.get(entity_offset + i);
			if ((*Entity::component_containers[i])(ref)) {
				Ref new_component_ref = entity.add_component(i);
				(*Entity::component_copiers[i])(ref, new_component_ref);
			}
		}
	}

	return entity;
}

}

//
// entity components
//

#if defined(ENTITY_COMPONENTS_DENSE)

namespace custom {

static u32 find(u32 type, u32 entity, Array<u32> const & types, Array<u32> const & entities) {
	for (u32 i = 0; i < entities.count; ++i) {
		if (types[i] != type) { continue; }
		if (entities[i] == entity) { return i; }
	}
	return custom::empty_index;
}

Ref Entity::add_component(u32 type) {
	// @Change: ignore, but warn, if component exists?
	CUSTOM_ASSERT(exists(), "entity doesn't exist");

	u32 component_index = find(type, id, component_types, component_entity_ids);
	if (component_index == custom::empty_index) {
		component_index = Entity::components.count;
		Entity::components.push(custom::empty_ref);
		Entity::component_types.push(type);
		Entity::component_entity_ids.push(id);
	}

	Ref & ref = Entity::components[component_index];

	if (!(*Entity::component_containers[type])(ref)) {
		ref = (*Entity::component_constructors[type])();
	}
	else { CUSTOM_ASSERT(false, "component already exists"); }

	return ref;
}

void Entity::rem_component(u32 type) {
	// @Change: ignore, but warn, if component doesn't exist?
	CUSTOM_ASSERT(exists(), "entity doesn't exist");

	u32 component_index = find(type, id, component_types, component_entity_ids);
	if (component_index == custom::empty_index) {
		CUSTOM_ASSERT(false, "component doesn't exist"); return;
	}

	Ref ref = Entity::components[component_index];

	Entity::components.remove_at(component_index);
	Entity::component_types.remove_at(component_index);
	Entity::component_entity_ids.remove_at(component_index);

	if ((*Entity::component_containers[type])(ref)) {
		(*Entity::component_destructors[type])(ref);
	}
	else { CUSTOM_ASSERT(false, "component doesn't exist"); }
}

Ref Entity::get_component(u32 type) const {
	CUSTOM_ASSERT(exists(), "entity doesn't exist");

	u32 component_index = find(type, id, component_types, component_entity_ids);
	if (component_index == custom::empty_index) { return custom::empty_ref; }

	return Entity::components[component_index];
}

bool Entity::has_component(u32 type) const {
	CUSTOM_ASSERT(exists(), "entity doesn't exist");

	u32 component_index = find(type, id, component_types, component_entity_ids);
	if (component_index == custom::empty_index) { return false; }

	Ref const & ref = Entity::components[component_index];
	return (*Entity::component_containers[type])(ref);
}

}

#else

namespace custom {

Ref Entity::add_component(u32 type) {
	// @Change: ignore, but warn, if component exists?
	CUSTOM_ASSERT(exists(), "entity doesn't exist");

	// entity_components_ensure_capacity
	u32 capacity_before = Entity::components.capacity;
	Entity::components.ensure_capacity((id + 1) * Entity::component_constructors.count);
	for (u32 i = capacity_before; i < Entity::components.capacity; ++i) {
		Entity::components.data[i] = custom::empty_ref;
	}

	u32 component_index = id * Entity::component_constructors.count + type;
	Ref & ref = Entity::components.get(component_index);

	if (!(*Entity::component_containers[type])(ref)) {
		ref = (*Entity::component_constructors[type])();
	}
	else { CUSTOM_ASSERT(false, "component already exists"); }

	return ref;
}

void Entity::rem_component(u32 type) {
	// @Change: ignore, but warn, if component doesn't exist?
	CUSTOM_ASSERT(exists(), "entity doesn't exist");

	u32 component_index = id * Entity::component_destructors.count + type;
	if (component_index >= Entity::components.capacity) {
		CUSTOM_ASSERT(false, "component doesn't exist"); return;
	}

	Ref const & ref = Entity::components.get(component_index);

	if ((*Entity::component_containers[type])(ref)) {
		(*Entity::component_destructors[type])(ref);
	}
	else { CUSTOM_ASSERT(false, "component doesn't exist"); }
}

Ref Entity::get_component(u32 type) const {
	CUSTOM_ASSERT(exists(), "entity doesn't exist");

	u32 component_index = id * Entity::component_containers.count + type;
	if (component_index >= Entity::components.capacity) { return custom::empty_ref; }

	return Entity::components.get(component_index);
}

bool Entity::has_component(u32 type) const {
	CUSTOM_ASSERT(exists(), "entity doesn't exist");

	u32 component_index = id * Entity::component_containers.count + type;
	if (component_index >= Entity::components.capacity) { return false; }

	Ref const & ref = Entity::components.get(component_index);
	return (*Entity::component_containers[type])(ref);
}

}

#endif
