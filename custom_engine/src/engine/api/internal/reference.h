#pragma once
#include "engine/core/collection_types.h"

namespace custom {
	template<typename T> struct Ref_Pool;
}

//
// reference
//

namespace custom {

struct Ref
{
	u32 id, gen;
};

inline bool operator==(Ref const & a, Ref const & b) {
	return a.id  == b.id
	    && a.gen == b.gen;
}

template<typename T>
struct RefT : public Ref
{
	static Ref_Pool<T> pool;

	static RefT<T> create(void) { return pool.create(); }
	static void destroy(Ref const & ref) { return pool.destroy(ref); }
	inline bool exists(void) const { return pool.contains(*this); }

	inline T * get_fast(void) { return pool.get_fast(*this); }
	inline T * get_safe(void) { return pool.get_safe(*this); }

	inline T const * get_fast(void) const { return pool.get_fast(*this); }
	inline T const * get_safe(void) const { return pool.get_safe(*this); }
};

}

//
// pool
//

namespace custom {
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
	inline bool contains(Ref const & ref) const { return (ref.id < gens.count) && (gens[ref.id] == ref.gen); };
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
	RefT<T> create(void);
	void destroy(Ref const & ref);

	// RefT API
	inline bool contains(Ref const & ref) const { return generations.contains(ref); };

	inline T * get_fast(Ref const & ref) { return &instances[ref.id]; }
	inline T * get_safe(Ref const & ref) { return generations.contains(ref) ? &instances[ref.id] : NULL; }

	inline T const * get_fast(Ref const & ref) const { return &instances[ref.id]; }
	inline T const * get_safe(Ref const & ref) const { return generations.contains(ref) ? &instances[ref.id] : NULL; }
};

}
