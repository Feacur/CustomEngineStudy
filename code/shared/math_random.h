#define SHARED_MATH_RANDOM

#if !defined(SHARED_MATH_HASHING)
	#error include "shared/math_scalar.h"
#endif

//
// integer types
//

#define random_uint32(STATE) hash_xorshift32(STATE)

#define RANDOM_IN_RANGE_IMPL(T)\
inline T random_in_range(uint32 * state, T min, T max) {\
	return min + (T)(random_uint32(state) % (uint32)(max - min));\
}

RANDOM_IN_RANGE_IMPL(int8)
RANDOM_IN_RANGE_IMPL(int16)
RANDOM_IN_RANGE_IMPL(int32)
RANDOM_IN_RANGE_IMPL(int64)
RANDOM_IN_RANGE_IMPL(uint8)
RANDOM_IN_RANGE_IMPL(uint16)
RANDOM_IN_RANGE_IMPL(uint32)
RANDOM_IN_RANGE_IMPL(uint64)

RANDOM_IN_RANGE_IMPL(int48)
RANDOM_IN_RANGE_IMPL(uint48)
#undef RANDOM_IN_RANGE_IMPL

//
// floating point types
//

#define random_01(STATE) hash_01(STATE)
// inline float random_01(uint32 * state) { return (float)random_uint32(state) / UINT32_MAX; }

#define random_radius01(STATE) hash_radius01(STATE)
// inline float random_radius01(uint32 * state) { return random_01(state) * 2 - 1; }

inline float random_in_range(uint32 * state, float min, float max) {
	return min + (max - min) * random_01(state);
}
