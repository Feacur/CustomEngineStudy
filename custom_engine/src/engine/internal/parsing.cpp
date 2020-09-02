#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/api/internal/parsing.h"
#include <math.h>

inline static u32 parse_u32_base(u32 value, cstring * source);
inline static s8 parse_sign(cstring * source);
inline static r32 construct_r32(s8 sign, u32 mantissa, s32 exponent_10);

namespace custom {

void skip_to_eol(cstring * source) {
	while (**source && !IS_EOL(**source)) { ++(*source); }
}

void skip_to_void(cstring * source) {
	while (**source && !IS_VOID(**source)) { ++(*source); }
}

void skip_to_digit(cstring * source) {
	while (**source && !IS_DIGIT(**source)) { ++(*source); }
}

void skip_to(cstring * source, char value) {
	while (**source && (**source != value)) { ++(*source); }
}

void parse_void(cstring * source) {
	while (IS_VOID(**source)) { ++(*source); }
}

void parse_eol(cstring * source) {
	while (IS_EOL(**source)) { ++(*source); }
}

u32 parse_u32(cstring * source) {
	return parse_u32_base(0u, source);
}

s32 parse_s32(cstring * source) {
	return (s32)parse_sign(source) * (s32)parse_u32_base(0u, source);
}

bln parse_bln(cstring * source) {
	if (strncmp(*source, "true", 4) == 0) { return true; }
	if (strncmp(*source, "false", 5) == 0) { return false; }
	return (bln)parse_u32_base(0u, source);
}

r32 parse_r32(cstring * source) {
	s8 sign = parse_sign(source);

	// @Note: no digits count limit
	u32 mantissa = parse_u32_base(0u, source);
	s32 exponent = 0;

	if (**source == '.') {
		cstring source_before = ++(*source);
		mantissa = parse_u32_base(mantissa, source);
		exponent -= (s32)(*source - source_before);
	}

	if (**source == 'e' || **source == 'E') {
		++(*source);
		exponent += parse_s32(source);
	}

	return construct_r32(sign, mantissa, exponent);
}

vec2 parse_vec2(cstring * source) {
	return {
		(parse_void(source), parse_r32(source)),
		(parse_void(source), parse_r32(source)),
	};
}

vec3 parse_vec3(cstring * source) {
	return {
		(parse_void(source), parse_r32(source)),
		(parse_void(source), parse_r32(source)),
		(parse_void(source), parse_r32(source)),
	};
}

vec4 parse_vec4(cstring * source) {
	return {
		(parse_void(source), parse_r32(source)),
		(parse_void(source), parse_r32(source)),
		(parse_void(source), parse_r32(source)),
		(parse_void(source), parse_r32(source)),
	};
}

}

inline static u32 parse_u32_base(u32 value, cstring * source) {
	// @Note: no overflow protection
	while (IS_DIGIT(**source)) {
		value = value * 10 + (**source - '0');
		++(*source);
	}
	return value;
}

inline static s8 parse_sign(cstring * source) {
	if (**source == '-') { ++(*source); return -1; }
	if (**source == '+') { ++(*source); return  1; }
	return 1;
}

inline static r32 construct_r32(s8 sign, u32 mantissa, s32 exponent_10) {
	// ldexp(a, b) == a * 2^b
	// return sign * (mantissa * powf(10, (r32)exponent));
	// return sign * ldexpf(mantissa * powf(5, (r32)exponent), exponent);

	if (!mantissa) { return 0.0f; }
	if (!exponent_10) { return (r32)sign * (r32)mantissa; }

	// > mantissa_x * 2^exponent_2 == mantissa * 10^exponent_10
	// > [start with] exponent_2 == exponent_10
	// > [calculate]  mantissa_x == mantissa * 5^exponent_10
	// > preserve significance by transferring powers of 2 from mantissa to exponent_2
	s32 exponent_2 = exponent_10;

	while (exponent_10 > 0) {
		constexpr int const mask = 0b11100000000000000000000000000000;
		while (mantissa & mask) { mantissa >>= 1u; ++exponent_2; }
		mantissa *= 5u; --exponent_10;
	}

	while (exponent_10 < 0) {
		constexpr int const mask = 0b10000000000000000000000000000000;
		while (!(mantissa & mask)) { mantissa <<= 1u; --exponent_2; }
		mantissa /= 5u; ++exponent_10;
	}

	return (r32)sign * ldexpf((r32)mantissa, exponent_2);
}
