#pragma once
#include "engine/core/math_types.h"
#include "engine/impl/math_scalar.h"

//
//
//

/*
vector 3d math representation
(i * j) = -(j * i) = k
(j * k) = -(k * j) = i
(k * i) = -(i * k) = j

everything's left-handed

---- cross product ----
(i * i) = (j * j) = (k * k) = (i * j * k) =  0 == sin(0)

----  dot product  ----
(i * i) = (j * j) = (k * k) = (i * j * k) =  1 == cos(0)

----   quaternion  ----
(i * i) = (j * j) = (k * k) = (i * j * k) = -1 == cos(180)
*/

/*
This code is a result of expanding the following expression
(x1*i + y1*j) * (x2*i + y2*j)
this function is a shortcut through vec2 instead of vec3
*/
template<typename T>
constexpr inline T cross_product(xvec2<T> const & first, xvec2<T> const & second) {
	return first.x * second.y - first.y * second.x;
}

/*
This code is a result of expanding the following expression
(x1*i + y1*j) * (x2*k)
this function is a shortcut through vec2 instead of vec3
*/
template<typename T>
constexpr inline xvec2<T> cross_product(xvec2<T> const & first, T second) {
	return {first.y * second, -first.x * second};
}

/*
This code is a result of expanding the following expression
(x1*k) * (x2*i + y2*j)
this function is a shortcut through vec2 instead of vec3
*/
template<typename T>
constexpr inline xvec2<T> cross_product(T first, xvec2<T> const & second) {
	return {-first * second.y, first * second.x};
}

/*
This code is a result of expanding the following expression
(x1*i + y1*j + z1*k) * (x2*i + y2*j + z2*k)
*/
template<typename T>
constexpr inline xvec3<T> cross_product(xvec3<T> const & first, xvec3<T> const & second) {
	return {
		first.y * second.z - first.z * second.y,
		first.z * second.x - first.x * second.z,
		first.x * second.y - first.y * second.x
	};
}

//
// xvec2<T> routines
// Applications: 2d position, 2d rotation as complex number, 2d scale, 2d direction
//

#if defined(__cplusplus) // operator xvec2<T>
template<typename T>
constexpr inline bool operator==(xvec2<T> const & first, xvec2<T> const & second) {
	return (first.x == second.x)
	    && (first.y == second.y);
}

template<typename T>
constexpr inline bool operator!=(xvec2<T> const & first, xvec2<T> const & second) {
	return (first.x != second.x)
	    || (first.y != second.y);
}

template<typename T>
constexpr inline xvec2<T> operator+(xvec2<T> const & first, xvec2<T> const & second) {
	return {
		first.x + second.x,
		first.y + second.y
	};
}

template<typename T>
constexpr inline xvec2<T> operator+(xvec2<T> const & first, T second) {
	return {
		first.x + second,
		first.y + second
	};
}

template<typename T>
constexpr inline xvec2<T> operator-(xvec2<T> const & first, xvec2<T> const & second) {
	return {
		first.x - second.x,
		first.y - second.y
	};
}

template<typename T>
constexpr inline xvec2<T> operator-(xvec2<T> const & first, T second) {
	return {
		first.x - second,
		first.y - second
	};
}

template<typename T>
constexpr inline xvec2<T> operator-(xvec2<T> const & value) {
	return {
		-value.x,
		-value.y
	};
}

template<typename T>
constexpr inline xvec2<T> operator*(xvec2<T> const & first, T second) {
	return {
		first.x * second,
		first.y * second
	};
}

template<typename T>
constexpr inline xvec2<T> operator*(xvec2<T> const & first, xvec2<T> const & second) {
	return {
		first.x * second.x,
		first.y * second.y
	};
}

template<typename T>
constexpr inline xvec2<T> operator/(xvec2<T> const & first, T second) {
	return {
		first.x / second,
		first.y / second
	};
}

template<typename T>
constexpr inline xvec2<T> operator/(xvec2<T> const & first, xvec2<T> const & second) {
	return {
		first.x / second.x,
		first.y / second.y
	};
}

template<typename T>
constexpr inline xvec2<T> & operator+=(xvec2<T> & first, xvec2<T> const & second) {
	first.x += second.x;
	first.y += second.y;
	return first;
}

template<typename T>
constexpr inline xvec2<T> & operator+=(xvec2<T> & first, T second) {
	first.x += second;
	first.y += second;
	return first;
}

template<typename T>
constexpr inline xvec2<T> & operator-=(xvec2<T> & first, xvec2<T> const & second) {
	first.x -= second.x;
	first.y -= second.y;
	return first;
}

template<typename T>
constexpr inline xvec2<T> & operator-=(xvec2<T> & first, T second) {
	first.x -= second;
	first.y -= second;
	return first;
}

template<typename T>
constexpr inline xvec2<T> & operator*=(xvec2<T> & first, xvec2<T> const & second) {
	first.x *= second.x;
	first.y *= second.y;
	return first;
}

template<typename T>
constexpr inline xvec2<T> & operator*=(xvec2<T> & first, T second) {
	first.x *= second;
	first.y *= second;
	return first;
}

template<typename T>
constexpr inline xvec2<T> & operator/=(xvec2<T> & first, xvec2<T> const & second) {
	first.x /= second.x;
	first.y /= second.y;
	return first;
}

template<typename T>
constexpr inline xvec2<T> & operator/=(xvec2<T> & first, T second) {
	first.x /= second;
	first.y /= second;
	return first;
}
#endif // defined(__cplusplus) // operator xvec2<T>

/*
This code is a result of expanding the following expression
(x1*i + y1*j) * (x2*i + y2*j)
(i * i) = (j * j) = 1 == cos(0)
*/
template<typename T>
constexpr inline T dot_product(xvec2<T> const & first, xvec2<T> const & second) {
	return first.x * second.x
	     + first.y * second.y;
}

template<typename T>
constexpr inline xvec2<T> min(xvec2<T> const & first, xvec2<T> const & second) {
	return {
		min(first.x, second.x),
		min(first.y, second.y)
	};
}

