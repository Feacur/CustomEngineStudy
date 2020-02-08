#pragma once
#include "engine/impl/array.h"

#include <new>

namespace custom {

constexpr u32 const empty_id = UINT32_MAX;

struct Ref_Raw
{
	u32 id, gen;
};

template<typename T>
struct Ref
{
	u32 id, gen;

	Ref()
		: id(empty_id)
		, gen(0)
	{ }

	Ref(u32 id, u32 gen)
		: id(id)
		, gen(gen)
	{ }

	Ref(T const * instance)
	{
		if (instance) {
			id = T::pool.get_id(instance);
			gen = T::pool.get_gen(id);
		}
		else {
			id = empty_id;
		}
	}

	Ref<T> & operator=(T const * instance)
	{
		if (instance) {
			id = T::pool.get_id(instance);
			gen = T::pool.get_gen(id);
		}
		else {
			id = empty_id;
		}
		return *this;
	}

	bool operator==(Ref<T> const & other)
	{
		return id == other.id && gen = other.gen;
	}

	T * operator->()
	{
		if (id == empty_id) { return NULL; }
		T * instance = T::pool.get_instance(id);
		return T::pool.get_active(id) && gen == T::pool.get_gen(id) ? instance : NULL;
	}
};

struct RefPoolBase
{
	virtual void destroy(u32 id, u32 gen) = 0;
};

template<typename T>
struct RefPool : public RefPoolBase
{
	// SoA data
	Array<T> instances;
	Array<u32> gens;
	Array<bool> active;

	// in-between data
	Array<u32> gaps;

	// API
	Ref<T> create();
	void destroy(Ref<T> ref);
	void destroy(u32 id, u32 gen) override;
	Ref<T> get_ref(u32 id) { return { id, gens[id] }; }

	// Ref<T> API
	u32 get_id(T const * instance) const { return (u32)(instance - instances.data); }
	T * get_instance(u32 id) { return &instances[id]; }
	u32 get_gen(u32 id) const { return gens[id]; }
	bool get_active(u32 id) const { return active[id]; }
};

template<typename T>
Ref<T> RefPool<T>::create() {
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
	return get_ref(id);
}

template<typename T>
void RefPool<T>::destroy(Ref<T> ref) {
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
void RefPool<T>::destroy(u32 id, u32 gen) {
	destroy({ id, gen });
}

struct Entity
{
	u8 data;

	// components
	static u32 component_types_count;
	static Array<Ref_Raw> components;

	template<typename T> void add_component();
	template<typename T> void remove_component();
	template<typename T> bool has_component() const;
	template<typename T> Ref<T> get_component();

	// Ref<T> API, creation
	static RefPool<Entity> pool;
};

template<typename T>
void Entity::add_component() {
	u32 id = Entity::pool.get_id(this);
	u32 id_offset = id * Entity::component_types_count + T::offset;
	Entity::components.ensure_capacity((id + 1) * Entity::component_types_count);
	Ref_Raw & ref_raw = Entity::components[id_offset];
	if (ref_raw.id == empty_id) {
		Ref<T> ref = T::pool.create();
		ref_raw = { ref.id, ref.gen };
	}
}

template<typename T>
void Entity::remove_component() {
	u32 id = Entity::pool.get_id(this);
	u32 id_offset = id * Entity::component_types_count + T::offset;
	if (Entity::components.capacity <= id_offset) { return; }
	Ref_Raw & ref_raw = Entity::components[id_offset];
	if (ref_raw.id != empty_id) {
		ref_raw.id = empty_id;
		T::pool.destroy({ ref_raw.id, ref_raw.gen });
	}
}

template<typename T>
bool Entity::has_component() const {
	u32 id = Entity::pool.get_id(this);
	u32 id_offset = id * Entity::component_types_count + T::offset;
	if (Entity::component_ids.capacity <= id_offset) { return false; }
	Ref_Raw & ref_raw = Entity::component_ids[id_offset];
	return ref_raw.id != empty_id;
}

template<typename T>
Ref<T> Entity::get_component() {
	u32 id = Entity::get_id(this);
	u32 id_offset = id * Entity::component_types_count + T::offset;
	if (Entity::component_ids.capacity <= id_offset) { return NULL; }
	Ref_Raw & ref_raw = Entity::component_ids[id_offset];
	return T::pool.get_ref(ref_raw.id);
}

struct World
{
	static Array<RefPoolBase *> component_pools;
	static Ref<Entity> create();
	static void destroy(Ref<Entity> ref);
};

}
