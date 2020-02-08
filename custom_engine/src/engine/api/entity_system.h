#pragma once
#include "engine/impl/array.h"

#include <new>

namespace custom {

//
// reference
//

template<typename T>
struct Ref
{
	u32 id, gen;

	Ref()
		: id(0)
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
			id = 0;
		}
	}

	Ref<T> & operator=(T const * instance)
	{
		if (instance) {
			id = T::pool.get_id(instance);
			gen = T::pool.get_gen(id);
		}
		else {
			id = 0;
		}
		return *this;
	}

	bool operator==(Ref<T> const & other)
	{
		return id == other.id && gen = other.gen;
	}

	T * operator->()
	{
		if (!id) { return NULL; }
		T * instance = T::pool.get_instance(id);
		return gen == T::pool.get_gen(id) ? instance : NULL;
	}
};

struct Ref_Pool_Base
{
	virtual void destroy(u32 id, u32 gen) = 0;
};

template<typename T>
struct Ref_Pool : public Ref_Pool_Base
{
	Ref_Pool()
	{
		Ref<T> null_instance = create();
	}

	// SoA data
	Array<T> instances;
	Array<u32> gens;

	// in-between data
	Array<u32> gaps;

	// API
	Ref<T> create();
	void destroy(Ref<T> ref);
	void destroy(u32 id, u32 gen) override;
	bool contains(u32 id, u32 gen) { gens[id] == gen; };

	// Ref<T> API
	u32 get_id(T const * instance) const { return (u32)(instance - instances.data); }
	T * get_instance(u32 id) { return &instances[id]; }
	u32 get_gen(u32 id) const { return gens[id]; }
};

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
	}
	return { id, gens[id] };
}

template<typename T>
void Ref_Pool<T>::destroy(Ref<T> ref) {
	CUSTOM_ASSERT(ref.gen == gens[ref.id], "destroying null data");
	if (ref.id == instances.count - 1) {
		instances.pop();
		gens.pop();
	}
	else {
		gaps.push(ref.id);
	}
	++gens[ref.id];
}

template<typename T>
void Ref_Pool<T>::destroy(u32 id, u32 gen) {
	destroy({ id, gen });
}

//
// entity
//

struct Plain_Ref
{
	u32 id, gen;
};

struct Entity
{
	u8 data;

	// components
	static u32 component_types_count;
	static Array<Plain_Ref> components;

	template<typename T> void add_component();
	template<typename T> void remove_component();
	template<typename T> bool has_component() const;
	template<typename T> Ref<T> get_component();

	// Ref<T> API, creation
	static Ref_Pool<Entity> pool;
};

template<typename T>
void Entity::add_component() {
	u32 id = Entity::pool.get_id(this);
	u32 id_offset = id * Entity::component_types_count + T::offset;
	Entity::components.ensure_capacity((id + 1) * Entity::component_types_count);
	Plain_Ref & c_ref = Entity::components[id_offset];
	CUSTOM_ASSERT(!c_ref.id, "component already exist");
	if (!c_ref.id) {
		Ref<T> ref = T::pool.create();
		c_ref = { ref.id, ref.gen };
	}
}

template<typename T>
void Entity::remove_component() {
	u32 id = Entity::pool.get_id(this);
	u32 id_offset = id * Entity::component_types_count + T::offset;
	if (Entity::components.capacity <= id_offset) { return; }
	Plain_Ref & c_ref = Entity::components[id_offset];
	CUSTOM_ASSERT(c_ref.id, "component doesn't exist");
	if (c_ref.id) {
		T::pool.destroy({ c_ref.id, c_ref.gen });
		c_ref.id = 0;
	}
}

template<typename T>
bool Entity::has_component() const {
	u32 id = Entity::pool.get_id(this);
	u32 id_offset = id * Entity::component_types_count + T::offset;
	if (Entity::component_ids.capacity <= id_offset) { return false; }
	Plain_Ref & c_ref = Entity::component_ids[id_offset];
	return T::pool.contains(c_ref.id, c_ref.gen);
}

template<typename T>
Ref<T> Entity::get_component() {
	u32 id = Entity::get_id(this);
	u32 id_offset = id * Entity::component_types_count + T::offset;
	if (Entity::component_ids.capacity <= id_offset) { return NULL; }
	Plain_Ref & c_ref = Entity::component_ids[id_offset];
	return { c_ref.id, c_ref.gen };
}

//
// world, a utility over entities
//

struct World
{
	static Array<Ref_Pool_Base *> component_pools;
	static Ref<Entity> create();
	static void destroy(Ref<Entity> ref);
};

}
