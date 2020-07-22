#pragma once
#include "engine/core/collection_types.h"

namespace custom {

//
// reference
//

struct Ref
{
	u32 id, gen;
};

template<typename T>
struct RefT : public Ref
{
	inline T * get_instance() { return T::pool.get_instance(id, gen); }
};

//
// pool
//

#define VOID_REF_FUNC(ROUTINE_NAME) void ROUTINE_NAME(u32 id, u32 gen)
typedef VOID_REF_FUNC(void_ref_func);

struct Gen_Pool
{
	Gen_Pool();
	~Gen_Pool() = default;

	Array<u32> gens; // sparse; count indicates the last active object
	Array<u32> gaps;

	// API
	Ref create();
	void destroy(u32 id, u32 gen);
	inline bool contains(u32 id, u32 gen) { return id && (id < gens.count) && (gens[id] == gen); };
};

// @Todo: might want to dynamically init pools should the code be used from a DLL?
//        not quite relates to the pool itself, but definitely to RefT<T> and
//        types/places that make use of it
template<typename T>
struct Ref_Pool
{
	Ref_Pool();
	~Ref_Pool() = default;

	Gen_Pool pool;
	Array<T> instances; // sparse; count indicates the last active object

	// API
	RefT<T> create();
	void destroy(u32 id, u32 gen);
	inline bool contains(u32 id, u32 gen) { return pool.contains(id, gen); };

	// Entity API
	static VOID_REF_FUNC(destroy_safe);

	// RefT<T> API
	inline T * get_instance(u32 id, u32 gen) { return pool.contains(id, gen) ? &instances[id] : NULL; }
};

//
// entity
//

struct Entity : public Ref
{
	// entities
	static Array<Entity> instances;
	static Gen_Pool pool;

	static Entity get(u32 index) { return instances[index]; }
	static Entity create();
	static void destroy(Entity entity);

	bool exists() const { return pool.contains(id, gen); }

	// components
	static Array<void_ref_func *> component_destructors;
	static Array<Ref> components; // sparse

	template<typename T> void add_component();
	template<typename T> void remove_component();
	template<typename T> bool has_component() const;
	template<typename T> RefT<T> get_component() const;
};

}
