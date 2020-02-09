#pragma once
#include "engine/api/entity_system.h"
#include "engine/impl/array.h"

// #include <new>

namespace custom {

//
// reference
//

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
		id = 0;
	}
	return *this;
}

template<typename T>
bool Ref<T>::operator==(Ref<T> const & other) {
	return id == other.id && gen = other.gen;
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
void Entity::add_component() {
	u32 id = Entity::pool.get_id(this);
	u32 id_offset = id * Entity::component_types_count + T::offset;
	Entity::components.ensure_capacity((id + 1) * Entity::component_types_count);
	Plain_Ref & c_ref = Entity::components[id_offset];
	CUSTOM_ASSERT(!T::pool.contains(c_ref.id, c_ref.gen), "component already exist");
	Ref<T> ref = T::pool.create();
	c_ref = { ref.id, ref.gen };
}

template<typename T>
void Entity::remove_component() {
	u32 id = Entity::pool.get_id(this);
	u32 id_offset = id * Entity::component_types_count + T::offset;
	if (Entity::components.capacity <= id_offset) { return; }
	Plain_Ref & c_ref = Entity::components[id_offset];
	CUSTOM_ASSERT(T::pool.contains(c_ref.id, c_ref.gen), "component doesn't exist");
	T::pool.destroy({ c_ref.id, c_ref.gen });
	// c_ref = {}; // @Note: relevant if [gen] value is not stored
}

template<typename T>
bool Entity::has_component() const {
	u32 id = Entity::pool.get_id(this);
	u32 id_offset = id * Entity::component_types_count + T::offset;
	if (Entity::components.capacity <= id_offset) { return false; }
	Plain_Ref & c_ref = Entity::components[id_offset];
	return T::pool.contains(c_ref.id, c_ref.gen);
}

template<typename T>
Ref<T> Entity::get_component() {
	u32 id = Entity::pool.get_id(this);
	u32 id_offset = id * Entity::component_types_count + T::offset;
	if (Entity::components.capacity <= id_offset) { return {}; }
	Plain_Ref & c_ref = Entity::components[id_offset];
	return { c_ref.id, c_ref.gen };
}

}
