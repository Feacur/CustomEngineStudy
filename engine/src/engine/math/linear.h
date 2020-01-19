#pragma once
#include "scalar.h"

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

// @Note
// vector structs have duplicate names in them
// though it seems to be OK, as their layout matches
// still, debugger draws them ugly

typedef ivec2 icomplex;
typedef vec2  complex;
typedef vec4  quat;

constexpr inline ivec2 civec2(s32 x, s32 y) { return {x, y}; }
constexpr inline ivec3 civec3(s32 x, s32 y, s32 z) { return {x, y, z}; }

constexpr inline vec2 cvec2(r32 x, r32 y) { return {x, y}; }
constexpr inline vec3 cvec3(r32 x, r32 y, r32 z) { return {x, y, z}; }
constexpr inline vec4 cvec4(r32 x, r32 y, r32 z, r32 w) { return {x, y, z, w}; }

//
//
//

constexpr inline s32 cross_product(ivec2 first, ivec2 second) {
	return first.x * second.y - first.y * second.x;
}

constexpr inline ivec3 cross_product(ivec3 first, ivec3 second) {
	return {
		first.y * second.z - first.z * second.y,
		first.z * second.x - first.x * second.z,
		first.x * second.y - first.y * second.x
	};
}

constexpr inline r32 cross_product(vec2 first, vec2 second) {
	return first.x * second.y - first.y * second.x;
}

constexpr inline vec3 cross_product(vec3 first, vec3 second) {
	return {
		first.y * second.z - first.z * second.y,
		first.z * second.x - first.x * second.z,
		first.x * second.y - first.y * second.x
	};
}

//
// ivec2 routines
// Applications: 2d position
//

#if defined(__cplusplus) // operator ivec2
constexpr inline bool operator==(ivec2 first, ivec2 second) {
	return (first.x == second.x)
	    && (first.y == second.y);
}

constexpr inline bool operator!=(ivec2 first, ivec2 second) {
	return (first.x != second.x)
	    || (first.y != second.y);
}

constexpr inline ivec2 operator+(ivec2 first, ivec2 second) {
	return {
		first.x + second.x,
		first.y + second.y
	};
}

constexpr inline ivec2 operator+(ivec2 first, s32 second) {
	return {
		first.x + second,
		first.y + second
	};
}

constexpr inline ivec2 operator-(ivec2 first, ivec2 second) {
	return {
		first.x - second.x,
		first.y - second.y
	};
}

constexpr inline ivec2 operator-(ivec2 first, s32 second) {
	return {
		first.x - second,
		first.y - second
	};
}

constexpr inline ivec2 operator-(ivec2 value) {
	return {
		-value.x,
		-value.y
	};
}

constexpr inline ivec2 operator*(ivec2 first, s32 second) {
	return {
		first.x * second,
		first.y * second
	};
}

constexpr inline ivec2 operator*(ivec2 first, ivec2 second) {
	return {
		first.x * second.x,
		first.y * second.y
	};
}

constexpr inline ivec2 operator/(ivec2 first, s32 second) {
	return {
		first.x / second,
		first.y / second
	};
}

constexpr inline ivec2 operator/(ivec2 first, ivec2 second) {
	return {
		first.x / second.x,
		first.y / second.y
	};
}

constexpr inline ivec2 operator%(ivec2 first, s32 second) {
	return {
		first.x % second,
		first.y % second
	};
}

constexpr inline ivec2 operator%(ivec2 first, ivec2 second) {
	return {
		first.x % second.x,
		first.y % second.y
	};
}

constexpr inline ivec2 & operator+=(ivec2 & first, ivec2 const & second) {
	first.x += second.x;
	first.y += second.y;
	return first;
}

constexpr inline ivec2 & operator+=(ivec2 & first, s32 second) {
	first.x += second;
	first.y += second;
	return first;
}

constexpr inline ivec2 & operator-=(ivec2 & first, ivec2 const & second) {
	first.x -= second.x;
	first.y -= second.y;
	return first;
}

constexpr inline ivec2 & operator-=(ivec2 & first, s32 second) {
	first.x -= second;
	first.y -= second;
	return first;
}

constexpr inline ivec2 & operator*=(ivec2 & first, ivec2 const & second) {
	first.x *= second.x;
	first.y *= second.y;
	return first;
}

constexpr inline ivec2 & operator*=(ivec2 & first, s32 second) {
	first.x *= second;
	first.y *= second;
	return first;
}

constexpr inline ivec2 & operator/=(ivec2 & first, ivec2 const & second) {
	first.x /= second.x;
	first.y /= second.y;
	return first;
}

constexpr inline ivec2 & operator/=(ivec2 & first, s32 second) {
	first.x /= second;
	first.y /= second;
	return first;
}

constexpr inline ivec2 & operator%=(ivec2 & first, ivec2 const & second) {
	first.x %= second.x;
	first.y %= second.y;
	return first;
}

