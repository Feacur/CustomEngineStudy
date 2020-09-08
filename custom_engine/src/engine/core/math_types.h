#pragma once
#include "engine/core/types.h"

template<typename T>
struct xvec2 {
	union {
		struct { T x, y; };
		T data[2];
	};

#if defined(__cplusplus) // xvec2
	constexpr inline T & operator[](size_t i) { return data[i]; }
	constexpr inline T const & operator[](size_t i) const { return data[i]; }
#endif // defined(__cplusplus) // xvec2
};

template<typename T>
struct xvec3 {
	union {
		struct { xvec2<T> xy; T _1; };
		struct { T _2; xvec2<T> yz; };
		struct { T x, y, z; };
		T data[3];
	};

#if defined(__cplusplus) // xvec3
	constexpr inline T & operator[](size_t i) { return data[i]; }
	constexpr inline T const & operator[](size_t i) const { return data[i]; }
#endif // defined(__cplusplus) // xvec3
};

template<typename T>
struct xvec4 {
	union {
		struct { xvec3<T> xyz; T _1; };
		struct { T _2; xvec3<T> yzw; };
		struct { xvec2<T> xy; xvec2<T> zw; };
		struct { T _3; xvec2<T> yz; T _4; };
		struct { T x, y, z, w; };
		T data[4];
	};

#if defined(__cplusplus) // xvec4
	constexpr inline T & operator[](size_t i) { return data[i]; }
	constexpr inline T const & operator[](size_t i) const { return data[i]; }
#endif // defined(__cplusplus) // xvec4
};

// @Note
// vector structs have duplicate names in them
// though it seems to be OK, as their layout matches
// still, debugger draws them ugly

typedef xvec2<r32> vec2;
typedef xvec3<r32> vec3;
typedef xvec4<r32> vec4;

typedef xvec2<s32> ivec2;
typedef xvec3<s32> ivec3;
typedef xvec4<s32> ivec4;

typedef xvec2<u32> uvec2;
typedef xvec3<u32> uvec3;
typedef xvec4<u32> uvec4;

struct aabb2 { vec2 position, extents; };
struct aabb3 { vec3 position, extents; };

struct mat2 {
	union {
		struct { vec2 x, y; };
		vec2 axes[2];
		r32 data[2 * 2];
	};

#if defined(__cplusplus) // mat2
	constexpr inline vec2 & operator[](size_t i) { return axes[i]; }
	constexpr inline vec2 const & operator[](size_t i) const { return axes[i]; }
#endif // defined(__cplusplus) // mat2
};

struct mat3 {
	union {
		struct { vec3 x, y, z; };
		vec3 axes[3];
		r32 data[3 * 3];
	};

#if defined(__cplusplus) // mat3
	constexpr inline vec3 & operator[](size_t i) { return axes[i]; }
	constexpr inline vec3 const & operator[](size_t i) const { return axes[i]; }
#endif // defined(__cplusplus) // mat3
};

struct mat4 {
	union {
		struct { vec4 x, y, z, w; };
		vec4 axes[4];
		r32 data[4 * 4];
	};

#if defined(__cplusplus) // mat4
	constexpr inline vec4 & operator[](size_t i) { return axes[i]; }
	constexpr inline vec4 const & operator[](size_t i) const { return axes[i]; }
#endif // defined(__cplusplus) // mat4
};

typedef ivec2 icomplex;
typedef vec2  complex;
typedef vec4  quat;
