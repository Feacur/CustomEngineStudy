#pragma once
#include "engine/core/types.h"

struct vec2 {
	union {
		struct { r32 x, y; };
		r32 data[2];
	};

#if defined(__cplusplus) // vec2
	constexpr inline r32 operator[](size_t i) { return data[i]; }
	constexpr inline r32 const & operator[](size_t i) const { return data[i]; }
#endif // defined(__cplusplus) // vec2
};

struct vec3 {
	union {
		struct { vec2 xy; r32 z; };
		struct { r32 x; vec2 yz; };
		struct { r32 x, y, z; };
		r32 data[3];
	};

#if defined(__cplusplus) // vec3
	constexpr inline r32 operator[](size_t i) { return data[i]; }
	constexpr inline r32 const & operator[](size_t i) const { return data[i]; }
#endif // defined(__cplusplus) // vec3
};

struct vec4 {
	union {
		struct { vec3 xyz; r32 w; };
		struct { r32 x; vec3 yzw; };
		struct { vec2 xy; vec2 zw; };
		struct { r32 x; vec2 yz; r32 w; };
		struct { r32 x, y, z, w; };
		r32 data[4];
	};

#if defined(__cplusplus) // vec4
	constexpr inline r32 operator[](size_t i) { return data[i]; }
	constexpr inline r32 const & operator[](size_t i) const { return data[i]; }
#endif // defined(__cplusplus) // vec4
};

struct ivec2 {
	union {
		struct { s32 x, y; };
		s32 data[2];
	};
	
#if defined(__cplusplus) // ivec2
	constexpr inline s32 & operator[](size_t i) { return data[i]; }
	constexpr inline s32 const & operator[](size_t i) const { return data[i]; }
#endif // defined(__cplusplus) // ivec2
};

struct ivec3 {
	union {
		struct { ivec2 xy; s32 z; };
		struct { s32 x; ivec2 yz; };
		struct { s32 x, y, z; };
		s32 data[3];
	};
	
#if defined(__cplusplus) // ivec3
	constexpr inline s32 & operator[](size_t i) { return data[i]; }
	constexpr inline s32 const & operator[](size_t i) const { return data[i]; }
#endif // defined(__cplusplus) // ivec3
};

struct ivec4 {
	union {
		struct { ivec3 xyz; s32 w; };
		struct { s32 x; ivec3 yzw; };
		struct { vec2 xy; ivec2 zw; };
		struct { s32 x; ivec2 yz; s32 w; };
		struct { s32 x, y, z, w; };
		s32 data[4];
	};
	
#if defined(__cplusplus) // ivec4
	constexpr inline s32 & operator[](size_t i) { return data[i]; }
	constexpr inline s32 const & operator[](size_t i) const { return data[i]; }
#endif // defined(__cplusplus) // ivec4
};

struct uvec2 { u32 data[2]; };
struct uvec3 { u32 data[3]; };
struct uvec4 { u32 data[4]; };

struct mat2 { vec2 data[2]; };
struct mat3 { vec3 data[3]; };
struct mat4 { vec4 data[4]; };

// @Note
// vector structs have duplicate names in them
// though it seems to be OK, as their layout matches
// still, debugger draws them ugly

typedef ivec2 icomplex;
typedef vec2  complex;
typedef vec4  quat;
