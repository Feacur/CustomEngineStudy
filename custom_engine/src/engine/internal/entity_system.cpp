#include "custom_pch.h"

#include "engine/impl/entity_system.h"

namespace custom {

//  @Note: initialize compile-time structs:
template struct Array<Entity>;
template struct Array<ref_void_func *>;
template struct Array<void_ref_func *>;
template struct Array<bool_ref_func *>;

//  @Note: initialize compile-time statics:
Gen_Pool               Entity::generations;
Array<Entity>          Entity::instances;
Array<ref_void_func *> Entity::component_constructors;
Array<bool_ref_func *> Entity::component_containers;
Array<void_ref_func *> Entity::component_destructors;
Array<Ref>             Entity::components;

//
// pool
//

Ref Gen_Pool::create(void) {
	u32 id;
	if (gaps.count > 0) {
		id = gaps[gaps.count - 1];
		gaps.pop();
	}
	else {
		id = gens.count;
		gens.push(0);
	}
	return { id, gens.get(id) };
}

void Gen_Pool::destroy(Ref const & ref) {
	// @Change: ignore, but warn, if data doesn't exist?
	CUSTOM_ASSERT(ref.gen == gens.get(ref.id), "destroying null data");
	++gens[ref.id];
	if (ref.id == gens.count - 1) {
		gens.pop();
	}
	else {
		gaps.push(ref.id);
	}
}

//
// entity
//

Entity Entity::create(void) {
	Ref ref = Entity::generations.create();
	instances.push({ref.id, ref.gen});
	return {ref.id, ref.gen};
}

void Entity::destroy(Entity const & entity) {
	CUSTOM_ASSERT(entity.exists(), "entity doesn't exist");

	u32 entity_offset = entity.id * Entity::component_destructors.count;
	if (entity_offset >= Entity::components.capacity) { return; }
	for (u32 i = 0; i < Entity::component_destructors.count; ++i) {
		Ref const & ref = Entity::components.get(entity_offset + i);
		(*Entity::component_destructors[i])(ref);
	}
	Entity::generations.destroy(entity);

	u32 instance_index = UINT32_MAX;
	for (u32 i = 0; i < instances.count; ++i) {
		if (instances[i].id != entity.id) { continue; }
		if (instances[i].gen != entity.gen) { continue; }
		instance_index = i; break;
	}

	if(instance_index < instances.count) {
		instances.remove_at(instance_index);
	}
	else {
		CUSTOM_WARNING("entity instance doesn't exist");
	}
}

//
// entity components
//

Ref Entity::add_component(u32 type) {
	// @Change: ignore, but warn, if component exists?
	CUSTOM_ASSERT(exists(), "entity doesn't exist");

	// entity_components_ensure_capacity
	u32 capacity_before = Entity::components.capacity;
	Entity::components.ensure_capacity((id + 1) * Entity::component_constructors.count);
	for (u32 i = capacity_before; i < Entity::components.capacity; ++i) {
		Entity::components.data[i] = {UINT32_MAX, UINT32_MAX};
	}

	u32 component_index = id * Entity::component_constructors.count + type;
	Ref & ref = Entity::components.get(component_index);

	CUSTOM_ASSERT(!(*Entity::component_containers[type])(ref), "component already exist");
	ref = (*Entity::component_constructors[type])();

	return {ref.id, ref.gen};
}

void Entity::rem_component(u32 type) {
	// @Change: ignore, but warn, if component doesn't exist?
	CUSTOM_ASSERT(exists(), "entity doesn't exist");

	u32 component_index = id * Entity::component_destructors.count + type;
	CUSTOM_ASSERT(component_index < Entity::components.capacity, "component doesn't exist");
	Ref const & ref = Entity::components.get(component_index);

	(*Entity::component_destructors[type])(ref);
}

bool Entity::has_component(u32 type) const {
	CUSTOM_ASSERT(exists(), "entity doesn't exist");

	u32 component_index = id * Entity::component_containers.count + type;
	if (component_index >= Entity::components.capacity) { return false; }
	Ref const & ref = Entity::components.get(component_index);

	return (*Entity::component_containers[type])(ref);
}

Ref Entity::get_component(u32 type) const {
	CUSTOM_ASSERT(exists(), "entity doesn't exist");

	u32 component_index = id * Entity::component_containers.count + type;
	if (component_index >= Entity::components.capacity) { return {UINT32_MAX, 0}; }
	Ref const & ref = Entity::components.get(component_index);

	return {ref.id, ref.gen};
}

}
