#pragma once
#include "engine/api/internal/entity_system.h"
#include "engine/impl/array.h"

#include <new>

// https://github.com/etodd/lasercrabs/blob/master/src/data/entity.h

// @Note: relies on the fact, that Array<T> is zero-initialized by default
//        - leave it as is?
//        - provide Array<T> with a default value?
//        - provide each Entity with a bitfield for components?

namespace custom {

//
// pool
//

template<typename T>
template<typename... Args>
RefT<T> Ref_Pool<T>::create(Args... args) {
	if (generations.gaps.count == 0) { instances.push(); }
	Ref ref = generations.create();
	new (&instances.data[ref.id]) T (args...);
	return {ref.id, ref.gen};
}

template<typename T>
void Ref_Pool<T>::destroy(Ref const & ref) {
	generations.destroy(ref);
	if (ref.id == instances.count - 1) { instances.pop(); }
}

template<typename T> VOID_REF_FUNC(ref_pool_destruct) {
	if (RefT<T>::pool.contains(ref)) { RefT<T>::pool.destroy(ref); }
}

//
// entity
//

template<typename T, typename... Args>
RefT<T> Entity::add_component(Args... args) {
	// @Change: ignore, but warn, if component exists?
	CUSTOM_ASSERT(exists(), "entity doesn't exist");

	// entity_components_ensure_capacity
	u32 capacity_before = Entity::components.capacity;
	Entity::components.ensure_capacity((id + 1) * Entity::component_destructors.count);
	for (u32 i = capacity_before; i < Entity::components.capacity; ++i) {
		Entity::components.data[i] = {UINT32_MAX, UINT32_MAX};
	}

	u32 component_index = id * Entity::component_destructors.count + Component_Registry<T>::offset;
	Ref & ref = Entity::components.get(component_index);

	CUSTOM_ASSERT(!RefT<T>::pool.contains(ref), "component already exist");
	ref = RefT<T>::pool.create(args...);
	++Entity::components.count;

	return {ref.id, ref.gen};
}

template<typename T>
void Entity::rem_component(void) {
	// @Change: ignore, but warn, if component doesn't exist?
	CUSTOM_ASSERT(exists(), "entity doesn't exist");

	u32 component_index = id * Entity::component_destructors.count + Component_Registry<T>::offset;
	CUSTOM_ASSERT(component_index < Entity::components.capacity, "component doesn't exist");
	Ref const & ref = Entity::components.get(component_index);

	RefT<T>::pool.destroy(ref);
	--Entity::components.count;
}

template<typename T>
bool Entity::has_component(void) const {
	CUSTOM_ASSERT(exists(), "entity doesn't exist");

	u32 component_index = id * Entity::component_destructors.count + Component_Registry<T>::offset;
	if (component_index >= Entity::components.capacity) { return false; }
	Ref const & ref = Entity::components.get(component_index);

	return RefT<T>::pool.contains(ref);
}

template<typename T>
RefT<T> Entity::get_component(void) const {
	CUSTOM_ASSERT(exists(), "entity doesn't exist");

	u32 component_index = id * Entity::component_destructors.count + Component_Registry<T>::offset;
	if (component_index >= Entity::components.capacity) { return {UINT32_MAX, 0}; }
	Ref const & ref = Entity::components.get(component_index);

	return {ref.id, ref.gen};
}

}
