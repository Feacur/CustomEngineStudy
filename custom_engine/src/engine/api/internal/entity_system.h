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

template<typename T> struct Ref_Pool;

template<typename T>
struct RefT : public Ref
{
	static Ref_Pool<T> pool;
	inline T * get_fast(void) { return pool.get_fast(*this); }
	inline T * get_safe(void) { return pool.get_safe(*this); }
};

//
// pool
//

#define VOID_REF_FUNC(ROUTINE_NAME) void ROUTINE_NAME(Ref const & ref)
typedef VOID_REF_FUNC(void_ref_func);

// @Todo: factor out sparse array functionality?
// @GEAB: replace `gaps` with a `next_free` pointer
//        although, might be not the valid case here if `gens` needs to be consistent
struct Gen_Pool
{
	~Gen_Pool(void) = default;

	Array<u32> gens; // sparse; count indicates the last active object
	Array<u32> gaps;

	// API
	Ref create(void);
	void destroy(Ref const & ref);
	inline bool contains(Ref const & ref) { return (ref.id < gens.count) && (gens[ref.id] == ref.gen); };
};

// @Todo: might want to dynamically init pools should the code be used from a DLL?
//        not quite relates to the pool itself, but definitely to RefT and
//        types/places that make use of it
template<typename T>
struct Ref_Pool
{
	~Ref_Pool(void) = default;

	Gen_Pool generations;
	Array<T> instances; // sparse; count indicates the last active object

	// API
	template<typename... Args> RefT<T> create(Args... args);
	void destroy(Ref const & ref);
	inline bool contains(Ref const & ref) { return generations.contains(ref); };

	// RefT API
	inline T * get_fast(Ref const & ref) { return &instances[ref.id]; }
	inline T * get_safe(Ref const & ref) { return generations.contains(ref) ? &instances[ref.id] : NULL; }
};

//
// entity
//

template<typename T> struct Component_Registry { static u32 offset; };

struct Entity : public Ref
{
	// entities
	static Gen_Pool generations;
	static Array<Entity> instances;

	// API
	static Entity create(void);
	static void destroy(Entity const & entity);
	inline bool exists(void) const { return generations.contains(*this); }

	// components API
	static Array<void_ref_func *> component_destructors; // count indicates the number of component types
	static Array<Ref> components; // sparse; count indicates number of active components

	template<typename T, typename... Args> RefT<T> add_component(Args... args);
	template<typename T> void remove_component(void);
	template<typename T> bool has_component(void) const;
	template<typename T> RefT<T> get_component(void) const;
};

}
