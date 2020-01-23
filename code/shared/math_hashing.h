#define SHARED_MATH_HASHING

// https://en.wikipedia.org/wiki/Xorshift
// https://en.wikipedia.org/wiki/Lehmer_random_number_generator
// https://en.wikipedia.org/wiki/Jenkins_hash_function
// http://www.iquilezles.org/www/articles/sfrand/sfrand.htm

constexpr inline u32 hash_xorshift32(u32 x) {
	x ^= x << 13U;
	x ^= x >> 17U;
	x ^= x << 15U;
	return x;
}

constexpr inline u64 hash_lehmer32(u64 x) {
	constexpr u64 const multiplier = 0x10a860c1ULL; // 0x0000bc8fULL
	constexpr u64 const modulo     = 0xfffffffbULL; // 0x7fffffffULL
	x = (x * multiplier) % modulo;
	return x;
}

constexpr u32 hash_jenkins(u32 x) {
	x += x << 10u;
	x ^= x >>  6u;
	x += x <<  3u;
	x ^= x >> 11u;
	x += x << 15u;
	return x;
}

constexpr inline u32 hash_iquilez(u32 value) {
	return value * 16807U;
}
