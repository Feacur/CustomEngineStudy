#define SHARED_MATH_SCALAR

static r32 const epsilon    = 0.00001f;
static r32 const pi         = 3.14159265359f;
static r32 const tau        = pi * 2; // 6.28318530718f
static r32 const deg_to_rad = pi / 180; // 0.01745329251f

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

MUL_DIV_IMPL(s8)
MUL_DIV_IMPL(s16)
MUL_DIV_IMPL(s32)
MUL_DIV_IMPL(s64)
MUL_DIV_IMPL(u8)
MUL_DIV_IMPL(u16)
MUL_DIV_IMPL(u32)
MUL_DIV_IMPL(u64)

MUL_DIV_IMPL(s48)
MUL_DIV_IMPL(u48)
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

SIGN_NON_ZERO_IMPL(s8)
SIGN_NON_ZERO_IMPL(s16)
SIGN_NON_ZERO_IMPL(s32)
SIGN_NON_ZERO_IMPL(s64)

SIGN_NON_ZERO_IMPL(s48)
#undef SIGN_NON_ZERO_IMPL

SIGN_IMPL(s8)
SIGN_IMPL(s16)
SIGN_IMPL(s32)
SIGN_IMPL(s64)

SIGN_IMPL(s48)
#undef SIGN_IMPL

MIN_IMPL(s8)
MIN_IMPL(s16)
MIN_IMPL(s32)
MIN_IMPL(s64)
MIN_IMPL(u8)
MIN_IMPL(u16)
MIN_IMPL(u32)
MIN_IMPL(u64)

MIN_IMPL(s48)
MIN_IMPL(u48)
#undef MIN_IMPL

MAX_IMPL(s8)
MAX_IMPL(s16)
MAX_IMPL(s32)
MAX_IMPL(s64)
MAX_IMPL(u8)
MAX_IMPL(u16)
MAX_IMPL(u32)
MAX_IMPL(u64)

MAX_IMPL(s48)
MAX_IMPL(u48)
#undef MAX_IMPL

//
// s32
//

constexpr inline s32 interpolate(s32 from, s32 to, s32 mul, s32 div) {
	return from + mul_div((to - from), mul, div);
}

constexpr inline s32 absolute(s32 value) {
	return value >= 0 ? value : -value;
}

constexpr inline s32 align_up(s32 value, s32 step) {
	return ((value + step - 1) / step) * step;
}

//
// r32
//

constexpr inline r32 sign_non_zero(r32 value) {
	return (value < 0.0f) ? -1.0f : 1.0f;
}

constexpr inline r32 sign(r32 value) {
	if (value < 0) { return -1.0f; }
	if (value > 0) { return 1.0f; }
	return 0.0f;
}

constexpr inline r32 min(r32 first, r32 second) {
	return (first < second) ? first : second;
}

constexpr inline r32 max(r32 first, r32 second) {
	return (first > second) ? first : second;
}

constexpr inline r32 interpolate(r32 from, r32 to, r32 fraction) {
	return from + (to - from) * fraction;
}

constexpr inline r32 absolute(r32 value) {
	return value >= 0 ? value : -value;
}

inline r32 square_root(r32 value) {
	return sqrtf(value);
}

inline r32 sine(r32 value) {
	return sinf(value);
}

inline r32 cosine(r32 value) {
	return cosf(value);
}

inline r32 tangent(r32 value) {
	return tanf(value);
}

//
// Mix
//

constexpr inline r32 uint8_to_fraction(u8 value) {
	return value / 255.f;
}

constexpr inline r32 uint16_to_fraction(u16 value) {
	return value / 65535.f;
}

constexpr inline r32 int8_to_fraction(s8 value) {
	return (value > 0) ? (value / 127.f) : (value / 128.f);
}

constexpr inline r32 int16_to_fraction(s16 value) {
	return (value > 0) ? (value / 32767.f) : (value / 32768.f);
}

constexpr inline r32 uint32_bits_to_float(u32 value) {
	union { u32 x; r32 xf; }; x = value;
	return xf;
}

constexpr inline r32 uint32_bits_to_01(u32 value) {
	// @Note: might well mask fractional part with [0x007fffffU] instead of shifting
	return uint32_bits_to_float((value >> 9U) | 0x3f800000U) - 1.0f; // return [1 .. 2) * (2^0) - 1
}

constexpr inline r32 uint32_bits_to_radius01(u32 value) {
	// @Note: might well mask fractional part with [0x007fffffU] instead of shifting
	return uint32_bits_to_float((value >> 9U) | 0x40000000U) - 3.0f; // [1 .. 2) * (2^1) - 3
}