constexpr inline ivec2 & operator%=(ivec2 & first, s32 second) {
	first.x %= second;
	first.y %= second;
	return first;
}
#endif // defined(__cplusplus) // operator ivec2

constexpr inline s32 dot_product(ivec2 first, ivec2 second) {
	return first.x * second.x
	     + first.y * second.y;
}

constexpr inline ivec2 min(ivec2 first, ivec2 second) {
	return {
		min(first.x, second.x),
		min(first.y, second.y)
	};
}

constexpr inline ivec2 max(ivec2 first, ivec2 second) {
	return {
		max(first.x, second.x),
		max(first.y, second.y)
	};
}

constexpr inline s32 min(ivec2 value) {
	return min(value.x, value.y);
}

constexpr inline s32 max(ivec2 value) {
	return max(value.x, value.y);
}

constexpr inline ivec2 absolute(ivec2 value) {
	return {
		absolute(value.x),
		absolute(value.y)
	};
}

constexpr inline ivec2 sign(ivec2 value) {
	return {
		sign(value.x),
		sign(value.y)
	};
}

constexpr inline ivec2 interpolate(ivec2 from, ivec2 to, s32 mul, s32 div) {
	return {
		interpolate(from.x, to.x, mul, div),
		interpolate(from.y, to.y, mul, div)
	};
}

//
// ivec3 routines
// Applications: 3d position
//

#if defined(__cplusplus) // operator ivec3
constexpr inline bool operator==(ivec3 first, ivec3 second) {
	return (first.x == second.x)
	    && (first.y == second.y)
	    && (first.z == second.z);
}

constexpr inline bool operator!=(ivec3 first, ivec3 second) {
	return (first.x != second.x)
	    || (first.y != second.y)
	    || (first.z != second.z);
}

constexpr inline ivec3 operator+(ivec3 first, ivec3 second) {
	return {
		first.x + second.x,
		first.y + second.y,
		first.z + second.z
	};
}

constexpr inline ivec3 operator+(ivec3 first, s32 second) {
	return {
		first.x + second,
		first.y + second,
		first.z + second
	};
}

constexpr inline ivec3 operator-(ivec3 first, ivec3 second) {
	return {
		first.x - second.x,
		first.y - second.y,
		first.z - second.z
	};
}

constexpr inline ivec3 operator-(ivec3 first, s32 second) {
	return {
		first.x - second,
		first.y - second,
		first.z - second
	};
}

constexpr inline ivec3 operator-(ivec3 value) {
	return {
		-value.x,
		-value.y,
		-value.z
	};
}

constexpr inline ivec3 operator*(ivec3 first, s32 second) {
	return {
		first.x * second,
		first.y * second,
		first.z * second
	};
}

constexpr inline ivec3 operator*(ivec3 first, ivec3 second) {
	return {
		first.x * second.x,
		first.y * second.y,
		first.z * second.z
	};
}

constexpr inline ivec3 operator/(ivec3 first, s32 second) {
	return {
		first.x / second,
		first.y / second,
		first.z / second
	};
}

constexpr inline ivec3 operator/(ivec3 first, ivec3 second) {
	return {
		first.x / second.x,
		first.y / second.y,
		first.z / second.z
	};
}

constexpr inline ivec3 operator%(ivec3 first, s32 second) {
	return {
		first.x % second,
		first.y % second,
		first.z % second
	};
}

constexpr inline ivec3 operator%(ivec3 first, ivec3 second) {
	return {
		first.x % second.x,
		first.y % second.y,
		first.z % second.z
	};
}

constexpr inline ivec3 & operator+=(ivec3 & first, ivec3 const & second) {
	first.x += second.x;
	first.y += second.y;
	first.z += second.z;
	return first;
}

constexpr inline ivec3 & operator+=(ivec3 & first, s32 second) {
	first.x += second;
	first.y += second;
	first.z += second;
	return first;
}

constexpr inline ivec3 & operator-=(ivec3 & first, ivec3 const & second) {
	first.x -= second.x;
	first.y -= second.y;
	first.z -= second.z;
	return first;
}

constexpr inline ivec3 & operator-=(ivec3 & first, s32 second) {
	first.x -= second;
	first.y -= second;
	first.z -= second;
	return first;
}

constexpr inline ivec3 & operator*=(ivec3 & first, ivec3 const & second) {
	first.x *= second.x;
	first.y *= second.y;
	first.z *= second.z;
	return first;
}

constexpr inline ivec3 & operator*=(ivec3 & first, s32 second) {
	first.x *= second;
	first.y *= second;
	first.z *= second;
	return first;
}

constexpr inline ivec3 & operator/=(ivec3 & first, ivec3 const & second) {
	first.x /= second.x;
	first.y /= second.y;
	first.z /= second.z;
	return first;
}

constexpr inline ivec3 & operator/=(ivec3 & first, s32 second) {
	first.x /= second;
	first.y /= second;
	first.z /= second;
	return first;
}