template<typename T>
constexpr inline xvec2<T> max(xvec2<T> const & first, xvec2<T> const & second) {
	return {
		max(first.x, second.x),
		max(first.y, second.y)
	};
}

template<typename T>
constexpr inline T min(xvec2<T> const & value) {
	return min(value.x, value.y);
}

template<typename T>
constexpr inline T max(xvec2<T> const & value) {
	return max(value.x, value.y);
}

template<typename T>
constexpr inline xvec2<T> absolute(xvec2<T> const & value) {
	return {
		absolute(value.x),
		absolute(value.y)
	};
}

template<typename T>
constexpr inline xvec2<T> sign(xvec2<T> const & value) {
	return {
		sign(value.x),
		sign(value.y)
	};
}

template<typename T>
constexpr inline xvec2<T> interpolate(xvec2<T> const & from, xvec2<T> const & to, T fraction) {
	return {
		interpolate(from.x, to.x, fraction),
		interpolate(from.y, to.y, fraction)
	};
}

template<typename T>
inline xvec2<T> square_root(xvec2<T> const & value) {
	return {
		square_root(value.x),
		square_root(value.y)
	};
}

template<typename T>
inline xvec2<T> sine(xvec2<T> const & value) {
	return {
		sine(value.x),
		sine(value.y)
	};
}

template<typename T>
inline xvec2<T> cosine(xvec2<T> const & value) {
	return {
		cosine(value.x),
		cosine(value.y)
	};
}

//
// xvec3<T> routines
// Applications: 3d position, 3d rotation as euler angles, 3d scale, 3d direction
//

#if defined(__cplusplus) // operator xvec3<T>
template<typename T>
constexpr inline bool operator==(xvec3<T> const & first, xvec3<T> const & second) {
	return (first.x == second.x)
	    && (first.y == second.y)
	    && (first.z == second.z);
}

template<typename T>
constexpr inline bool operator!=(xvec3<T> const & first, xvec3<T> const & second) {
	return (first.x != second.x)
	    || (first.y != second.y)
	    || (first.z != second.z);
}

template<typename T>
constexpr inline xvec3<T> operator+(xvec3<T> const & first, xvec3<T> const & second) {
	return {
		first.x + second.x,
		first.y + second.y,
		first.z + second.z
	};
}

template<typename T>
constexpr inline xvec3<T> operator+(xvec3<T> const & first, T second) {
	return {
		first.x + second,
		first.y + second,
		first.z + second
	};
}

template<typename T>
constexpr inline xvec3<T> operator-(xvec3<T> const & first, xvec3<T> const & second) {
	return {
		first.x - second.x,
		first.y - second.y,
		first.z - second.z
	};
}

template<typename T>
constexpr inline xvec3<T> operator-(xvec3<T> const & first, T second) {
	return {
		first.x - second,
		first.y - second,
		first.z - second
	};
}

template<typename T>
constexpr inline xvec3<T> operator-(xvec3<T> const & value) {
	return {
		-value.x,
		-value.y,
		-value.z
	};
}

template<typename T>
constexpr inline xvec3<T> operator*(xvec3<T> const & first, T second) {
	return {
		first.x * second,
		first.y * second,
		first.z * second
	};
}

template<typename T>
constexpr inline xvec3<T> operator*(xvec3<T> const & first, xvec3<T> const & second) {
	return {
		first.x * second.x,
		first.y * second.y,
		first.z * second.z
	};
}

template<typename T>
constexpr inline xvec3<T> operator/(xvec3<T> const & first, T second) {
	return {
		first.x / second,
		first.y / second,
		first.z / second
	};
}

template<typename T>
constexpr inline xvec3<T> operator/(xvec3<T> const & first, xvec3<T> const & second) {
	return {
		first.x / second.x,
		first.y / second.y,
		first.z / second.z
	};
}

template<typename T>
constexpr inline xvec3<T> & operator+=(xvec3<T> & first, xvec3<T> const & second) {
	first.x += second.x;
	first.y += second.y;
	first.z += second.z;
	return first;
}

template<typename T>
constexpr inline xvec3<T> & operator+=(xvec3<T> & first, T second) {
	first.x += second;
	first.y += second;
	first.z += second;
	return first;
}

template<typename T>
constexpr inline xvec3<T> & operator-=(xvec3<T> & first, xvec3<T> const & second) {
	first.x -= second.x;
	first.y -= second.y;
	first.z -= second.z;
	return first;
}

template<typename T>
constexpr inline xvec3<T> & operator-=(xvec3<T> & first, T second) {
	first.x -= second;
	first.y -= second;
	first.z -= second;
	return first;
}

template<typename T>
constexpr inline xvec3<T> & operator*=(xvec3<T> & first, xvec3<T> const & second) {
	first.x *= second.x;
	first.y *= second.y;
	first.z *= second.z;
	return first;
}

template<typename T>
constexpr inline xvec3<T> & operator*=(xvec3<T> & first, T second) {
	first.x *= second;
	first.y *= second;
	first.z *= second;
	return first;
}

template<typename T>
constexpr inline xvec3<T> & operator/=(xvec3<T> & first, xvec3<T> const & second) {
	first.x /= second.x;
	first.y /= second.y;
	first.z /= second.z;
	return first;
}

template<typename T>
constexpr inline xvec3<T> & operator/=(xvec3<T> & first, T second) {
	first.x /= second;
	first.y /= second;
	first.z /= second;
	return first;
}
#endif // defined(__cplusplus) // operator xvec3<T>

/*
This code is a result of expanding the following expression
(x1*i + y1*j + z1*k) * (x2*i + y2*j + z2*k)
*/
template<typename T>
constexpr inline T dot_product(xvec3<T> const & first, xvec3<T> const & second) {
	return first.x * second.x
	     + first.y * second.y
	     + first.z * second.z;
}

template<typename T>
constexpr inline xvec3<T> min(xvec3<T> const & first, xvec3<T> const & second) {
	return {
		min(first.x, second.x),
		min(first.y, second.y),
		min(first.z, second.z)
	};
}

