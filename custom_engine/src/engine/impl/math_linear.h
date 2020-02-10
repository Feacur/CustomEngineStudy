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

mat2 mat_transpose(mat2 m) {
	return {
		vec2{m.x.x, m.y.x},
		vec2{m.x.y, m.y.y}
	};
}

mat2 mat_product(mat2 m1, mat2 m2) {
	return {
		m1.x * m2.x.x + m1.y * m2.x.y,
		m1.x * m2.y.x + m1.y * m2.y.y,
	};
}

//
// mat3 routines
//

mat3 mat_transpose(mat3 m) {
	return {
		vec3{m.x.x, m.y.x, m.z.x},
		vec3{m.x.y, m.y.y, m.z.y},
		vec3{m.x.z, m.y.z, m.z.z}
	};
}

mat3 mat_product(mat3 m1, mat3 m2) {
	return {
		m1.x * m2.x.x + m1.y * m2.x.y + m1.z * m2.x.z,
		m1.x * m2.y.x + m1.y * m2.y.y + m1.z * m2.y.z,
		m1.x * m2.z.x + m1.y * m2.z.y + m1.z * m2.z.z,
	};
}

mat3 mat_position_scale(vec2 p, vec2 s) {
	return {
		vec3{s.x, 0,   0},
		vec3{0,   s.y, 0},
		vec3{p.x, p.y, 1}
	};
}

//
// mat4 routines
//

mat4 mat_transpose(mat4 m) {
	return {
		vec4{m.x.x, m.y.x, m.z.x, m.w.x},
		vec4{m.x.y, m.y.y, m.z.y, m.w.y},
		vec4{m.x.z, m.y.z, m.z.z, m.w.z},
		vec4{m.x.w, m.y.w, m.z.w, m.w.w},
	};
}

mat4 mat_product(mat4 m1, mat4 m2) {
	return {
		m1.x * m2.x.x + m1.y * m2.x.y + m1.z * m2.x.z + m1.w * m2.x.w,
		m1.x * m2.y.x + m1.y * m2.y.y + m1.z * m2.y.z + m1.w * m2.y.w,
		m1.x * m2.z.x + m1.y * m2.z.y + m1.z * m2.z.z + m1.w * m2.z.w,
		m1.x * m2.w.x + m1.y * m2.w.y + m1.z * m2.w.z + m1.w * m2.w.w
	};
}

mat4 mat_position_scale(vec3 p, vec3 s) {
	return {
		vec4{s.x, 0,   0,   0},
		vec4{0,   s.y, 0,   0},
		vec4{0,   0,   s.z, 0},
		vec4{p.x, p.y, p.z, 1}
	};
}

// https://github.com/g-truc/glm/blob/master/glm/ext/matrix_clip_space.inl
// https://github.com/OneLoneCoder/olcPixelGameEngine/blob/master/Extensions/olcPGEX_Graphics3D.h
// https://www.scratchapixel.com/lessons/3d-basic-rendering/perspective-and-orthographic-projection-matrix
// https://docs.microsoft.com/ru-ru/windows/win32/direct3d9/d3dxmatrixortholh
// https://docs.microsoft.com/ru-ru/windows/win32/direct3d9/d3dxmatrixperspectivelh
// https://docs.microsoft.com/ru-ru/windows/win32/direct3d9/d3dxmatrixperspectivefovlh

mat4 mat_persp(vec2 scale, r32 near, r32 far) {
	// @Note: left-handed, symmetric, zero to one
	r32 const z_scale = far / (far - near); // -- diff
	mat4 result = {};
	result[0][0] = scale.x;
	result[1][1] = scale.y;
	result[2][2] = z_scale; // scale to (N + [0..1])
	result[2][3] = 1; // -- diff: normalize XY by Z
	result[3][2] = -near * z_scale; // offset by N to [0..1]
	result[3][3] = 0; // -- diff
	return result;
}

mat4 mat_ortho(vec2 scale, r32 near, r32 far) {
	// @Note: left-handed, symmetric, zero to one
	r32 const z_scale = 1 / (far - near); // -- diff
	mat4 result = {};
	result[0][0] = scale.x;
	result[1][1] = scale.y;
	result[2][2] = z_scale; // scale to (N + [0..1])
	result[2][3] = 0; // -- diff: do not normalize XY by Z
	result[3][2] = -near * z_scale; // offset by N to [0..1]
	result[3][3] = 1; // -- diff
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
