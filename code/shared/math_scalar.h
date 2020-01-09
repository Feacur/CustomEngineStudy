#define SHARED_MATH_SCALAR

#if !defined(SHARED_CODE)
	#error include "shared/code.h"
#endif

static float const epsilon    = 0.00001f;
static float const pi         = 3.14159265359f;
static float const tau        = pi * 2; // 6.28318530718f
static float const deg_to_rad = pi / 180; // 0.01745329251f

// with 64 bit integers compiler has no chance promoting to a larger storage type
// thus naive "return (value * numerator) / denominator" wont't work properly
// neither "return (value / denominator) * numerator" would, obviously
// substitute "value" == "(value / denominator) * denominator + (value % denominator)", simplify

// thanks: https://github.com/rust-lang/rust/blob/master/src/libstd/sys_common/mod.rs

#define MUL_DIV_IMPL(T)\
constexpr inline T mul_div(T value, T numerator, T denominator) {\
	T a = value / denominator;\
	T b = value % denominator;\
	return a * numerator + b * numerator / denominator;\
}

MUL_DIV_IMPL(int8)
MUL_DIV_IMPL(int16)
MUL_DIV_IMPL(int32)
MUL_DIV_IMPL(int64)
MUL_DIV_IMPL(uint8)
MUL_DIV_IMPL(uint16)
MUL_DIV_IMPL(uint32)
MUL_DIV_IMPL(uint64)

MUL_DIV_IMPL(int48)
MUL_DIV_IMPL(uint48)
#undef MUL_DIV_IMPL

//
//
//


#define SIGN_NON_ZERO_IMPL(T)\
constexpr inline T sign_non_zero(T value) {\
	return (value < 0) ? -1 : 1;\
}

#define SIGN_IMPL(T)\
constexpr inline T sign(T value) {\
	if (value < 0) { return -1; }\
	if (value > 0) { return 1; }\
	return 0;\
}

#define MIN_IMPL(T)\
constexpr inline T min(T first, T second) {\
	return (first < second) ? first : second;\
}

#define MAX_IMPL(T)\
constexpr inline T max(T first, T second) {\
	return (first > second) ? first : second;\
}

SIGN_NON_ZERO_IMPL(int8)
SIGN_NON_ZERO_IMPL(int16)
SIGN_NON_ZERO_IMPL(int32)
SIGN_NON_ZERO_IMPL(int64)

SIGN_NON_ZERO_IMPL(int48)
#undef SIGN_NON_ZERO_IMPL

SIGN_IMPL(int8)
SIGN_IMPL(int16)
SIGN_IMPL(int32)
SIGN_IMPL(int64)

SIGN_IMPL(int48)
#undef SIGN_IMPL

MIN_IMPL(int8)
MIN_IMPL(int16)
MIN_IMPL(int32)
MIN_IMPL(int64)
MIN_IMPL(uint8)
MIN_IMPL(uint16)
MIN_IMPL(uint32)
MIN_IMPL(uint64)

MIN_IMPL(int48)
MIN_IMPL(uint48)
#undef MIN_IMPL

MAX_IMPL(int8)
MAX_IMPL(int16)
MAX_IMPL(int32)
MAX_IMPL(int64)
MAX_IMPL(uint8)
MAX_IMPL(uint16)
MAX_IMPL(uint32)
MAX_IMPL(uint64)

MAX_IMPL(int48)
MAX_IMPL(uint48)
#undef MAX_IMPL

//
// int32
//

constexpr inline int32 interpolate(int32 from, int32 to, int32 mul, int32 div) {
	return from + mul_div((to - from), mul, div);
}

constexpr inline int32 absolute(int32 value) {
	return value >= 0 ? value : -value;
}

constexpr inline int32 round_up_with_step(int32 value, int32 step) {
	int32 reminder = (value % step);
	return (reminder < 1) ? value : (value + (step - reminder));
}

//
// float
//

constexpr inline float sign_non_zero(float value) {
	return (value < 0.0f) ? -1.0f : 1.0f;
}

constexpr inline float sign(float value) {
	if (value < 0) { return -1.0f; }
	if (value > 0) { return 1.0f; }
	return 0.0f;
}

constexpr inline float min(float first, float second) {
	return (first < second) ? first : second;
}

constexpr inline float max(float first, float second) {
	return (first > second) ? first : second;
}

constexpr inline float interpolate(float from, float to, float fraction) {
	return from + (to - from) * fraction;
}

constexpr inline float absolute(float value) {
	return value >= 0 ? value : -value;
}

inline float square_root(float value) {
	return sqrtf(value);
}

inline float sine(float value) {
	return sinf(value);
}

inline float cosine(float value) {
	return cosf(value);
}

inline float tangent(float value) {
	return tanf(value);
}

//
// Mix
//

constexpr inline float uint8_to_fraction(uint8 value) {
	return value / 255.f;
}

constexpr inline float uint16_to_fraction(uint16 value) {
	return value / 65535.f;
}

constexpr inline float int8_to_fraction(int8 value) {
	return (value > 0) ? (value / 127.f) : (value / 128.f);
}

constexpr inline float int16_to_fraction(int16 value) {
	return (value > 0) ? (value / 32767.f) : (value / 32768.f);
}

constexpr inline float uint32_bits_to_float(uint32 value) {
	union { uint32 x; float xf; }; x = value;
	return xf;
}

constexpr inline float uint32_bits_to_01(uint32 value) {
	// @Note: might well mask fractional part with [0x007fffffU] instead of shifting
	return uint32_bits_to_float((value >> 9U) | 0x3f800000U) - 1.0f; // return [1 .. 2) * (2^0) - 1
}

constexpr inline float uint32_bits_to_radius01(uint32 value) {
	// @Note: might well mask fractional part with [0x007fffffU] instead of shifting
	return uint32_bits_to_float((value >> 9U) | 0x40000000U) - 3.0f; // [1 .. 2) * (2^1) - 3
}