template<typename T>
constexpr inline xvec3<T> max(xvec3<T> const & first, xvec3<T> const & second) {
	return {
		max(first.x, second.x),
		max(first.y, second.y),
		max(first.z, second.z)
	};
}

template<typename T>
constexpr inline T min(xvec3<T> const & value) {
	return min(min(value.x, value.y), value.z);
}

template<typename T>
constexpr inline T max(xvec3<T> const & value) {
	return max(max(value.x, value.y), value.z);
}

template<typename T>
constexpr inline xvec3<T> absolute(xvec3<T> const & value) {
	return {
		absolute(value.x),
		absolute(value.y),
		absolute(value.z)
	};
}

template<typename T>
constexpr inline xvec3<T> sign(xvec3<T> const & value) {
	return {
		sign(value.x),
		sign(value.y),
		sign(value.z)
	};
}

template<typename T>
constexpr inline xvec3<T> interpolate(xvec3<T> const & from, xvec3<T> const & to, T fraction) {
	return {
		interpolate(from.x, to.x, fraction),
		interpolate(from.y, to.y, fraction),
		interpolate(from.z, to.z, fraction)
	};
}

template<typename T>
inline xvec3<T> square_root(xvec3<T> const & value) {
	return {
		square_root(value.x),
		square_root(value.y),
		square_root(value.z)
	};
}

template<typename T>
inline xvec3<T> sine(xvec3<T> const & value) {
	return {
		sine(value.x),
		sine(value.y),
		sine(value.z)
	};
}

template<typename T>
inline xvec3<T> cosine(xvec3<T> const & value) {
	return {
		cosine(value.x),
		cosine(value.y),
		cosine(value.z)
	};
}

//
// xvec4<T> routines
// Applications: 3d rotation as quat, color
//

#if defined(__cplusplus) // operator xvec4<T>
template<typename T>
constexpr inline bool operator==(xvec4<T> const & first, xvec4<T> const & second) {
	return (first.x == second.x)
	    && (first.y == second.y)
	    && (first.z == second.z)
	    && (first.w == second.w);
}

template<typename T>
constexpr inline bool operator!=(xvec4<T> const & first, xvec4<T> const & second) {
	return (first.x != second.x)
	    || (first.y != second.y)
	    || (first.z != second.z)
	    || (first.w != second.w);
}

template<typename T>
constexpr inline xvec4<T> operator+(xvec4<T> const & first, xvec4<T> const & second) {
	return {
		first.x + second.x,
		first.y + second.y,
		first.z + second.z,
		first.w + second.w
	};
}

template<typename T>
constexpr inline xvec4<T> operator+(xvec4<T> const & first, T second) {
	return {
		first.x + second,
		first.y + second,
		first.z + second,
		first.w + second
	};
}

template<typename T>
constexpr inline xvec4<T> operator-(xvec4<T> const & first, xvec4<T> const & second) {
	return {
		first.x - second.x,
		first.y - second.y,
		first.z - second.z,
		first.w - second.w
	};
}

template<typename T>
constexpr inline xvec4<T> operator-(xvec4<T> const & first, T second) {
	return {
		first.x - second,
		first.y - second,
		first.z - second,
		first.w - second
	};
}

template<typename T>
constexpr inline xvec4<T> operator-(xvec4<T> const & value) {
	return {
		-value.x,
		-value.y,
		-value.z,
		-value.w
	};
}

template<typename T>
constexpr inline xvec4<T> operator*(xvec4<T> const & first, T second) {
	return {
		first.x * second,
		first.y * second,
		first.z * second,
		first.w * second
	};
}

template<typename T>
constexpr inline xvec4<T> operator*(xvec4<T> const & first, xvec4<T> const & second) {
	return {
		first.x * second.x,
		first.y * second.y,
		first.z * second.z,
		first.w * second.w
	};
}

template<typename T>
constexpr inline xvec4<T> operator/(xvec4<T> const & first, T second) {
	return {
		first.x / second,
		first.y / second,
		first.z / second,
		first.w / second
	};
}

template<typename T>
constexpr inline xvec4<T> operator/(xvec4<T> const & first, xvec4<T> const & second) {
	return {
		first.x / second.x,
		first.y / second.y,
		first.z / second.z,
		first.w / second.w
	};
}

template<typename T>
constexpr inline xvec4<T> & operator+=(xvec4<T> & first, xvec4<T> const & second) {
	first.x += second.x;
	first.y += second.y;
	first.z += second.z;
	first.w += second.w;
	return first;
}

template<typename T>
constexpr inline xvec4<T> & operator+=(xvec4<T> & first, T second) {
	first.x += second;
	first.y += second;
	first.z += second;
	first.w += second;
	return first;
}

template<typename T>
constexpr inline xvec4<T> & operator-=(xvec4<T> & first, xvec4<T> const & second) {
	first.x -= second.x;
	first.y -= second.y;
	first.z -= second.z;
	first.w -= second.w;
	return first;
}

template<typename T>
constexpr inline xvec4<T> & operator-=(xvec4<T> & first, T second) {
	first.x -= second;
	first.y -= second;
	first.z -= second;
	first.w -= second;
	return first;
}

template<typename T>
constexpr inline xvec4<T> & operator*=(xvec4<T> & first, xvec4<T> const & second) {
	first.x *= second.x;
	first.y *= second.y;
	first.z *= second.z;
	first.w *= second.w;
	return first;
}

template<typename T>
constexpr inline xvec4<T> & operator*=(xvec4<T> & first, T second) {
	first.x *= second;
	first.y *= second;
	first.z *= second;
	first.w *= second;
	return first;
}

template<typename T>
constexpr inline xvec4<T> & operator/=(xvec4<T> & first, xvec4<T> const & second) {
	first.x /= second.x;
	first.y /= second.y;
	first.z /= second.z;
	first.w /= second.w;
	return first;
}

