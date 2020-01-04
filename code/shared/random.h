#define SHARED_RANDOM

#if !defined(SHARED_CODE)
	#error include "shared/code.h"
#endif

// https://en.wikipedia.org/wiki/Xorshift
// https://en.wikipedia.org/wiki/Lehmer_random_number_generator
// http://www.iquilezles.org/www/articles/sfrand/sfrand.htm

inline uint32 hash_xorshift32(uint32 * state) {
	uint32 x = *state;
	x ^= x << 13U; // hash
	x ^= x >> 17U; // hash
	x ^= x << 15U; // hash
	return (*state = x);
}

inline uint32 hash_lehmer32(uint32 * state) {
	static uint64 const multiplier = 0x10a860c1UL; // 0x0000bc8fUL
	static uint64 const modulo     = 0xfffffffbUL; // 0x7fffffffUL
	uint64 x = *state;
	x = (x * multiplier) % modulo; // hash
	return (*state = (uint32)x);
}

inline float hash_01(uint32 * state) {
	union { uint32 x; float xf; };  // local unionized values
	x = (*state = *state * 16807U); // hash
	// @Note: might well mask fractional part with [0x007fffffU]
	x = (x >> 9) | 0x3f800000U;     // clamp to [1 .. 2) * (2^0)
	return xf - 1;                  // return [1 .. 2) - 1
}

inline float hash_radius01(uint32 * state) {
	union { uint32 x; float xf; };  // local unionized values
	x = (*state = *state * 16807U); // hash
	// @Note: might well mask fractional part with [0x007fffffU]
	x = (x >> 9) | 0x40000000U;     // clamp to [1 .. 2) * (2^1)
	return xf - 3;                  // return [2 .. 4) - 3
}

//
// integer types
//

#define random_uint32(STATE) hash_xorshift32(STATE)

#define RANDOM_IN_RANGE(T)\
inline T random_in_range(uint32 * state, T min, T max) {\
	return min + (T)(random_uint32(state) % (uint32)(max - min));\
}

RANDOM_IN_RANGE(int8)
RANDOM_IN_RANGE(int16)
RANDOM_IN_RANGE(int32)
RANDOM_IN_RANGE(int64)
RANDOM_IN_RANGE(uint8)
RANDOM_IN_RANGE(uint16)
RANDOM_IN_RANGE(uint32)
RANDOM_IN_RANGE(uint64)

RANDOM_IN_RANGE(int48)
RANDOM_IN_RANGE(uint48)
#undef RANDOM_IN_RANGE

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

//
//
//

#if defined(SHARED_MATH)
inline Vector2 random2_01(uint32 * state) { return {random_01(state), random_01(state)}; }
inline Vector3 random3_01(uint32 * state) { return {random_01(state), random_01(state), random_01(state)}; }
inline Vector4 random4_01(uint32 * state) { return {random_01(state), random_01(state), random_01(state), random_01(state)}; }

#define RANDOM_RADIUS(T, name)\
inline T name##_radius01(uint32 * state) {\
	T point;\
	do {\
		point = name##_01(state) * 2 - 1;\
	} while (magnitude_squared(point) > 1);\
	return point;\
}

RANDOM_RADIUS(Vector2, random2)
RANDOM_RADIUS(Vector3, random3)
RANDOM_RADIUS(Vector4, random4)
#undef RANDOM_RADIUS
#else
	#pragma message("no random functions declared for the vector types\n\t---> " FILE_AND_LINE)
#endif
