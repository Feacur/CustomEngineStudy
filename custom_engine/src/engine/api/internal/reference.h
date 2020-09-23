#pragma once
#include "engine/core/collection_types.h"

namespace custom {
	// @Forward
	template<typename T> struct Ref_PoolT;
}

//
// reference
//

namespace custom {

// @Change: replace with a u64? with a union? with a struct of bit fields?
//          manually do bitwise operations? use smaller types?
struct Ref {u32 id, gen;};

constexpr Ref const empty_ref = {UINT32_MAX, UINT32_MAX};

inline bool operator==(Ref const & a, Ref const & b) {
	return a.id == b.id && a.gen == b.gen;
}

inline bool operator!=(Ref const & a, Ref const & b) {
	return a.id != b.id || a.gen != b.gen;
}

template<typename T>
struct RefT : public Ref
{
	// @Todo: factor this state out, too
	static Ref_PoolT<T> pool;

	inline static RefT<T> create(void) { return RefT<T>::pool.create(); }
	inline void destroy(void) { return RefT<T>::pool.destroy(*this); }
	inline bool exists(void) const { return RefT<T>::pool.contains(*this); }

	inline T * get_fast(void) { return RefT<T>::pool.get_fast(*this); }
	inline T * get_safe(void) { return RefT<T>::pool.get_safe(*this); }

	inline T const * get_fast(void) const { return RefT<T>::pool.get_fast(*this); }
	inline T const * get_safe(void) const { return RefT<T>::pool.get_safe(*this); }
};
template<typename T> Ref_PoolT<T> RefT<T>::pool;

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
	// @Todo: provide a relocation table, so that the data can be compressed?

	// API
	Ref create(void);
	void destroy(Ref const & ref);
	inline bool contains(Ref const & ref) const { return (ref.id < gens.count) && (gens[ref.id] == ref.gen); };
};

struct Ref_Pool
{
	Ref_Pool(u32 size);
	~Ref_Pool(void) = default;

	Gen_Pool generations;
	Array<u8> buffer; // sparse; count indicates the last active object
	u32 const size;

	// API
	Ref create_void(void);
	void destroy(Ref const & ref);

	// RefT API
	inline bool contains(Ref const & ref) const { return generations.contains(ref); };

	inline u8 * get_fast_void(Ref const & ref) { return &buffer[ref.id * size]; }
	inline u8 * get_safe_void(Ref const & ref) { return generations.contains(ref) ? &buffer[ref.id * size] : NULL; }

	inline u8 const * get_fast_void(Ref const & ref) const { return &buffer[ref.id * size]; }
	inline u8 const * get_safe_void(Ref const & ref) const { return generations.contains(ref) ? &buffer[ref.id * size] : NULL; }
};

// @Todo: might want to dynamically init pools should the code be used from a DLL?
//        not quite relates to the pool itself, but definitely to RefT and
//        types/places that make use of it
template<typename T>
struct Ref_PoolT : public Ref_Pool
{
	Ref_PoolT(void);
	~Ref_PoolT(void) = default;

	// API
	inline RefT<T> create(void) { return {create_void()}; }

	// RefT API
	inline T * get_fast(Ref const & ref) { return (T *)get_fast_void(ref); }
	inline T * get_safe(Ref const & ref) { return (T *)get_safe_void(ref); }

	inline T const * get_fast(Ref const & ref) const { return (T const *)get_fast_void(ref); }
	inline T const * get_safe(Ref const & ref) const { return (T const *)get_safe_void(ref); }
};

}

//
// universal access
//

// @Change: use byte arrays instead?
namespace custom {

#define REF_VOID_FUNC(ROUTINE_NAME) Ref ROUTINE_NAME(void)
typedef REF_VOID_FUNC(ref_void_func);

#define VOID_REF_FUNC(ROUTINE_NAME) void ROUTINE_NAME(Ref const & ref)
typedef VOID_REF_FUNC(void_ref_func);

#define BOOL_REF_FUNC(ROUTINE_NAME) bool ROUTINE_NAME(Ref const & ref)
typedef BOOL_REF_FUNC(bool_ref_func);

}