constexpr inline ivec3 & operator%=(ivec3 & first, ivec3 const & second) {
	first.x %= second.x;
	first.y %= second.y;
	first.z %= second.z;
	return first;
}

constexpr inline ivec3 & operator%=(ivec3 & first, s32 second) {
	first.x %= second;
	first.y %= second;
	first.z %= second;
	return first;
}
#endif // defined(__cplusplus) // operator ivec3

constexpr inline s32 dot_product(ivec3 first, ivec3 second) {
	return first.x * second.x
	     + first.y * second.y
	     + first.z * second.z;
}

constexpr inline ivec3 min(ivec3 first, ivec3 second) {
	return {
		min(first.x, second.x),
		min(first.y, second.y),
		min(first.z, second.z)
	};
}

constexpr inline ivec3 max(ivec3 first, ivec3 second) {
	return {
		max(first.x, second.x),
		max(first.y, second.y),
		max(first.z, second.z)
	};
}

constexpr inline s32 min(ivec3 value) {
	return min(min(value.x, value.y), value.z);
}

constexpr inline s32 max(ivec3 value) {
	return max(max(value.x, value.y), value.z);
}

constexpr inline ivec3 absolute(ivec3 value) {
	return {
		absolute(value.x),
		absolute(value.y),
		absolute(value.z)
	};
}

constexpr inline ivec3 sign(ivec3 value) {
	return {
		sign(value.x),
		sign(value.y),
		sign(value.z)
	};
}

constexpr inline ivec3 interpolate(ivec3 from, ivec3 to, s32 mul, s32 div) {
	return {
		interpolate(from.x, to.x, mul, div),
		interpolate(from.y, to.y, mul, div),
		interpolate(from.z, to.z, mul, div)
	};
}

//
// vec2 routines
// Applications: 2d position, 2d rotation as complex number, 2d scale, 2d direction
//

#if defined(__cplusplus) // operator vec2
constexpr inline bool operator==(vec2 first, vec2 second) {
	return (first.x == second.x)
	    && (first.y == second.y);
}

constexpr inline bool operator!=(vec2 first, vec2 second) {
	return (first.x != second.x)
	    || (first.y != second.y);
}

constexpr inline vec2 operator+(vec2 first, vec2 second) {
	return {
		first.x + second.x,
		first.y + second.y
	};
}

constexpr inline vec2 operator+(vec2 first, r32 second) {
	return {
		first.x + second,
		first.y + second
	};
}

constexpr inline vec2 operator-(vec2 first, vec2 second) {
	return {
		first.x - second.x,
		first.y - second.y
	};
}

constexpr inline vec2 operator-(vec2 first, r32 second) {
	return {
		first.x - second,
		first.y - second
	};
}

constexpr inline vec2 operator-(vec2 value) {
	return {
		-value.x,
		-value.y
	};
}

constexpr inline vec2 operator*(vec2 first, r32 second) {
	return {
		first.x * second,
		first.y * second
	};
}

constexpr inline vec2 operator*(vec2 first, vec2 second) {
	return {
		first.x * second.x,
		first.y * second.y
	};
}

constexpr inline vec2 operator/(vec2 first, r32 second) {
	return {
		first.x / second,
		first.y / second
	};
}

constexpr inline vec2 operator/(vec2 first, vec2 second) {
	return {
		first.x / second.x,
		first.y / second.y
	};
}

constexpr inline vec2 & operator+=(vec2 & first, vec2 const & second) {
	first.x += second.x;
	first.y += second.y;
	return first;
}

constexpr inline vec2 & operator+=(vec2 & first, r32 second) {
	first.x += second;
	first.y += second;
	return first;
}

constexpr inline vec2 & operator-=(vec2 & first, vec2 const & second) {
	first.x -= second.x;
	first.y -= second.y;
	return first;
}

constexpr inline vec2 & operator-=(vec2 & first, r32 second) {
	first.x -= second;
	first.y -= second;
	return first;
}

constexpr inline vec2 & operator*=(vec2 & first, vec2 const & second) {
	first.x *= second.x;
	first.y *= second.y;
	return first;
}

constexpr inline vec2 & operator*=(vec2 & first, r32 second) {
	first.x *= second;
	first.y *= second;
	return first;
}

constexpr inline vec2 & operator/=(vec2 & first, vec2 const & second) {
	first.x /= second.x;
	first.y /= second.y;
	return first;
}

constexpr inline vec2 & operator/=(vec2 & first, r32 second) {
	first.x /= second;
	first.y /= second;
	return first;
}
#endif // defined(__cplusplus) // operator vec2

constexpr inline r32 dot_product(vec2 first, vec2 second) {
	return first.x * second.x
	     + first.y * second.y;
}

constexpr inline vec2 min(vec2 first, vec2 second) {
	return {
		min(first.x, second.x),
		min(first.y, second.y)
	};
}

