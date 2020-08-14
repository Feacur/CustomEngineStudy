#include "custom_pch.h"

#include "engine/api/internal/entity_system.h"
#include "engine/impl/array.h"

namespace custom {

//  @Note: initialize compile-time structs:
template struct Array<Entity>;

//  @Note: initialize compile-time statics:
Gen_Pool               Entity::generations;
Array<Entity>          Entity::instances;
Array<ref_void_func *> Entity::component_constructors;
Array<void_ref_func *> Entity::component_destructors;
Array<bool_ref_func *> Entity::component_containers;
Array<Ref>             Entity::components;

}

//
// entity
//

namespace custom {

Entity Entity::create(void) {
	Ref ref = Entity::generations.create();
	instances.push({ref.id, ref.gen});
	return {ref.id, ref.gen};
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
		if (instances[i].id != id) { continue; }
		if (instances[i].gen != gen) { continue; }
		instances.remove_at(i);
		break;
	}
}

}

//
// entity components
//

namespace custom {

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
	if (component_index >= Entity::components.capacity) { return {UINT32_MAX, 0}; }

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
