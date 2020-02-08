#pragma once
#include "reference.h"

namespace custom {

//
// entity
//

struct Plain_Ref
{
	u32 id, gen;
};

struct Entity
{
	u8 data; // @Note: explicit dummy data of a single byte size

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
