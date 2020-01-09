#define SHARED_MATH_HASHING

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
