#pragma once
#include "engine/api/internal/entity_system.h"
#include "engine/impl/array.h"

// #include <new>

// @Note: I'm yet to provide possibility of components construction
//        with parameters, if needed; until then there is no need
//        to include <new>, as it's required for the placement new operator

// https://github.com/etodd/lasercrabs/blob/master/src/data/entity.h

// @Note: relies on the fact, that Array<T> is zero-initialized by default
//        - leave it as is?
//        - provide Array<T> with a default value?
//        - provide each Entity with a bitfield for components?
constexpr u32 const empty_ref_id = 0;

namespace custom {

//
// pool
//

template<typename T>
Ref_Pool<T>::Ref_Pool() {
	// @Note: it's required for the current implementation where 0 is a default and invalid id
	instances.push();
}

template<typename T>
RefT<T> Ref_Pool<T>::create() {
	if (pool.gaps.count == 0) { instances.push(); }
	Ref ref = pool.create();
	return {ref.id, ref.gen};
}

template<typename T>
void Ref_Pool<T>::destroy(Ref const & ref) {
	pool.destroy(ref);
	if (ref.id == instances.count - 1) { instances.pop(); }
}

template<typename T> VOID_REF_FUNC(ref_pool_destruct) {
	if (T::pool.contains(ref)) { T::pool.destroy(ref); }
}

//
// entity
//

template<typename T>
void Entity::add_component(void) {
	CUSTOM_ASSERT(exists(), "entity doesn't exist");
	u32 component_index = id * Entity::component_destructors.count + T::offset;
	Entity::components.ensure_capacity((id + 1) * Entity::component_destructors.count);
	Ref & ref = Entity::components.get(component_index);
	CUSTOM_ASSERT(!T::pool.contains(ref), "component already exist");
	ref = T::pool.create();
}

template<typename T>
void Entity::remove_component(void) {
	CUSTOM_ASSERT(exists(), "entity doesn't exist");
	u32 component_index = id * Entity::component_destructors.count + T::offset;
	if (Entity::components.capacity <= component_index) { return; }
	Ref const & ref = Entity::components.get(component_index);
	T::pool.destroy(ref);
}

template<typename T>
bool Entity::has_component(void) const {
	CUSTOM_ASSERT(exists(), "entity doesn't exist");
	u32 component_index = id * Entity::component_destructors.count + T::offset;
	if (Entity::components.capacity <= component_index) { return false; }
	Ref const & ref = Entity::components.get(component_index);
	return T::pool.contains(ref);
}

template<typename T>
RefT<T> Entity::get_component(void) const {
	CUSTOM_ASSERT(exists(), "entity doesn't exist");
	u32 component_index = id * Entity::component_destructors.count + T::offset;
	if (Entity::components.capacity <= component_index) { return {empty_ref_id, 0}; }
	Ref const & ref = Entity::components.get(component_index);
	return {ref.id, ref.gen};
}

}
