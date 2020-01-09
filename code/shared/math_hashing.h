#define SHARED_MATH_HASHING

#if !defined(SHARED_CODE)
	#error include "shared/code.h"
#endif

// https://en.wikipedia.org/wiki/Xorshift
// https://en.wikipedia.org/wiki/Lehmer_random_number_generator
// https://en.wikipedia.org/wiki/Jenkins_hash_function
// http://www.iquilezles.org/www/articles/sfrand/sfrand.htm

constexpr inline uint32 hash_xorshift32(uint32 x) {
	x ^= x << 13U;
	x ^= x >> 17U;
	x ^= x << 15U;
	return x;
}

constexpr inline uint64 hash_lehmer32(uint64 x) {
	constexpr uint64 const multiplier = 0x10a860c1UL; // 0x0000bc8fUL
	constexpr uint64 const modulo     = 0xfffffffbUL; // 0x7fffffffUL
	x = (x * multiplier) % modulo;
	return x;
}

constexpr uint32 hash_jenkins(uint32 x) {
	x += x << 10u;
	x ^= x >>  6u;
	x += x <<  3u;
	x ^= x >> 11u;
	x += x << 15u;
	return x;
}

constexpr inline uint32 hash_iquilez(uint32 value) {
	return value * 16807U;
}