constexpr inline vec2 max(vec2 first, vec2 second) {
	return {
		max(first.x, second.x),
		max(first.y, second.y)
	};
}

constexpr inline r32 min(vec2 value) {
	return min(value.x, value.y);
}

constexpr inline r32 max(vec2 value) {
	return max(value.x, value.y);
}

constexpr inline vec2 absolute(vec2 value) {
	return {
		absolute(value.x),
		absolute(value.y)
	};
}

constexpr inline vec2 sign(vec2 value) {
	return {
		sign(value.x),
		sign(value.y)
	};
}

constexpr inline vec2 interpolate(vec2 from, vec2 to, r32 fraction) {
	return {
		interpolate(from.x, to.x, fraction),
		interpolate(from.y, to.y, fraction)
	};
}

inline vec2 square_root(vec2 value) {
	return {
		square_root(value.x),
		square_root(value.y)
	};
}

inline vec2 sine(vec2 value) {
	return {
		sine(value.x),
		sine(value.y)
	};
}

inline vec2 cosine(vec2 value) {
	return {
		cosine(value.x),
		cosine(value.y)
	};
}

//
// vec3 routines
// Applications: 3d position, 3d rotation as euler angles, 3d scale, 3d direction
//

#if defined(__cplusplus) // operator vec3
constexpr inline bool operator==(vec3 first, vec3 second) {
	return (first.x == second.x)
	    && (first.y == second.y)
	    && (first.z == second.z);
}

constexpr inline bool operator!=(vec3 first, vec3 second) {
	return (first.x != second.x)
	    || (first.y != second.y)
	    || (first.z != second.z);
}

constexpr inline vec3 operator+(vec3 first, vec3 second) {
	return {
		first.x + second.x,
		first.y + second.y,
		first.z + second.z
	};
}

constexpr inline vec3 operator+(vec3 first, r32 second) {
	return {
		first.x + second,
		first.y + second,
		first.z + second
	};
}

constexpr inline vec3 operator-(vec3 first, vec3 second) {
	return {
		first.x - second.x,
		first.y - second.y,
		first.z - second.z
	};
}

constexpr inline vec3 operator-(vec3 first, r32 second) {
	return {
		first.x - second,
		first.y - second,
		first.z - second
	};
}

constexpr inline vec3 operator-(vec3 value) {
	return {
		-value.x,
		-value.y,
		-value.z
	};
}

constexpr inline vec3 operator*(vec3 first, r32 second) {
	return {
		first.x * second,
		first.y * second,
		first.z * second
	};
}

constexpr inline vec3 operator*(vec3 first, vec3 second) {
	return {
		first.x * second.x,
		first.y * second.y,
		first.z * second.z
	};
}

constexpr inline vec3 operator/(vec3 first, r32 second) {
	return {
		first.x / second,
		first.y / second,
		first.z / second
	};
}

constexpr inline vec3 operator/(vec3 first, vec3 second) {
	return {
		first.x / second.x,
		first.y / second.y,
		first.z / second.z
	};
}

constexpr inline vec3 & operator+=(vec3 & first, vec3 const & second) {
	first.x += second.x;
	first.y += second.y;
	first.z += second.z;
	return first;
}

constexpr inline vec3 & operator+=(vec3 & first, r32 second) {
	first.x += second;
	first.y += second;
	first.z += second;
	return first;
}

constexpr inline vec3 & operator-=(vec3 & first, vec3 const & second) {
	first.x -= second.x;
	first.y -= second.y;
	first.z -= second.z;
	return first;
}

constexpr inline vec3 & operator-=(vec3 & first, r32 second) {
	first.x -= second;
	first.y -= second;
	first.z -= second;
	return first;
}

constexpr inline vec3 & operator*=(vec3 & first, vec3 const & second) {
	first.x *= second.x;
	first.y *= second.y;
	first.z *= second.z;
	return first;
}

constexpr inline vec3 & operator*=(vec3 & first, r32 second) {
	first.x *= second;
	first.y *= second;
	first.z *= second;
	return first;
}

constexpr inline vec3 & operator/=(vec3 & first, vec3 const & second) {
	first.x /= second.x;
	first.y /= second.y;
	first.z /= second.z;
	return first;
}

constexpr inline vec3 & operator/=(vec3 & first, r32 second) {
	first.x /= second;
	first.y /= second;
	first.z /= second;
	return first;
}
#endif // defined(__cplusplus) // operator vec3

constexpr inline r32 dot_product(vec3 first, vec3 second) {
	return first.x * second.x
	     + first.y * second.y
	     + first.z * second.z;
}

constexpr inline vec3 min(vec3 first, vec3 second) {
	return {
		min(first.x, second.x),
		min(first.y, second.y),
		min(first.z, second.z)
	};
}

constexpr inline vec3 max(vec3 first, vec3 second) {
	return {
		max(first.x, second.x),
		max(first.y, second.y),
		max(first.z, second.z)
	};
}

constexpr inline r32 min(vec3 value) {
	return min(min(value.x, value.y), value.z);
}

