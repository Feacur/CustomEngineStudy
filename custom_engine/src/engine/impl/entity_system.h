#pragma once
#include "engine/api/entity_system.h"
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
// reference
//

template<typename T>
Ref<T>::Ref()
	: id(empty_ref_id)
	, gen(0)
{ }

template<typename T>
Ref<T>::Ref(u32 id, u32 gen)
	: id(id)
	, gen(gen)
{ }

template<typename T>
Ref<T>::Ref(T const * instance) {
	operator=(instance);
}

template<typename T>
Ref<T> & Ref<T>::operator=(T const * instance) {
	if (instance) {
		id = T::pool.get_id(instance);
		gen = T::pool.get_gen(id);
	}
	else {
		id = empty_ref_id;
	}
	return *this;
}

template<typename T>
bool Ref<T>::operator==(Ref<T> const & other) {
	return id == other.id && gen == other.gen;
}

template<typename T>
T * Ref<T>::operator->() {
	return T::pool.contains(id, gen) ? T::pool.get_instance(id) : NULL;
}

//
// pool
//

template<typename T>
Ref_Pool<T>::Ref_Pool() {
	// @Note: it's required for the current implementation where 0 is a default and invalid id
	Ref<T> null_instance = create();
}

template<typename T>
Ref<T> Ref_Pool<T>::create() {
	u32 id;
	if (gaps.count > 0) {
		gaps.pop();
		id = gaps[gaps.count];
	}
	else {
		id = instances.count;
		instances.push();
		gens.push();
		active.push();
	}
	active[id] = true;
	return { id, gens[id] };
}

template<typename T>
void Ref_Pool<T>::destroy(Ref<T> ref) {
	CUSTOM_ASSERT(ref.gen == gens[ref.id], "destroying null data");
	if (ref.id == instances.count - 1) {
		instances.pop();
		gens.pop();
		active.pop();
	}
	else {
		gaps.push(ref.id);
	}
	active[ref.id] = false;
	++gens[ref.id];
}

template<typename T>
void Ref_Pool<T>::destroy_safe(u32 id, u32 gen) {
	if (contains(id, gen)) {
		destroy({ id, gen });
	}
}

//
// entity
//

template<typename T>
void Entity::add_component(void) {
	u32 entity_id = Entity::pool.get_id(this);
	u32 component_id = entity_id * Entity::component_pools.count + T::offset;
	Entity::components.ensure_capacity((entity_id + 1) * Entity::component_pools.count);
	Plain_Ref & c_ref = Entity::components[component_id];
	CUSTOM_ASSERT(!T::pool.contains(c_ref.id, c_ref.gen), "component already exist");
	Ref<T> ref = T::pool.create();
	c_ref = { ref.id, ref.gen };
}

template<typename T>
void Entity::remove_component(void) {
	u32 entity_id = Entity::pool.get_id(this);
	u32 component_id = entity_id * Entity::component_pools.count + T::offset;
	if (Entity::components.capacity <= component_id) { return; }
	Plain_Ref & c_ref = Entity::components[component_id];
	CUSTOM_ASSERT(T::pool.contains(c_ref.id, c_ref.gen), "component doesn't exist");
	T::pool.destroy({ c_ref.id, c_ref.gen });
	// c_ref = {}; // @Note: relevant if [gen] value is not stored
}

template<typename T>
bool Entity::has_component(void) const {
	u32 entity_id = Entity::pool.get_id(this);
	u32 component_id = entity_id * Entity::component_pools.count + T::offset;
	if (Entity::components.capacity <= component_id) { return false; }
	Plain_Ref & c_ref = Entity::components[component_id];
	return T::pool.contains(c_ref.id, c_ref.gen);
}

template<typename T>
Ref<T> Entity::get_component(void) {
	u32 entity_id = Entity::pool.get_id(this);
	u32 component_id = entity_id * Entity::component_pools.count + T::offset;
	if (Entity::components.capacity <= component_id) { return {empty_ref_id, 0}; }
	Plain_Ref & c_ref = Entity::components[component_id];
	return { c_ref.id, c_ref.gen };
}

}
