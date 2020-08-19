#include "custom_pch.h"

#include "engine/api/internal/parsing.h"
#include <math.h>

inline static r32 construct_r32(s8 sign, u32 mantissa, s32 exponent_10);
inline static s8 parse_sign(cstring line, cstring * next_out);

namespace custom {

u32 parse_u32(u32 value, cstring line, cstring * next_out) {
	// @Note: no overflow protection
	while (IS_DIGIT(*line)) {
		value = value * 10 + (*line - '0');
		++line;
	}
	*next_out = line; return value;
}

s32 parse_s32(cstring line, cstring * next_out) {
	s8 sign = parse_sign(skip_blank(line), &line);
	u32 value = parse_u32(0u, line, next_out);
	return sign * (s32)value;
}

r32 parse_r32(cstring line, cstring * next_out) {
	s8 sign = parse_sign(skip_blank(line), &line);

	// @Note: no digits count limit
	u32 mantissa = parse_u32(0u, line, &line);
	s32 exponent = 0;

	if (*line == '.') {
		cstring line_before = ++line;
		mantissa = parse_u32(mantissa, line, &line);
		exponent -= (s32)(line - line_before);
	}

	if (*line == 'e' || *line == 'E') {
		exponent += parse_s32(line + 1, &line);
	}

	*next_out = line;
	return construct_r32(sign, mantissa, exponent);
}

}

inline static s8 parse_sign(cstring line, cstring * next_out) {
	if (*line == '-') { *next_out = line + 1; return -1; }
	if (*line == '+') { *next_out = line + 1; return  1; }
	*next_out = line; return 1;
}

inline static r32 construct_r32(s8 sign, u32 mantissa, s32 exponent_10) {
	// ldexp(a, b) == a * 2^b
	// return sign * (mantissa * powf(10, (r32)exponent));
	// return sign * ldexpf(mantissa * powf(5, (r32)exponent), exponent);

	if (!mantissa) { return 0.0f; }
	if (!exponent_10) { return sign * (r32)mantissa; }

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

	return sign * ldexpf((r32)mantissa, exponent_2);
}