constexpr inline r32 max(vec3 value) {
	return max(max(value.x, value.y), value.z);
}

constexpr inline vec3 absolute(vec3 value) {
	return {
		absolute(value.x),
		absolute(value.y),
		absolute(value.z)
	};
}

constexpr inline vec3 sign(vec3 value) {
	return {
		sign(value.x),
		sign(value.y),
		sign(value.z)
	};
}

constexpr inline vec3 interpolate(vec3 from, vec3 to, r32 fraction) {
	return {
		interpolate(from.x, to.x, fraction),
		interpolate(from.y, to.y, fraction),
		interpolate(from.z, to.z, fraction)
	};
}

inline vec3 square_root(vec3 value) {
	return {
		square_root(value.x),
		square_root(value.y),
		square_root(value.z)
	};
}

inline vec3 sine(vec3 value) {
	return {
		sine(value.x),
		sine(value.y),
		sine(value.z)
	};
}

inline vec3 cosine(vec3 value) {
	return {
		cosine(value.x),
		cosine(value.y),
		cosine(value.z)
	};
}

//
// vec4 routines
// Applications: 3d rotation as quat, color
//

#if defined(__cplusplus) // operator vec4
constexpr inline bool operator==(vec4 first, vec4 second) {
	return (first.x == second.x)
	    && (first.y == second.y)
	    && (first.z == second.z)
	    && (first.w == second.w);
}

constexpr inline bool operator!=(vec4 first, vec4 second) {
	return (first.x != second.x)
	    || (first.y != second.y)
	    || (first.z != second.z)
	    || (first.w != second.w);
}

constexpr inline vec4 operator+(vec4 first, vec4 second) {
	return {
		first.x + second.x,
		first.y + second.y,
		first.z + second.z,
		first.w + second.w
	};
}

constexpr inline vec4 operator+(vec4 first, r32 second) {
	return {
		first.x + second,
		first.y + second,
		first.z + second,
		first.w + second
	};
}

constexpr inline vec4 operator-(vec4 first, vec4 second) {
	return {
		first.x - second.x,
		first.y - second.y,
		first.z - second.z,
		first.w - second.w
	};
}

constexpr inline vec4 operator-(vec4 first, r32 second) {
	return {
		first.x - second,
		first.y - second,
		first.z - second,
		first.w - second
	};
}

constexpr inline vec4 operator-(vec4 value) {
	return {
		-value.x,
		-value.y,
		-value.z,
		-value.w
	};
}

constexpr inline vec4 operator*(vec4 first, r32 second) {
	return {
		first.x * second,
		first.y * second,
		first.z * second,
		first.w * second
	};
}

constexpr inline vec4 operator*(vec4 first, vec4 second) {
	return {
		first.x * second.x,
		first.y * second.y,
		first.z * second.z,
		first.w * second.w
	};
}

constexpr inline vec4 operator/(vec4 first, r32 second) {
	return {
		first.x / second,
		first.y / second,
		first.z / second,
		first.w / second
	};
}

constexpr inline vec4 operator/(vec4 first, vec4 second) {
	return {
		first.x / second.x,
		first.y / second.y,
		first.z / second.z,
		first.w / second.w
	};
}

constexpr inline vec4 & operator+=(vec4 & first, vec4 const & second) {
	first.x += second.x;
	first.y += second.y;
	first.z += second.z;
	first.w += second.w;
	return first;
}

constexpr inline vec4 & operator+=(vec4 & first, r32 second) {
	first.x += second;
	first.y += second;
	first.z += second;
	first.w += second;
	return first;
}

constexpr inline vec4 & operator-=(vec4 & first, vec4 const & second) {
	first.x -= second.x;
	first.y -= second.y;
	first.z -= second.z;
	first.w -= second.w;
	return first;
}

constexpr inline vec4 & operator-=(vec4 & first, r32 second) {
	first.x -= second;
	first.y -= second;
	first.z -= second;
	first.w -= second;
	return first;
}

constexpr inline vec4 & operator*=(vec4 & first, vec4 const & second) {
	first.x *= second.x;
	first.y *= second.y;
	first.z *= second.z;
	first.w *= second.w;
	return first;
}

constexpr inline vec4 & operator*=(vec4 & first, r32 second) {
	first.x *= second;
	first.y *= second;
	first.z *= second;
	first.w *= second;
	return first;
}

constexpr inline vec4 & operator/=(vec4 & first, vec4 const & second) {
	first.x /= second.x;
	first.y /= second.y;
	first.z /= second.z;
	first.w /= second.w;
	return first;
}

constexpr inline vec4 & operator/=(vec4 & first, r32 second) {
	first.x /= second;
	first.y /= second;
	first.z /= second;
	first.w /= second;
	return first;
}
#endif // defined(__cplusplus) // operator vec4

