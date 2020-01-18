#define SHARED_MATH_RANDOM
#if !defined(SHARED_MATH_HASHING)
	#error include "shared/math_scalar.h"
#endif

// http://www.iquilezles.org/www/articles/sfrand/sfrand.htm

//
// integer types
//

inline u32 random_uint32(u32 * state) {
	return *state = hash_xorshift32(*state);
}

#define RANDOM_IN_RANGE_IMPL(T)\
inline T random_in_range(u32 * state, T min, T max) {\
	return min + (T)(random_uint32(state) % (u32)(max - min));\
}

RANDOM_IN_RANGE_IMPL(s8)
RANDOM_IN_RANGE_IMPL(s16)
RANDOM_IN_RANGE_IMPL(s32)
RANDOM_IN_RANGE_IMPL(s64)
RANDOM_IN_RANGE_IMPL(u8)
RANDOM_IN_RANGE_IMPL(u16)
RANDOM_IN_RANGE_IMPL(u32)
RANDOM_IN_RANGE_IMPL(u64)

RANDOM_IN_RANGE_IMPL(s48)
RANDOM_IN_RANGE_IMPL(u48)
#undef RANDOM_IN_RANGE_IMPL

//
// floating point types
//

inline r32 random_01(u32 * state) {
	return uint32_bits_to_01(*state = hash_iquilez(*state));
}
// inline r32 random_01(u32 * state) { return (r32)random_uint32(state) / UINT32_MAX; }

inline r32 random_radius01(u32 * state) {
	return uint32_bits_to_radius01(*state = hash_iquilez(*state));
}
// inline r32 random_radius01(u32 * state) { return random_01(state) * 2 - 1; }

inline r32 random_in_range(u32 * state, r32 min, r32 max) {
	return min + (max - min) * random_01(state);
}