template<typename T>
constexpr inline xvec4<T> & operator/=(xvec4<T> & first, T second) {
	first.x /= second;
	first.y /= second;
	first.z /= second;
	first.w /= second;
	return first;
}
#endif // defined(__cplusplus) // operator xvec4<T>

template<typename T>
constexpr inline T dot_product(xvec4<T> const & first, xvec4<T> const & second) {
	return first.x * second.x
	     + first.y * second.y
	     + first.z * second.z
	     + first.w * second.w;
}

template<typename T>
constexpr inline xvec4<T> min(xvec4<T> const & first, xvec4<T> const & second) {
	return {
		min(first.x, second.x),
		min(first.y, second.y),
		min(first.z, second.z),
		min(first.w, second.w)
	};
}

template<typename T>
constexpr inline xvec4<T> max(xvec4<T> const & first, xvec4<T> const & second) {
	return {
		max(first.x, second.x),
		max(first.y, second.y),
		max(first.z, second.z),
		max(first.w, second.w)
	};
}

template<typename T>
constexpr inline T min(xvec4<T> const & value) {
	return min(min(min(value.x, value.y), value.z), value.w);
}

template<typename T>
constexpr inline T max(xvec4<T> const & value) {
	return max(max(max(value.x, value.y), value.z), value.w);
}

template<typename T>
constexpr inline xvec4<T> absolute(xvec4<T> const & value) {
	return {
		absolute(value.x),
		absolute(value.y),
		absolute(value.z),
		absolute(value.w)
	};
}

template<typename T>
constexpr inline xvec4<T> sign(xvec4<T> const & value) {
	return {
		sign(value.x),
		sign(value.y),
		sign(value.z),
		sign(value.w)
	};
}

template<typename T>
constexpr inline xvec4<T> interpolate(xvec4<T> const & from, xvec4<T> const & to, T fraction) {
	return {
		interpolate(from.x, to.x, fraction),
		interpolate(from.y, to.y, fraction),
		interpolate(from.z, to.z, fraction),
		interpolate(from.w, to.w, fraction)
	};
}

template<typename T>
inline xvec4<T> square_root(xvec4<T> const & value) {
	return {
		square_root(value.x),
		square_root(value.y),
		square_root(value.z),
		square_root(value.w)
	};
}

template<typename T>
inline xvec4<T> sine(xvec4<T> const & value) {
	return {
		sine(value.x),
		sine(value.y),
		sine(value.z),
		sine(value.w)
	};
}

template<typename T>
inline xvec4<T> cosine(xvec4<T> const & value) {
	return {
		cosine(value.x),
		cosine(value.y),
		cosine(value.z),
		cosine(value.w)
	};
}

//
// icomplex routines
//

constexpr inline icomplex complex_conjugate(icomplex const & value) {
	return {value.x, -value.y};
}

constexpr inline icomplex complex_product(icomplex const & first, icomplex const & second) {
	return {
		first.x * second.x - first.y * second.y,
		first.x * second.y + first.y * second.x
	};
}

//
// complex routines
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

constexpr inline complex complex_conjugate(complex const & value) {
	return {value.x, -value.y};
}

// @Note: normalized value do not need reciprocal: a conjugate suffice
constexpr inline r32 magnitude_squared(vec2 const & value);
constexpr inline complex complex_reciprocal(complex const & value) {
	return complex_conjugate(value) / magnitude_squared(value);
}

constexpr inline complex complex_product(complex const & first, complex const & second) {
	return {
		first.x * second.x - first.y * second.y,
		first.x * second.y + first.y * second.x
	};
}

constexpr inline void complex_get_axes(complex const & c, vec2 & x, vec2 & y) {
	x = c;
	y = cross_product(1.0f, c);
}

inline r32 complex_get_radians(complex const & value) {
	return atan2f(value.y, value.x);
}

//
// quaternion routines
// code assumes normalized values
//

/*
quaternion math representation:
quat = (w + x * i) + (y + z * i) * j
quat = w + (x * i) + (y * j) + (z * k)
quat = e ^ (angle * axis)
quat = cos(angle) + axis * sin(angle)

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

quaternion rotation formula (by angle A around axis N)
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

constexpr inline quat quat_conjugate(quat const & value) {
	return {-value.x, -value.y, -value.z, value.w};
}

// @Note: normalized value do not need reciprocal: a conjugate suffice
constexpr inline r32 magnitude_squared(vec4 const & value);
constexpr inline quat quat_reciprocal(quat value) {
	return quat{-value.x, -value.y, -value.z, value.w} / magnitude_squared(value);
}

inline quat quat_from_axis(vec3 const & axis, r32 radians) {
	r32 half_radians = radians / 2;
	r32 s = sine(half_radians);
	r32 c = cosine(half_radians);
	return {axis.x * s, axis.y * s, axis.z * s, c};
}

/*
This code is a result of expanding the following expression
(x1*i + y1*j + z1*k + w1) * (x2*i + y2*j + z2*k + w2)
(i * i) = (j * j) = (k * k) = (i * j * k) = -1 == cos(pi)
*/
constexpr inline quat quat_product(quat const & first, quat const & second) {
	return {
		cross_product(first.xyz, second.xyz) + first.xyz * second.w + second.xyz * first.w,
		first.w * second.w - dot_product(first.xyz, second.xyz)
	};
}