constexpr inline r32 dot_product(vec4 first, vec4 second) {
	return first.x * second.x
	     + first.y * second.y
	     + first.z * second.z
	     + first.w * second.w;
}

constexpr inline vec4 min(vec4 first, vec4 second) {
	return {
		min(first.x, second.x),
		min(first.y, second.y),
		min(first.z, second.z),
		min(first.w, second.w)
	};
}

constexpr inline vec4 max(vec4 first, vec4 second) {
	return {
		max(first.x, second.x),
		max(first.y, second.y),
		max(first.z, second.z),
		max(first.w, second.w)
	};
}

constexpr inline r32 min(vec4 value) {
	return min(min(min(value.x, value.y), value.z), value.w);
}

constexpr inline r32 max(vec4 value) {
	return max(max(max(value.x, value.y), value.z), value.w);
}

constexpr inline vec4 absolute(vec4 value) {
	return {
		absolute(value.x),
		absolute(value.y),
		absolute(value.z),
		absolute(value.w)
	};
}

constexpr inline vec4 sign(vec4 value) {
	return {
		sign(value.x),
		sign(value.y),
		sign(value.z),
		sign(value.w)
	};
}

constexpr inline vec4 interpolate(vec4 from, vec4 to, r32 fraction) {
	return {
		interpolate(from.x, to.x, fraction),
		interpolate(from.y, to.y, fraction),
		interpolate(from.z, to.z, fraction),
		interpolate(from.w, to.w, fraction)
	};
}

inline vec4 square_root(vec4 value) {
	return {
		square_root(value.x),
		square_root(value.y),
		square_root(value.z),
		square_root(value.w)
	};
}

inline vec4 sine(vec4 value) {
	return {
		sine(value.x),
		sine(value.y),
		sine(value.z),
		sine(value.w)
	};
}

inline vec4 cosine(vec4 value) {
	return {
		cosine(value.x),
		cosine(value.y),
		cosine(value.z),
		cosine(value.w)
	};
}

//
// icomplex
//

constexpr inline icomplex complex_reciprocal(icomplex value) {
	return {value.x, -value.y};
}

constexpr inline icomplex complex_multiply(icomplex first, icomplex second) {
	return {
		first.x * second.x - first.y * second.y,
		first.x * second.y + first.y * second.x
	};
}

constexpr inline icomplex complex_rotate_vector(icomplex complex, ivec2 vector) {
	return complex_multiply(complex, vector);
}

//
// complex
// code assumes normalized values
//

/*
complex number math representation:
complex = x + y * i
complex = e ^ (angle * i)
complex = cos(angle) + i * sin(angle)

i * i = -1

* https://en.wikipedia.org/wiki/Complex_number
Rotation formula (by angle A)
> > V' = e^(A*i) * V
*/

inline complex complex_from_radians(r32 radians) {
	return {cosine(radians), sine(radians)};
}

constexpr inline complex complex_reciprocal(complex value) {
	// @Note: normalized value do not need division by magnitude_squared(value)
	return {value.x, -value.y};
}

constexpr inline complex complex_multiply(complex first, complex second) {
	return {
		first.x * second.x - first.y * second.y,
		first.x * second.y + first.y * second.x
	};
}

constexpr inline complex complex_rotate_vector(complex complex, vec2 vector) {
	return complex_multiply(complex, vector);
}

//
// quat
// code assumes normalized values
//

/*
quat math representation:
quat = (w + x * i) + (y + z * i) * j
quat = w + (x * i) + (y * j) + (z * k)
quat = e ^ (angle * axis)
quat = cos(angle) + axis * sin(angle)

(i * i) = (j * j) = (k * k) = (i * j * k) = -1

(i * j) = -(j * i) = k
(j * k) = -(k * j) = i
(k * i) = -(i * k) = j

* https://en.wikipedia.org/wiki/Quaternion
* https://www.youtube.com/watch?v=UaK2q22mMEg
* https://www.youtube.com/watch?v=q-ESzg03mQc
* https://www.youtube.com/watch?v=zjMuIxRvygQ
* https://www.youtube.com/watch?v=d4EgbgTm0Bg

Vector operations:
> > V = V_par + V_perp
> > NxV_par == 0
> > NxV_perp == NxV

* https://erkaman.github.io/posts/quaternion_rotation.html
* https://math.stackexchange.com/questions/1579336/how-do-quaternions-return-the-rodriguez-formula-for-rotations
* https://en.wikipedia.org/wiki/Euler-Rodrigues_formula
* https://en.wikipedia.org/wiki/Rodrigues_rotation_formula
Rodrigues' rotation formula (by angle A around axis N)
> V is parallel to axis N
> > V_par' = V_par

> V is perpendicular to axis N
> > V_perp' = V_perp * cos(A) + (NxV_perp) * sin(A)

> V is arbitrary to axis N
> > V = V_par + V_perp
> > V' = V_par + V_perp'
> > V' = V * cos(A) + (NxV) * sin(A) + N*(N*V) * (1 - cos(A))

> As you see, it's a bit cumbersome and requires trigonometric operations

quat rotation formula (by angle A around axis N)
> V is parallel to axis N
> > V_par' = V_par
> > e^(A*N) * V_par == V_par * e^(A*N)

> V is perpendicular to axis N
> > V_perp' = e^(A*N) * V_perp
> > e^(A*N) * V_perp == V_perp * e^(-A*N)

> V is arbitrary to axis N
> > V = V_par + V_perp
> > V' = V_par + V_perp'
> > V' = e^(A*N/2) * V * e^(-A*N/2)

> As you see, it's concise and avoids trigonometric operations
> > also that's why we specifically use (half_radians = euler_radians / 2) in the code
*/

