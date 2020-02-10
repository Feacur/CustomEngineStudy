#pragma once
#include "engine/core/math_types.h"
#include "engine/impl/math_scalar.h"

//
//
//

template<typename T>
constexpr inline T cross_product(xvec2<T> first, xvec2<T> second) {
	return first.x * second.y - first.y * second.x;
}

template<typename T>
constexpr inline xvec3<T> cross_product(xvec3<T> first, xvec3<T> second) {
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
constexpr inline bool operator==(xvec2<T> first, xvec2<T> second) {
	return (first.x == second.x)
	    && (first.y == second.y);
}

template<typename T>
constexpr inline bool operator!=(xvec2<T> first, xvec2<T> second) {
	return (first.x != second.x)
	    || (first.y != second.y);
}

template<typename T>
constexpr inline xvec2<T> operator+(xvec2<T> first, xvec2<T> second) {
	return {
		first.x + second.x,
		first.y + second.y
	};
}

template<typename T>
constexpr inline xvec2<T> operator+(xvec2<T> first, T second) {
	return {
		first.x + second,
		first.y + second
	};
}

template<typename T>
constexpr inline xvec2<T> operator-(xvec2<T> first, xvec2<T> second) {
	return {
		first.x - second.x,
		first.y - second.y
	};
}

template<typename T>
constexpr inline xvec2<T> operator-(xvec2<T> first, T second) {
	return {
		first.x - second,
		first.y - second
	};
}

template<typename T>
constexpr inline xvec2<T> operator-(xvec2<T> value) {
	return {
		-value.x,
		-value.y
	};
}

template<typename T>
constexpr inline xvec2<T> operator*(xvec2<T> first, T second) {
	return {
		first.x * second,
		first.y * second
	};
}

template<typename T>
constexpr inline xvec2<T> operator*(xvec2<T> first, xvec2<T> second) {
	return {
		first.x * second.x,
		first.y * second.y
	};
}

template<typename T>
constexpr inline xvec2<T> operator/(xvec2<T> first, T second) {
	return {
		first.x / second,
		first.y / second
	};
}

template<typename T>
constexpr inline xvec2<T> operator/(xvec2<T> first, xvec2<T> second) {
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

template<typename T>
constexpr inline T dot_product(xvec2<T> first, xvec2<T> second) {
	return first.x * second.x
	     + first.y * second.y;
}

template<typename T>
constexpr inline xvec2<T> min(xvec2<T> first, xvec2<T> second) {
	return {
		min(first.x, second.x),
		min(first.y, second.y)
	};
}

template<typename T>
constexpr inline xvec2<T> max(xvec2<T> first, xvec2<T> second) {
	return {
		max(first.x, second.x),
		max(first.y, second.y)
	};
}

template<typename T>
constexpr inline T min(xvec2<T> value) {
	return min(value.x, value.y);
}

template<typename T>
constexpr inline T max(xvec2<T> value) {
	return max(value.x, value.y);
}

template<typename T>
constexpr inline xvec2<T> absolute(xvec2<T> value) {
	return {
		absolute(value.x),
		absolute(value.y)
	};
}

template<typename T>
constexpr inline xvec2<T> sign(xvec2<T> value) {
	return {
		sign(value.x),
		sign(value.y)
	};
}

template<typename T>
constexpr inline xvec2<T> interpolate(xvec2<T> from, xvec2<T> to, T fraction) {
	return {
		interpolate(from.x, to.x, fraction),
		interpolate(from.y, to.y, fraction)
	};
}

template<typename T>
inline xvec2<T> square_root(xvec2<T> value) {
	return {
		square_root(value.x),
		square_root(value.y)
	};
}

template<typename T>
inline xvec2<T> sine(xvec2<T> value) {
	return {
		sine(value.x),
		sine(value.y)
	};
}

template<typename T>
inline xvec2<T> cosine(xvec2<T> value) {
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
constexpr inline bool operator==(xvec3<T> first, xvec3<T> second) {
	return (first.x == second.x)
	    && (first.y == second.y)
	    && (first.z == second.z);
}

template<typename T>
constexpr inline bool operator!=(xvec3<T> first, xvec3<T> second) {
	return (first.x != second.x)
	    || (first.y != second.y)
	    || (first.z != second.z);
}

template<typename T>
constexpr inline xvec3<T> operator+(xvec3<T> first, xvec3<T> second) {
	return {
		first.x + second.x,
		first.y + second.y,
		first.z + second.z
	};
}

template<typename T>
constexpr inline xvec3<T> operator+(xvec3<T> first, T second) {
	return {
		first.x + second,
		first.y + second,
		first.z + second
	};
}

template<typename T>
constexpr inline xvec3<T> operator-(xvec3<T> first, xvec3<T> second) {
	return {
		first.x - second.x,
		first.y - second.y,
		first.z - second.z
	};
}

template<typename T>
constexpr inline xvec3<T> operator-(xvec3<T> first, T second) {
	return {
		first.x - second,
		first.y - second,
		first.z - second
	};
}

template<typename T>
constexpr inline xvec3<T> operator-(xvec3<T> value) {
	return {
		-value.x,
		-value.y,
		-value.z
	};
}

template<typename T>
constexpr inline xvec3<T> operator*(xvec3<T> first, T second) {
	return {
		first.x * second,
		first.y * second,
		first.z * second
	};
}

template<typename T>
constexpr inline xvec3<T> operator*(xvec3<T> first, xvec3<T> second) {
	return {
		first.x * second.x,
		first.y * second.y,
		first.z * second.z
	};
}

template<typename T>
constexpr inline xvec3<T> operator/(xvec3<T> first, T second) {
	return {
		first.x / second,
		first.y / second,
		first.z / second
	};
}

template<typename T>
constexpr inline xvec3<T> operator/(xvec3<T> first, xvec3<T> second) {
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

template<typename T>
constexpr inline T dot_product(xvec3<T> first, xvec3<T> second) {
	return first.x * second.x
	     + first.y * second.y
	     + first.z * second.z;
}

template<typename T>
constexpr inline xvec3<T> min(xvec3<T> first, xvec3<T> second) {
	return {
		min(first.x, second.x),
		min(first.y, second.y),
		min(first.z, second.z)
	};
}

template<typename T>
constexpr inline xvec3<T> max(xvec3<T> first, xvec3<T> second) {
	return {
		max(first.x, second.x),
		max(first.y, second.y),
		max(first.z, second.z)
	};
}

template<typename T>
constexpr inline T min(xvec3<T> value) {
	return min(min(value.x, value.y), value.z);
}

template<typename T>
constexpr inline T max(xvec3<T> value) {
	return max(max(value.x, value.y), value.z);
}

template<typename T>
constexpr inline xvec3<T> absolute(xvec3<T> value) {
	return {
		absolute(value.x),
		absolute(value.y),
		absolute(value.z)
	};
}

template<typename T>
constexpr inline xvec3<T> sign(xvec3<T> value) {
	return {
		sign(value.x),
		sign(value.y),
		sign(value.z)
	};
}

template<typename T>
constexpr inline xvec3<T> interpolate(xvec3<T> from, xvec3<T> to, T fraction) {
	return {
		interpolate(from.x, to.x, fraction),
		interpolate(from.y, to.y, fraction),
		interpolate(from.z, to.z, fraction)
	};
}

template<typename T>
inline xvec3<T> square_root(xvec3<T> value) {
	return {
		square_root(value.x),
		square_root(value.y),
		square_root(value.z)
	};
}

template<typename T>
inline xvec3<T> sine(xvec3<T> value) {
	return {
		sine(value.x),
		sine(value.y),
		sine(value.z)
	};
}

template<typename T>
inline xvec3<T> cosine(xvec3<T> value) {
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
constexpr inline bool operator==(xvec4<T> first, xvec4<T> second) {
	return (first.x == second.x)
	    && (first.y == second.y)
	    && (first.z == second.z)
	    && (first.w == second.w);
}

template<typename T>
constexpr inline bool operator!=(xvec4<T> first, xvec4<T> second) {
	return (first.x != second.x)
	    || (first.y != second.y)
	    || (first.z != second.z)
	    || (first.w != second.w);
}

template<typename T>
constexpr inline xvec4<T> operator+(xvec4<T> first, xvec4<T> second) {
	return {
		first.x + second.x,
		first.y + second.y,
		first.z + second.z,
		first.w + second.w
	};
}

template<typename T>
constexpr inline xvec4<T> operator+(xvec4<T> first, T second) {
	return {
		first.x + second,
		first.y + second,
		first.z + second,
		first.w + second
	};
}

template<typename T>
constexpr inline xvec4<T> operator-(xvec4<T> first, xvec4<T> second) {
	return {
		first.x - second.x,
		first.y - second.y,
		first.z - second.z,
		first.w - second.w
	};
}

template<typename T>
constexpr inline xvec4<T> operator-(xvec4<T> first, T second) {
	return {
		first.x - second,
		first.y - second,
		first.z - second,
		first.w - second
	};
}

template<typename T>
constexpr inline xvec4<T> operator-(xvec4<T> value) {
	return {
		-value.x,
		-value.y,
		-value.z,
		-value.w
	};
}

template<typename T>
constexpr inline xvec4<T> operator*(xvec4<T> first, T second) {
	return {
		first.x * second,
		first.y * second,
		first.z * second,
		first.w * second
	};
}

template<typename T>
constexpr inline xvec4<T> operator*(xvec4<T> first, xvec4<T> second) {
	return {
		first.x * second.x,
		first.y * second.y,
		first.z * second.z,
		first.w * second.w
	};
}

template<typename T>
constexpr inline xvec4<T> operator/(xvec4<T> first, T second) {
	return {
		first.x / second,
		first.y / second,
		first.z / second,
		first.w / second
	};
}

template<typename T>
constexpr inline xvec4<T> operator/(xvec4<T> first, xvec4<T> second) {
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
constexpr inline T dot_product(xvec4<T> first, xvec4<T> second) {
	return first.x * second.x
	     + first.y * second.y
	     + first.z * second.z
	     + first.w * second.w;
}

template<typename T>
constexpr inline xvec4<T> min(xvec4<T> first, xvec4<T> second) {
	return {
		min(first.x, second.x),
		min(first.y, second.y),
		min(first.z, second.z),
		min(first.w, second.w)
	};
}

template<typename T>
constexpr inline xvec4<T> max(xvec4<T> first, xvec4<T> second) {
	return {
		max(first.x, second.x),
		max(first.y, second.y),
		max(first.z, second.z),
		max(first.w, second.w)
	};
}

template<typename T>
constexpr inline T min(xvec4<T> value) {
	return min(min(min(value.x, value.y), value.z), value.w);
}

template<typename T>
constexpr inline T max(xvec4<T> value) {
	return max(max(max(value.x, value.y), value.z), value.w);
}

template<typename T>
constexpr inline xvec4<T> absolute(xvec4<T> value) {
	return {
		absolute(value.x),
		absolute(value.y),
		absolute(value.z),
		absolute(value.w)
	};
}

template<typename T>
constexpr inline xvec4<T> sign(xvec4<T> value) {
	return {
		sign(value.x),
		sign(value.y),
		sign(value.z),
		sign(value.w)
	};
}

template<typename T>
constexpr inline xvec4<T> interpolate(xvec4<T> from, xvec4<T> to, T fraction) {
	return {
		interpolate(from.x, to.x, fraction),
		interpolate(from.y, to.y, fraction),
		interpolate(from.z, to.z, fraction),
		interpolate(from.w, to.w, fraction)
	};
}

template<typename T>
inline xvec4<T> square_root(xvec4<T> value) {
	return {
		square_root(value.x),
		square_root(value.y),
		square_root(value.z),
		square_root(value.w)
	};
}

template<typename T>
inline xvec4<T> sine(xvec4<T> value) {
	return {
		sine(value.x),
		sine(value.y),
		sine(value.z),
		sine(value.w)
	};
}

template<typename T>
inline xvec4<T> cosine(xvec4<T> value) {
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

constexpr inline icomplex complex_reciprocal(icomplex value) {
	return {value.x, -value.y};
}

constexpr inline icomplex complex_multiply(icomplex first, icomplex second) {
	return {
		first.x * second.x - first.y * second.y,
		first.x * second.y + first.y * second.x
	};
}

constexpr inline icomplex complex_rotate_vector(icomplex c, ivec2 vector) {
	return complex_multiply(c, vector);
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

constexpr inline complex complex_rotate_vector(complex c, vec2 vector) {
	return complex_multiply(c, vector);
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

constexpr inline quat quat_product(quat first, quat second) {
	return {
		cross_product(first.xyz, second.xyz) + first.xyz * second.w + second.xyz * first.w,
		first.w * second.w - dot_product(first.xyz, second.xyz)
	};
}

/*
This code is a result of expanding following expression,
excluding stuff negated by multiplication with zero

return quat_product(
	quat_product(
		quat_from_axis({0, 1, 0}, euler_radians.y)
		quat_from_axis({1, 0, 0}, euler_radians.x)
	),
	quat_from_axis({0, 0, 1}, euler_radians.z),
);
*/
inline quat quat_from_radians(vec3 euler_radians) {
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
This code is a result of expanding following expression,
excluding stuff negated by multiplication with zero

return quat_product(
	quat_product(quat, {vector, 0}),
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
// mat2 routines
//

constexpr inline mat2 mat_transpose(mat2 value) {
	return {
		vec2{value.x.x, value.y.x},
		vec2{value.x.y, value.y.y}
	};
}

constexpr inline vec2 mat_product(mat2 mat, vec2 v) {
	return mat.x * v.x
	     + mat.y * v.y;
}

constexpr inline mat2 mat_product(mat2 first, mat2 second) {
	return {
		mat_product(first, second.x),
		mat_product(first, second.y)
	};
}

//
// mat3 routines
//

constexpr inline mat3 mat_transpose(mat3 value) {
	return {
		vec3{value.x.x, value.y.x, value.z.x},
		vec3{value.x.y, value.y.y, value.z.y},
		vec3{value.x.z, value.y.z, value.z.z}
	};
}

constexpr inline vec3 mat_product(mat3 mat, vec3 v) {
	return mat.x * v.x
	     + mat.y * v.y
	     + mat.z * v.z;
}

constexpr inline mat3 mat_product(mat3 first, mat3 second) {
	return {
		mat_product(first, second.x),
		mat_product(first, second.y),
		mat_product(first, second.z)
	};
}

constexpr inline mat3 mat_position_scale(vec2 p, vec2 s) {
	return {
		vec3{s.x, 0,   0},
		vec3{0,   s.y, 0},
		vec3{p.x, p.y, 1}
	};
}

//
// mat4 routines
//

constexpr inline mat4 mat_transpose(mat4 value) {
	return {
		vec4{value.x.x, value.y.x, value.z.x, value.w.x},
		vec4{value.x.y, value.y.y, value.z.y, value.w.y},
		vec4{value.x.z, value.y.z, value.z.z, value.w.z},
		vec4{value.x.w, value.y.w, value.z.w, value.w.w},
	};
}

constexpr inline vec4 mat_product(mat4 mat, vec4 v) {
	return mat.x * v.x
	     + mat.y * v.y
	     + mat.z * v.z
	     + mat.w * v.w;
}

constexpr inline mat4 mat_product(mat4 first, mat4 second) {
	return {
		mat_product(first, second.x),
		mat_product(first, second.y),
		mat_product(first, second.z),
		mat_product(first, second.w)
	};
}

constexpr inline mat4 mat_position_scale(vec3 p, vec3 s) {
	return {
		vec4{s.x, 0,   0,   0},
		vec4{0,   s.y, 0,   0},
		vec4{0,   0,   s.z, 0},
		vec4{p.x, p.y, p.z, 1}
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
* https://developer.nvidia.com/content/depth-precision-visualized

Aim is to scale some world-space coordinates into a box of:
- ([-1..1], [-1..1], -[near clipping plane, 1]) - as a general case;
- ([-1..1], [-1..1], -[ 0, 1]) - DirectX, OpenGL _with clip control_ (>= 4.5)
- ([-1..1], [-1..1], -[-1, 1]) - OpenGL _without clip control_ (< 4.5); *technically*

^^^^ actually coordinates could be scaled whatever way required; it's just the fact
     that everything ouside this clipping box will be discarded, at least only this volume
     will be projected onto the screen inside its bounds

XY scale is usually represented by:
- 1 / tangent(vield of view radians / 2): a bisected angle scales each of the halves of X and Y planes respectively
- near / (XY half size): [in case of a perspective projection] keeps area of the near clipping plane intact, but affects field of view
- any value of choice

Orthograhic projection:
- XYZ' = (a + b * XYZ) / 1
- XY scale; a constant scale from [-bounds .. bounds] to [-1 .. 1]
- Z  scale; a constant scale from [Znear .. Zfar] to [near clipping plane .. 1]
- offset in case of assymetry
- any other optional distortion
> Z
  near clipping plane = a + b * Znear = 0
  far  clipping place = a + b * Zfar  = 1
  ---> b = 1 / (Zfar - Znear)      ---> scale
  ---> a = -Znear / (Zfar - Znear) ---> offset
> XY
  s1 + s2 * left = -1
  s1 + s2  * right = 1
  ---> s2 = 2 / (right - left)               ---> scale
  ---> s1 = -(left + right) / (right - left) ---> offset

Perspective projection:
- XYZ' = (a + b * XYZ) / Z
- XY scale; a linear scale from [-bounds .. bounds] to [-1 .. 1]
- Z  scale; an exponential scale from [Znear .. Zfar] to [near clipping plane .. 1]
- offset in case of assymetry
- any other optional distortion
> Z
  near clipping plane = a / Znear + b = 0
  far  clipping place = a / Zfar  + b = 1
  ---> b = Zfar / (Zfar - Znear)          ---> scale
  ---> a = -Znear * Zfar / (Zfar - Znear) ---> offset
> XY
  s1 + s2 * left = -Z
  s1 + s2 * right = Z
  ---> s2 = 2 * Z / (right - left)               ---> scale
  ---> s1 = -(left + right) * Z / (right - left) ---> offset

*/

constexpr inline mat4 mat_persp(vec2 scale, r32 near, r32 far) {
	// @Note: left-handed, XY symmetric, zero to one
	r32 const z_scale = far / (far - near); // -- diff, b value
	mat4 result = {};
	result[0][0] = scale.x;
	result[1][1] = scale.y;
	result[2][2] = z_scale;
	result[2][3] = 1; // -- diff: normalize XY by Z value; W += 1 * vec4.z
	result[3][2] = -z_scale * near; // offset by N to [0..1]
	result[3][3] = 0; // -- diff: W += 0 * vec4.w
	return result;
}

constexpr inline mat4 mat_persp(vec3 min, vec3 max) {
	// @Note: left-handed, zero to one
	vec3 const scale = vec3{min.xy * (min.z * 2), max.z} / (max - min);
	mat4 result = {};
	result[0][0] = scale.x;
	result[1][1] = scale.y;
	result[2][2] = scale.z;
	result[2][3] = 0; // -- diff: normalize XY by Z value; W += 1 * vec4.z
	result[3].xy = -scale.xy * (min.xy + max.xy) / (r32)2;
	result[3].z  = -scale.z * min.z;
	result[3][3] = 1; // -- diff: W += 0 * vec4.w
	return result;
}

constexpr inline mat4 mat_ortho(vec2 scale, r32 near, r32 far) {
	// @Note: left-handed, XY symmetric, zero to one
	r32 const z_scale = 1 / (far - near); // -- diff, b value
	mat4 result = {};
	result[0][0] = scale.x;
	result[1][1] = scale.y;
	result[2][2] = z_scale;
	result[2][3] = 0; // -- diff: do not normalize XY by Z; W += 0 * vec4.z
	result[3][2] = -z_scale * near;
	result[3][3] = 1; // -- diff: W += 1 * vec4.w
	return result;
}

constexpr inline mat4 mat_ortho(vec3 min, vec3 max) {
	// @Note: left-handed, zero to one
	vec3 const scale = vec3{2, 2, 1} / (max - min);
	mat4 result = {};
	result[0][0] = scale.x;
	result[1][1] = scale.y;
	result[2][2] = scale.z;
	result[2][3] = 0; // -- diff: do not normalize XY by Z; W += 0 * vec4.z
	result[3].xy = -scale.xy * (min.xy + max.xy) / (r32)2;
	result[3].z  = -scale.z * min.z;
	result[3][3] = 1; // -- diff: W += 1 * vec4.w
	return result;
}

//
// macro implementations
//

// @Todo: redefine them with templates?

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