/*
This code is a result of expanding the following expression,
excluding stuff negated by multiplication with zero

return quat_product(
	quat_product(
		quat_from_axis({0, 1, 0}, euler_radians.y)
		quat_from_axis({1, 0, 0}, euler_radians.x)
	),
	quat_from_axis({0, 0, 1}, euler_radians.z),
);
*/
inline quat quat_from_radians(vec3 const & euler_radians) {
	auto half_radians = euler_radians / (r32)2;
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
This code is a result of expanding the following expression,
excluding stuff negated by multiplication with zero

return quat_product(
	quat_product(quat, {vector, 0}),
	quat_reciprocal(quat)
);
*/
constexpr inline vec3 quat_rotate(quat const & q, vec3 const & vector) {
	// @Note: normalized value do not need reciprocal: a conjugate suffice
	quat reciprocal = quat_conjugate(q);
	vec3 product_axis = cross_product(q.xyz, vector) + vector * q.w;
	return cross_product(product_axis, reciprocal.xyz)
	     + product_axis * reciprocal.w
	     - reciprocal.xyz * dot_product(q.xyz, vector);
}

/*
This code is a result of expanding the following expressions,
excluding stuff negated by multiplication with zero

right   = quat_rotate(quat, {1, 0, 0});
up      = quat_rotate(quat, {0, 1, 0});
forward = quat_rotate(quat, {0, 0, 1});
*/
constexpr inline void quat_get_axes(quat const & q, vec3 & right, vec3 & up, vec3 & forward) {
	// @Note: normalized value do not need reciprocal: a conjugate suffice
	quat reciprocal = quat_conjugate(q);

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

constexpr inline vec3 quat_get_right(quat const & q) {
	// @Note: normalized value do not need reciprocal: a conjugate suffice
	quat reciprocal = quat_conjugate(q);
	vec3 product_axis_a = {q.w, q.z, -q.y};
	return cross_product(product_axis_a, reciprocal.xyz)
	     + product_axis_a * reciprocal.w
	     - reciprocal.xyz * q.x;
};

constexpr inline vec3 quat_get_up(quat const & q) {
	// @Note: normalized value do not need reciprocal: a conjugate suffice
	quat reciprocal = quat_conjugate(q);
	vec3 product_axis_b = {-q.z, q.w, q.x};
	return cross_product(product_axis_b, reciprocal.xyz)
	     + product_axis_b * reciprocal.w
	     - reciprocal.xyz * q.y;
};

constexpr inline vec3 quat_get_forward(quat const & q) {
	// @Note: normalized value do not need reciprocal: a conjugate suffice
	quat reciprocal = quat_conjugate(q);
	vec3 product_axis_c = {q.y, -q.x, q.w};
	return cross_product(product_axis_c, reciprocal.xyz)
	     + product_axis_c * reciprocal.w
	     - reciprocal.xyz * q.z;
};

// @Todo: figure the math out
// https://github.com/EpicGames/UnrealEngine/blob/release/Engine/Source/Runtime/Core/Private/Math/UnrealMath.cpp
// https://github.com/g-truc/glm/blob/master/glm/gtc/quaternion.inl
// https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
// https://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/
inline constexpr bool quat_is_singularity(quat const & q) {
	if (absolute(q.y * q.z + q.x * q.w) * 2 <= epsilon) {
		if (absolute((q.z * q.z + q.w * q.w) - (q.x * q.x + q.y * q.y)) <= epsilon) {
			return true;
		}
		if (absolute(1 - (q.x * q.x + q.y * q.y) * 2) <= epsilon) {
			return true;
		}
	}
	if (absolute(q.x * q.y + q.z * q.w) * 2 <= epsilon) {
		if (absolute((q.x * q.x + q.w * q.w) - (q.y * q.y + q.z * q.z)) <= epsilon) {
			return true;
		}
		if (absolute(1 - (q.y * q.y + q.z * q.z) * 2) <= epsilon) {
			return true;
		}
	}
	if (1 - absolute(q.y * q.w - q.x * q.z) * 2 <= epsilon) {
		return true;
	}
	return false;
}

inline constexpr r32 clamp(r32 value, r32 low, r32 high);
inline vec3 quat_get_radians(quat const & q) {
	r32 const w2_m_y2 = q.w * q.w - q.y * q.y;
	r32 const z2_m_x2 = q.z * q.z - q.x * q.x;
	return {
		atan2f((q.y * q.z + q.x * q.w) * 2, w2_m_y2 + z2_m_x2),
		asinf(clamp((q.y * q.w - q.x * q.z) * 2, -1.0f, 1.0f)),
		atan2f((q.x * q.y + q.z * q.w) * 2, w2_m_y2 - z2_m_x2),
	};
}

inline r32 quat_get_radians_x(quat const & q) {
	// if (axis_sin == 0 && axis_cos == 0) { return 2 * atan2f(q.x, q.w); }
	// if (absolute(q.y * q.w - q.x * q.z) >= 1) { return 2 * atan2f(q.x, q.w); }
	// @Note: alternatively `1 - 2 * (q.x * q.x + q.y * q.y)` as the `cos` part?
	return atan2f((q.y * q.z + q.x * q.w) * 2, (q.z * q.z + q.w * q.w) - (q.x * q.x + q.y * q.y));
};

inline r32 quat_get_radians_y(quat q) {
	return asinf(clamp((q.y * q.w - q.x * q.z) * 2, -1.0f, 1.0f));
};

inline r32 quat_get_radians_z(quat const & q) {
	// if (axis_sin == 0 && axis_cos == 0) { return 0; }
	// if (absolute(q.y * q.w - q.x * q.z) >= 1) { return 0; }
	// @Note: alternatively `1 - 2 * (q.y * q.y + q.z * q.z)` as the `cos` part?
	return atan2f((q.x * q.y + q.z * q.w) * 2, (q.x * q.x + q.w * q.w) - (q.y * q.y + q.z * q.z));
};

/*
matrices

> what is majorness?
  given an array [0, 1, 2, 3], or an array of arrays [[0, 1], [2, 3]], where each two values are a vector
  row-major would be read as [row, row] and written in a matrix form as
    |0 1|
    |2 3|
  column-major would be read as [column, column] and written in a matrix form as
    |0 2|
    |1 3|
  N.B.: majorness says nothing about whether rows or columns contain orientation and translation! nada!

> what does a `mat4` type consist of?
  an array of vectors [X, Y, Z, W]; neither rows, nor columns
  XYZ components are intended to store orientation and scaling axes of a transformation matrix
  W is intended to represent a position vector

> interoperation with the graphics APIs
  1) read the spec; there should be descriptions of matrix constructors
  2) GLSL states that it has a column-major ordering: on load [X, Y, Z, W] are treated as columns;
     in case those vectors are orientation axes and a translation offset, then the following formula works
     `result == camera_projection * camera_inverse_transform * object_transform * vector`;
     otherwise, if those vectors are a transposition of orientation and translation vectors, then that is you tool
     `result == vector * object_transform * camera_inverse_transform * camera_projection`

* https://en.wikipedia.org/wiki/Matrix_multiplication
* https://en.wikipedia.org/wiki/Transpose
* https://en.wikipedia.org/wiki/Matrix_representation
* https://en.wikipedia.org/wiki/Row-_and_column-major_order

> general math rules
  transpose(A * B) == transpose(B) * transpose(A)
  result_x_y == dot_product(row_a_y, column_b_x)

> math proof of transformation matrices (using abstract vectors, irrelevant to the `mat4` type)
  ```
  ax - orientation axis_X;   [ax.x ax.y ax.z 0] // `0` somewhat means that's a *direction vector*
  ay - orientation axis_Y;   [ay.x ay.y ay.z 0]
  az - orientation axis_Z;   [az.x az.y az.z 0]
  o  - translation offset;   [o.x  o.y  o.z  1] // `1` somewhat means that's a *position vector*
  p  - a point to transform; [p.x  p.y  p.z  1]
  ```
  
  what are coordinates of a point:
  - `x` component is a distance along an axis_X
  - `y` component is a distance along an axis_Y
  - `z` component is a distance along an axis_Z
  
  then:
  ```
  point_transformed = offset + axis_X * point.x + axis_Y * point.y + axis_Z * point.z
  ``
  
  alternatively, per-component:
  ```
  point_transformed.x = o.x + ax.x * p.x + ay.x * p.y + az.x * p.z
  point_transformed.y = o.y + ax.y * p.x + ay.y * p.y + az.y * p.z
  point_transformed.z = o.z + ax.z * p.x + ay.z * p.y + az.z * p.z
  point_transformed.w = 1
  ```
  
  which is exactly the same result if you do matrix multiplication
  [between matrix rows and a column vector]
  ```
  |ax.x ay.x az.x o.x|    |p.x|
  |ax.y ay.y az.y o.y| \/ |p.y|
  |ax.z ay.z az.z o.z| /\ |p.z|
  |0    0    0    1  |    |1  |
  ```
  
  alternatively, minding the transposition rule
  [between a row vector and matrix columns]
  ```
                     |ax.x ax.y ax.z 0|
  |p.x p.y p.z 1| \/ |ay.x ay.y ay.z 0|
                  /\ |az.x az.y az.z 0|
                     |o.x  o.y  o.z  1|
  ```
*/

//
// mat2 routines
//

constexpr inline mat2 mat_transpose(mat2 const & mat) {
	return {
		vec2{mat.x.x, mat.y.x},
		vec2{mat.x.y, mat.y.y}
	};
}

/*
This code is a result of expanding the following expression

return mat.x * value.x
     + mat.y * value.y
*/
constexpr inline vec2 mat_transform(mat2 const & mat, vec2 const & value) {
	return {
		dot_product({mat.x.x, mat.y.x}, value),
		dot_product({mat.x.y, mat.y.y}, value),
	};
}

constexpr inline mat2 mat_transform(mat2 const & mat, mat2 const & value) {
	return {
		mat_transform(mat, value.x),
		mat_transform(mat, value.y),
	};
}

constexpr inline mat2 interpolate(mat2 const & first, mat2 const & second, r32 fraction) {
	return {
		interpolate(first.x, second.x, fraction),
		interpolate(first.y, second.y, fraction),
	};
}

//
// mat3 routines
//

constexpr inline mat3 mat_transpose(mat3 const & mat) {
	return {
		vec3{mat.x.x, mat.y.x, mat.z.x},
		vec3{mat.x.y, mat.y.y, mat.z.y},
		vec3{mat.x.z, mat.y.z, mat.z.z}
	};
}

/*
This code is a result of expanding the following expression

return mat.x * value.x
     + mat.y * value.y
     + mat.z * value.z
*/
constexpr inline vec3 mat_transform(mat3 const & mat, vec3 const & value) {
	return {
		dot_product({mat.x.x, mat.y.x, mat.z.x}, value),
		dot_product({mat.x.y, mat.y.y, mat.z.y}, value),
		dot_product({mat.x.z, mat.y.z, mat.z.z}, value),
	};
}

constexpr inline mat3 mat_transform(mat3 const & mat, mat3 const & value) {
	return {
		mat_transform(mat, value.x),
		mat_transform(mat, value.y),
		mat_transform(mat, value.z),
	};
}

constexpr inline mat3 interpolate(mat3 const & first, mat3 const & second, r32 fraction) {
	return {
		interpolate(first.x, second.x, fraction),
		interpolate(first.y, second.y, fraction),
		interpolate(first.z, second.z, fraction),
	};
}

constexpr inline mat3 mat_from_position_scale(vec2 const & p, vec2 const & s) {
	return {
		vec3{s.x, 0,   0},
		vec3{0,   s.y, 0},
		vec3{p.x, p.y, 1}
	};
}

inline mat3 mat_from_transformation(vec2 const & position, vec2 const & scale, complex const & rotation) {
	mat3 mat; // = {};
	complex_get_axes(rotation, mat.x.xy, mat.y.xy);
	mat.x.xy *= scale.x; mat.x.z = 0;
	mat.y.xy *= scale.y; mat.y.z = 0;
	mat.z = {position, 1};
	return mat;
}

constexpr inline mat3 mat_inverse_transform(mat3 const & mat) {
	return {
		vec3{mat.x.x, mat.y.x, 0},
		vec3{mat.x.y, mat.y.y, 0},
		vec3{
			-dot_product(mat.z.xy, mat.x.xy),
			-dot_product(mat.z.xy, mat.y.xy),
			1
		}
	};
}

//
// mat4 routines
//

constexpr inline mat4 mat_transpose(mat4 const & mat) {
	return {
		vec4{mat.x.x, mat.y.x, mat.z.x, mat.w.x},
		vec4{mat.x.y, mat.y.y, mat.z.y, mat.w.y},
		vec4{mat.x.z, mat.y.z, mat.z.z, mat.w.z},
		vec4{mat.x.w, mat.y.w, mat.z.w, mat.w.w},
	};
}

/*
This code is a result of expanding the following expression

return mat.x * value.x
     + mat.y * value.y
     + mat.z * value.z
     + mat.w * value.w
*/
constexpr inline vec4 mat_transform(mat4 const & mat, vec4 const & value) {
	return {
		dot_product({mat.x.x, mat.y.x, mat.z.x, mat.w.x}, value),
		dot_product({mat.x.y, mat.y.y, mat.z.y, mat.w.y}, value),
		dot_product({mat.x.z, mat.y.z, mat.z.z, mat.w.z}, value),
		dot_product({mat.x.w, mat.y.w, mat.z.w, mat.w.w}, value),
	};
}

constexpr inline mat4 mat_transform(mat4 const & mat, mat4 const & value) {
	return {
		mat_transform(mat, value.x),
		mat_transform(mat, value.y),
		mat_transform(mat, value.z),
		mat_transform(mat, value.w),
	};
}

constexpr inline mat4 interpolate(mat4 const & first, mat4 const & second, r32 fraction) {
	return {
		interpolate(first.x, second.x, fraction),
		interpolate(first.y, second.y, fraction),
		interpolate(first.z, second.z, fraction),
		interpolate(first.w, second.w, fraction),
	};
}

constexpr inline mat4 mat_from_position_scale(vec3 const & p, vec3 const & s) {
	return {
		vec4{s.x, 0,   0,   0},
		vec4{0,   s.y, 0,   0},
		vec4{0,   0,   s.z, 0},
		vec4{p.x, p.y, p.z, 1}
	};
}

inline mat4 mat_from_transformation(vec3 const & position, vec3 const & scale, quat const & rotation) {
	mat4 mat; // = {};
	quat_get_axes(rotation, mat.x.xyz, mat.y.xyz, mat.z.xyz);
	mat.x.xyz *= scale.x; mat.x.w = 0;
	mat.y.xyz *= scale.y; mat.y.w = 0;
	mat.z.xyz *= scale.z; mat.z.w = 0;
	mat.w = {position, 1};
	return mat;
}

constexpr inline mat4 mat_inverse_transform(mat4 const & mat) {
	return {
		vec4{mat.x.x, mat.y.x, mat.z.x, 0},
		vec4{mat.x.y, mat.y.y, mat.z.y, 0},
		vec4{mat.x.z, mat.y.z, mat.z.z, 0},
		vec4{
			-dot_product(mat.w.xyz, mat.x.xyz),
			-dot_product(mat.w.xyz, mat.y.xyz),
			-dot_product(mat.w.xyz, mat.z.xyz),
			1
		}
	};
}

/*
projection matrices

* https://github.com/g-truc/glm/blob/master/glm/ext/matrix_clip_space.inl
* https://github.com/OneLoneCoder/olcPixelGameEngine/blob/master/Extensions/olcPGEX_Graphics3D.h
* https://www.scratchapixel.com/lessons/3d-basic-rendering/perspective-and-orthographic-projection-matrix
* https://docs.microsoft.com/ru-ru/windows/win32/direct3d9/d3dxmatrixortholh
* https://docs.microsoft.com/ru-ru/windows/win32/direct3d9/d3dxmatrixperspectivelh
* https://docs.microsoft.com/ru-ru/windows/win32/direct3d9/d3dxmatrixperspectivefovlh
* http://ogldev.atspace.co.uk/www/tutorial12/tutorial12.html
* http://learnwebgl.brown37.net/08_projections/projections_ortho.html
* http://learnwebgl.brown37.net/08_projections/projections_perspective.html

Aim is to scale some world-space coordinates into a box of:
- ([-1..1], [-1..1], -[near clipping plane, 1]) - as a general case;
- ([-1..1], [-1..1], -[ 0, 1]) - DirectX, OpenGL _with clip control_ (>= 4.5)
- ([-1..1], [-1..1], -[-1, 1]) - OpenGL _without clip control_ (< 4.5)
- ... *technically* it can be ignored, although it loses you a half of the depth range in the NDC (?)

^^^^ actually coordinates could be scaled whatever way required; it's just the fact
     that everything ouside this clipping box will be discarded;

XY scale is usually represented by:
- 1 / tangent(vield of view radians / 2): a bisected angle scales each of the halves of X and Y planes respectively
- ncp / (XY half size): [in case of a perspective projection] keeps area of the near clipping plane intact, but affects field of view
- any value of choice

Orthograhic projection:
- XYZ' = (a + b * XYZ) / 1
- XY scale; a constant scale from [-bounds .. bounds] to [-1 .. 1]
- Z  scale; a constant scale from [Znear .. Zfar] to [NCP .. 1]
- offset in case of assymetry
- any other optional distortion

> Z [NCP .. 1]
  a + Min * b = NCP
  a + Max * b = 1
  ---> b = (1 - NCP) / (Max - Min)         ---> scale
  ---> a = (Max * NCP - Min) / (Max - Min) ---> offset

> XY
  a + b * Min = -1
  a + b * Max =  1
  ---> b = 2 / (Max - Min)            ---> scale
  ---> a = -(Min + Max) / (Max - Min) ---> offset

Perspective projection:
- XYZ' = (a + b * XYZ) / Z
- XY scale; a linear scale from [-bounds .. bounds] to [-1 .. 1]
- Z  scale; an exponential scale from [Znear .. Zfar] to [NCP .. 1]
- offset in case of assymetry
- any other optional distortion

> Z [NCP .. 1]
  a / Min + b = NCP
  a / Max + b = 1
  ---> b = (Max - NCP * Min) / (Max - Min)     ---> scale
  ---> a = (NCP - 1) * Min * Max / (Max - Min) ---> offset

> XY [-1 .. 1]
  a + b * Min = -Z
  a + b * Max =  Z
  ---> b = 2 * Z / (Max - Min)            ---> scale
  ---> a = -(Min + Max) * Z / (Max - Min) ---> offset

*/

constexpr inline mat4 mat_persp(vec2 const & scale, r32 ncp, r32 fcp) {
	constexpr float const NCP = 0;
	float const reverse_depth = 1 / (fcp - ncp);
	float const scale_z = isinf(fcp) ? 1                 : ((fcp - NCP * ncp) * reverse_depth);
	float const offset  = isinf(fcp) ? ((NCP - 1) * ncp) : ((NCP - 1) * ncp * fcp * reverse_depth);
	return {
		vec4{scale.x, 0, 0, 0}, // W += 0 * vec4.x
		vec4{0, scale.y, 0, 0}, // W += 0 * vec4.y
		vec4{0, 0, scale_z, 1}, // W += 1 * vec4.z
		vec4{0, 0, offset,  0}, // W += 0 * vec4.w
	};
}

constexpr inline mat4 mat_ortho(vec2 const & scale, r32 ncp, r32 fcp) {
	constexpr float const NCP = 0;
	float const reverse_depth = 1 / (fcp - ncp);
	float const scale_z = isinf(fcp) ? 0 : ((1 - NCP) * reverse_depth);
	float const offset  = isinf(fcp) ? 0 : ((fcp * NCP - ncp) * reverse_depth);
	return {
		vec4{scale.x, 0, 0, 0}, // W += 0 * vec4.x
		vec4{0, scale.y, 0, 0}, // W += 0 * vec4.y
		vec4{0, 0, scale_z, 0}, // W += 0 * vec4.z
		vec4{0, 0, offset,  1}, // W += 1 * vec4.w
	};
}

//
// macro implementations
//

// @Todo: redefine them with templates?

#define CLAMP_IMPL(T)                              \
constexpr inline T clamp(T value, T low, T high) { \
    return min(max(value, low), high);             \
}                                                  \

#define MOVE_TOWARDS_CLAMPED_IMPL(T)                             \
constexpr inline T move_towards_clamped(T from, T to, T delta) { \
    return clamp(from + delta, from, to);                        \
}                                                                \

#define CLAMP_VEC_IMPL(T)                                  \
constexpr inline T clamp(T const & value, T low, T high) { \
    return min(max(value, low), high);                     \
}                                                          \

#define MOVE_TOWARDS_CLAMPED_VEC_IMPL(T)                                 \
constexpr inline T move_towards_clamped(T const & from, T to, T delta) { \
    return clamp(from + delta, from, to);                                \
}                                                                        \

#define VECTOR_MAGNITUDE_SQUARED_IMPL(T, S)             \
constexpr inline S magnitude_squared(T const & value) { \
    return dot_product(value, value);                   \
}\

#define VECTOR_MAGNITUDE_IMPL(T)                  \
inline r32 magnitude(T const & value) {           \
    return square_root(magnitude_squared(value)); \
}                                                 \

#define VECTOR_NORMALIZE_IMPL(T)      \
inline T normalize(T const & value) { \
    return value / magnitude(value);  \
}                                     \

#define VECTOR_REFLECT_IMPL(T)                                                 \
constexpr inline T reflect(T const & incident, T const & normal, r32 factor) { \
    r32 incident_cosine = dot_product(incident, normal);                       \
    r32 normal_factor   = incident_cosine * factor;                            \
    return incident - normal * normal_factor;                                  \
}                                                                              \

#define VECTOR_REFRACT_IMPL(T)                                            \
inline T refract(T const & incident, T const & normal, r32 factor) {      \
    r32 incident_cosine = dot_product(incident, normal);                  \
    r32 incident_sine_squared = 1 - incident_cosine * incident_cosine;    \
    r32 refracted_sine_squared = factor * factor * incident_sine_squared; \
    if (refracted_sine_squared < 1) {                                     \
        r32 refracted_cosine = square_root(1 - refracted_sine_squared);   \
        r32 normal_factor = incident_cosine * factor + refracted_cosine;  \
        return incident * factor - normal * normal_factor;                \
    }                                                                     \
    return {};                                                            \
}                                                                         \

#define VECTOR_MOVE_TOWARDS_CLAMPED_IMPL(T)                              \
inline T move_towards_clamped(T const & from, T const & to, r32 delta) { \
    T direction = to - from;                                             \
    r32 distance = magnitude(direction);                                 \
    if (delta <= 0) { return from; }                                     \
    if (delta >= distance) { return to; }                                \
    return from + (direction / distance) * delta;                        \
}                                                                        \

#define VECTOR_CLAMP_MAGNITUDE_IMPL(T)                         \
inline T clamp_magnitude(T const & value, r32 low, r32 high) { \
    r32 length = magnitude(value);                             \
    if (length < low) { return (value / length) * low; }       \
    if (length > high) { return (value / length) * high; }     \
    return value;                                              \
}                                                              \

CLAMP_IMPL(s32)
CLAMP_VEC_IMPL(ivec2)
CLAMP_VEC_IMPL(ivec3)
CLAMP_VEC_IMPL(ivec4)

CLAMP_IMPL(r32)
CLAMP_VEC_IMPL(vec2)
CLAMP_VEC_IMPL(vec3)
CLAMP_VEC_IMPL(vec4)

MOVE_TOWARDS_CLAMPED_IMPL(s32);
MOVE_TOWARDS_CLAMPED_VEC_IMPL(ivec2)
MOVE_TOWARDS_CLAMPED_VEC_IMPL(ivec3)
MOVE_TOWARDS_CLAMPED_VEC_IMPL(ivec4)

MOVE_TOWARDS_CLAMPED_IMPL(r32);
MOVE_TOWARDS_CLAMPED_VEC_IMPL(vec2)
MOVE_TOWARDS_CLAMPED_VEC_IMPL(vec3)
MOVE_TOWARDS_CLAMPED_VEC_IMPL(vec4)

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
