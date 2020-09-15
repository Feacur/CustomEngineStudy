#pragma once
#include "engine/core/code.h"
#include "engine/core/types.h"
#include "engine/core/math_types.h"
#include "engine/api/internal/parsing.h"

namespace custom {

template<u16 N>
inline static int strncmp_auto(cstring first, char const (& second)[N]) {
	return strncmp(first, second, N - 1);
}

inline static r32 to_r32(cstring * source) {
	return (skip_to_void(source), parse_void(source), parse_r32(source));
}

inline static u32 to_u32(cstring * source) {
	return (skip_to_void(source), parse_void(source), parse_u32(source));
}

inline static s32 to_s32(cstring * source) {
	return (skip_to_void(source), parse_void(source), parse_s32(source));
}

inline static cstring to_eol(cstring * source) {
	cstring line_end = (skip_to_void(source), parse_void(source), *source); skip_to_eol(&line_end);
	return line_end;
}

inline static vec2 to_vec2(cstring * source) {
	return (skip_to_void(source), parse_void(source), parse_vec2(source));
}

inline static vec3 to_vec3(cstring * source) {
	return (skip_to_void(source), parse_void(source), parse_vec3(source));
}

inline static vec4 to_vec4(cstring * source) {
	return (skip_to_void(source), parse_void(source), parse_vec4(source));
}

}