constexpr inline quat quat_reciprocal(quat value) {
	// @Note: normalized value do not need division by magnitude_squared(value)
	return {-value.x, -value.y, -value.z, value.w};
}

inline quat quat_from_axis(vec3 axis, r32 radians) {
	r32 half_radians = radians / 2;
	r32 s = sine(half_radians);
	r32 c = cosine(half_radians);
	return {axis.x * s, axis.y * s, axis.z * s, c};
}

constexpr inline quat quat_multiply(quat first, quat second) {
	return {
		cross_product(first.xyz, second.xyz) + first.xyz * second.w + second.xyz * first.w,
		first.w * second.w - dot_product(first.xyz, second.xyz)
	};
}

/*
This code is a result of expanding following expression,
excluding stuff negated by multiplication with zero

return quat_multiply(
	quat_multiply(
		quat_from_axis({0, 1, 0}, euler_radians.y)
		quat_from_axis({1, 0, 0}, euler_radians.x)
	),
	quat_from_axis({0, 0, 1}, euler_radians.z),
);
*/
inline quat quat_from_radians(vec3 euler_radians) {
	auto half_radians = euler_radians / 2;
	auto s = sine(half_radians);
	auto c = cosine(half_radians);
	return {
		s.y * c.x * s.z + c.y * s.x * c.z,
		s.y * c.x * c.z - c.y * s.x * s.z,
		c.y * c.x * s.z - s.y * s.x * c.z,
		c.y * c.x * c.z + s.y * s.x * s.z
	};
};

/*
This code is a result of expanding following expression,
excluding stuff negated by multiplication with zero

return quat_multiply(
	quat_multiply(quat, {vector, 0}),
	quat_reciprocal(quat)
);
*/
constexpr inline vec3 quat_rotate_vector(quat q, vec3 vector) {
	quat reciprocal = quat_reciprocal(q);
	
	vec3 product_axis = cross_product(q.xyz, vector) + vector * q.w;
	return cross_product(product_axis, reciprocal.xyz)
	     + product_axis * reciprocal.w
	     - reciprocal.xyz * dot_product(q.xyz, vector);
}

/*
This code is a result of expanding following expressions,
excluding stuff negated by multiplication with zero

right   = quat_rotate_vector(quat, {1, 0, 0});
up      = quat_rotate_vector(quat, {0, 1, 0});
forward = quat_rotate_vector(quat, {0, 0, 1});
*/
inline void quat_get_axes(quat q, vec3 & right, vec3 & up, vec3 & forward) {
	quat reciprocal = quat_reciprocal(q);

	vec3 product_axis_a = {q.w, q.z, -q.y};
	right = cross_product(product_axis_a, reciprocal.xyz)
	      + product_axis_a * reciprocal.w
	      - reciprocal.xyz * q.x;

	vec3 product_axis_b = {-q.z, q.w, q.x};
	up = cross_product(product_axis_b, reciprocal.xyz)
	   + product_axis_b * reciprocal.w
	   - reciprocal.xyz * q.y;

	vec3 product_axis_c = {q.y, -q.x, q.w};
	forward = cross_product(product_axis_c, reciprocal.xyz)
	        + product_axis_c * reciprocal.w
	        - reciprocal.xyz * q.z;
};

constexpr inline vec3 quat_get_right(quat q) {
	quat reciprocal = quat_reciprocal(q);

	vec3 product_axis_a = {q.w, q.z, -q.y};
	return cross_product(product_axis_a, reciprocal.xyz)
	     + product_axis_a * reciprocal.w
	     - reciprocal.xyz * q.x;
};

constexpr inline vec3 quat_get_up(quat q) {
	quat reciprocal = quat_reciprocal(q);

	vec3 product_axis_b = {-q.z, q.w, q.x};
	return cross_product(product_axis_b, reciprocal.xyz)
	     + product_axis_b * reciprocal.w
	     - reciprocal.xyz * q.y;
};

constexpr inline vec3 quat_get_forward(quat q) {
	quat reciprocal = quat_reciprocal(q);

	vec3 product_axis_c = {q.y, -q.x, q.w};
	return cross_product(product_axis_c, reciprocal.xyz)
	     + product_axis_c * reciprocal.w
	     - reciprocal.xyz * q.z;
};

