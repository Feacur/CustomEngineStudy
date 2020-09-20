#pragma once
#include "engine/core/code.h"
#include "engine/core/types.h"
#include "engine/core/math_types.h"
#include "engine/debug/log.h"
#include "engine/api/internal/parsing.h"

namespace custom {

template<u16 N>
inline static int strncmp_auto(cstring first, char const (& second)[N]) {
	return strncmp(first, second, N - 1);
}

inline static u32 to_identifier_length(cstring * source) {
	parse_void(source);

	if (!IS_VALID_IDENTIFIER_START(**source)) {
		CUSTOM_ASSERT(false, "invalid identifier start");
		skip_to_eol(source); return 0;
	}

	cstring string_end = *source; parse_identifier(&string_end);

	return (u32)(string_end - *source);
}

inline static u32 to_string_length(cstring * source) {
	parse_void(source);

	if (**source != '"') {
		CUSTOM_ASSERT(false, "invalid string start");
		skip_to_eol(source); return 0;
	}

	++(*source);
	cstring string_end = *source; skip_to_single_line_string_end(&string_end);

	return (u32)(string_end - *source);
}

inline static r32 to_r32(cstring * source) {
	return (parse_void(source), parse_r32(source));
}

inline static u32 to_u32(cstring * source) {
	return (parse_void(source), parse_u32(source));
}

inline static s32 to_s32(cstring * source) {
	return (parse_void(source), parse_s32(source));
}

inline static bln to_bln(cstring * source) {
	return (parse_void(source), parse_bln(source));
}

inline static vec2 to_vec2(cstring * source) {
	return (parse_void(source), parse_vec2(source));
}

inline static vec3 to_vec3(cstring * source) {
	return (parse_void(source), parse_vec3(source));
}

inline static vec4 to_vec4(cstring * source) {
	return (parse_void(source), parse_vec4(source));
}

inline static void to_next_line(cstring * source) {
	skip_to_eol(source); parse_eol(source);
}

}
