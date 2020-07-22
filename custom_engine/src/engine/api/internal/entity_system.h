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
	inline T * get_instance() { return T::pool.get_instance(*this); }
};

//
// pool
//

#define VOID_REF_FUNC(ROUTINE_NAME) void ROUTINE_NAME(Ref const & ref)
typedef VOID_REF_FUNC(void_ref_func);

struct Gen_Pool
{
	Gen_Pool();
	~Gen_Pool() = default;

	Array<u32> gens; // sparse; count indicates the last active object
	Array<u32> gaps;

	// API
	Ref create();
	void destroy(Ref const & ref);
	inline bool contains(Ref const & ref) { return ref.id && (ref.id < gens.count) && (gens[ref.id] == ref.gen); };
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
	void destroy(Ref const & ref);
	inline bool contains(Ref const & ref) { return pool.contains(ref); };

	// RefT<T> API
	inline T * get_instance(Ref const & ref) { return pool.contains(ref) ? &instances[ref.id] : NULL; }
};

//
// entity
//

struct Entity : public Ref
{
	// entities
	static Gen_Pool pool;
	static Array<Entity> instances;

	// API
	static Entity create();
	static void destroy(Entity const & entity);
	inline bool exists() const { return pool.contains(*this); }

	// components API
	static Array<void_ref_func *> component_destructors;
	static Array<Ref> components; // sparse

	template<typename T> void add_component();
	template<typename T> void remove_component();
	template<typename T> bool has_component() const;
	template<typename T> RefT<T> get_component() const;
};

}