//
// Macro implementations
//

#define CLAMP_IMPL(T)\
constexpr inline T clamp(T value, T low, T high) {\
	return min(max(value, low), high);\
}

#define MOVE_TOWARDS_CLAMPED_IMPL(T)\
constexpr inline T move_towards_clamped(T from, T to, T delta) {\
	return clamp(from + delta, from, to);\
}

#define VECTOR_MAGNITUDE_SQUARED_IMPL(T, S)\
constexpr inline S magnitude_squared(T value) {\
	return dot_product(value, value);\
}

#define VECTOR_MAGNITUDE_IMPL(T)\
inline r32 magnitude(T value) {\
	return square_root(magnitude_squared(value));\
}

#define VECTOR_NORMALIZE_IMPL(T)\
inline T normalize(T value) {\
	return value / magnitude(value);\
}

#define VECTOR_REFLECT_IMPL(T)\
constexpr inline T reflect(T incident, T normal, r32 factor) {\
	r32 incident_cosine = dot_product(incident, normal);\
	r32 normal_factor   = incident_cosine * factor;\
	return incident - normal * normal_factor;\
}

#define VECTOR_REFRACT_IMPL(T)\
inline T refract(T incident, T normal, r32 factor) {\
	r32 incident_cosine = dot_product(incident, normal);\
	r32 incident_sine_squared = 1 - incident_cosine * incident_cosine;\
	r32 refracted_sine_squared = factor * factor * incident_sine_squared;\
	if (refracted_sine_squared < 1) {\
		r32 refracted_cosine = square_root(1 - refracted_sine_squared);\
		r32 normal_factor = incident_cosine * factor + refracted_cosine;\
		return incident * factor - normal * normal_factor;\
	}\
	return {};\
}

#define VECTOR_MOVE_TOWARDS_CLAMPED_IMPL(T)\
inline T move_towards_clamped(T from, T to, r32 delta) {\
	T direction = to - from;\
	r32 distance = magnitude(direction);\
	if (delta <= 0) { return from; }\
	if (delta >= distance) { return to; }\
	return from + (direction / distance) * delta;\
}

#define VECTOR_CLAMP_MAGNITUDE_IMPL(T)\
inline T clamp_magnitude(T value, r32 low, r32 high) {\
	r32 length = magnitude(value);\
	if (length < low) { return (value / length) * low; }\
	if (length > high) { return (value / length) * high; }\
	return value;\
}

CLAMP_IMPL(s32)
CLAMP_IMPL(ivec2)
CLAMP_IMPL(ivec3)

CLAMP_IMPL(r32)
CLAMP_IMPL(vec2)
CLAMP_IMPL(vec3)
CLAMP_IMPL(vec4)

MOVE_TOWARDS_CLAMPED_IMPL(s32);
MOVE_TOWARDS_CLAMPED_IMPL(r32);

VECTOR_MAGNITUDE_SQUARED_IMPL(ivec2, s32)
VECTOR_MAGNITUDE_SQUARED_IMPL(ivec3, s32)

VECTOR_MAGNITUDE_SQUARED_IMPL(vec2, r32)
VECTOR_MAGNITUDE_SQUARED_IMPL(vec3, r32)
VECTOR_MAGNITUDE_SQUARED_IMPL(vec4, r32)

VECTOR_MAGNITUDE_IMPL(vec2)
VECTOR_MAGNITUDE_IMPL(vec3)
VECTOR_MAGNITUDE_IMPL(vec4)

VECTOR_NORMALIZE_IMPL(vec2)
VECTOR_NORMALIZE_IMPL(vec3)
VECTOR_NORMALIZE_IMPL(vec4)

VECTOR_REFLECT_IMPL(vec2)
VECTOR_REFLECT_IMPL(vec3)

VECTOR_REFRACT_IMPL(vec2)
VECTOR_REFRACT_IMPL(vec3)

VECTOR_MOVE_TOWARDS_CLAMPED_IMPL(vec2)
VECTOR_MOVE_TOWARDS_CLAMPED_IMPL(vec3)
VECTOR_MOVE_TOWARDS_CLAMPED_IMPL(vec4)

VECTOR_CLAMP_MAGNITUDE_IMPL(vec2)
VECTOR_CLAMP_MAGNITUDE_IMPL(vec3)
VECTOR_CLAMP_MAGNITUDE_IMPL(vec4)

#undef CLAMP_IMPL
#undef MOVE_TOWARDS_CLAMPED_IMPL
#undef VECTOR_MAGNITUDE_SQUARED_IMPL
#undef VECTOR_MAGNITUDE_IMPL
#undef VECTOR_NORMALIZE_IMPL
#undef VECTOR_REFLECT_IMPL
#undef VECTOR_REFRACT_IMPL
#undef VECTOR_MOVE_TOWARDS_CLAMPED_IMPL
#undef VECTOR_CLAMP_MAGNITUDE_IMPL
