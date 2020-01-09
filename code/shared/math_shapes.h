#define SHARED_MATH_SHAPES

#if !defined(SHARED_MATH_LINEAR)
	#error include "shared/math_linear.h"
#endif

struct Ray2 { Vector2 origin, direction; };
struct Ray3 { Vector3 origin, direction; };

struct Aabb2 { Vector2 position, extents; };
struct Aabb3 { Vector3 position, extents; };

// typedef Vector4 Sphere;
using Sphere = Vector4;

constexpr inline Ray2 ray2(Vector2 origin, Vector2 direction) { return {origin, direction}; }
constexpr inline Ray3 ray3(Vector3 origin, Vector3 direction) { return {origin, direction}; }

constexpr inline Aabb2 aabb2(Vector2 position, Vector2 extents) { return {position, extents}; }
constexpr inline Aabb3 aabb3(Vector3 position, Vector3 extents) { return {position, extents}; }

//
// Ray2 routines
//

constexpr inline Vector2 ray_point(Ray2 ray, float distance) {
	return ray.origin + ray.direction * distance;
}

//
// Ray3 routines
//

constexpr inline Vector3 ray_point(Ray3 ray, float distance) {
	return ray.origin + ray.direction * distance;
}

//
// Aabb2 routines
//

constexpr inline float contains_point(Aabb2 aabb, Vector2 point) {
	Vector2 position = aabb.position - point;
	Vector2 minimum = position - aabb.extents;
	Vector2 maximum = position + aabb.extents;
	return (minimum.x <= 0) && (minimum.y <= 0)
	    && (maximum.x >= 0) && (maximum.y >= 0);
}

constexpr inline bool collide(Aabb2 first, Aabb2 second) {
	Vector2 position = first.position - second.position;
	Vector2 size = first.extents + second.extents;
	return (position.x <=  size.x) && (position.y <=  size.y)
	    && (position.x >= -size.x) && (position.y >= -size.x);
}

constexpr inline float hit_aabb(Aabb2 aabb, Ray2 ray) {
	Vector2 position = aabb.position - ray.origin;
	Vector2 fraction1 = (position - aabb.extents) / ray.direction;
	Vector2 fraction2 = (position + aabb.extents) / ray.direction;
	Vector2 min_fractions = min(fraction1, fraction2);
	Vector2 max_fractions = max(fraction1, fraction2);
	float min_fraction = max(min_fractions);
	float max_fraction = min(max_fractions);
	if (min_fraction < 0) { return -1; }
	if (min_fraction >= max_fraction) { return -1; }
	return min_fraction;
}

constexpr inline bool is_hit_aabb(Aabb2 aabb, Ray2 ray) {
	// auto before = false;
	for (size_t i = 0; i < 2; ++i) {
		float position = aabb.position[i] - ray.origin[i];
		float fraction1 = (position - aabb.extents[i]) / ray.direction[i];
		float fraction2 = (position + aabb.extents[i]) / ray.direction[i];
		float min_fraction = min(fraction1, fraction2);
		float max_fraction = max(fraction1, fraction2);
		// before = before || (min_fraction >= 0);
		if (min_fraction >= max_fraction) { return false; }
	}
	return true;
}

constexpr inline Vector2 get_aabb_normal(Aabb2 aabb, Vector2 hit) {
	Vector2 offset = hit - aabb.position;
	Vector2 offset_squared = offset * offset;
	offset_squared = offset_squared - max(offset_squared);
	return {
		sign(offset.x) * (offset_squared.x == 0),
		sign(offset.y) * (offset_squared.y == 0)
	};
}

//
// Aabb3 routines
//

constexpr inline float contains_point(Aabb3 aabb, Vector3 point) {
	Vector3 position = aabb.position - point;
	Vector3 minimum = position - aabb.extents;
	Vector3 maximum = position + aabb.extents;
	return (minimum.x <= 0) && (minimum.y <= 0) && (minimum.z <= 0)
	    && (maximum.x >= 0) && (maximum.y >= 0) && (maximum.z >= 0);
}

constexpr inline bool collide(Aabb3 first, Aabb3 second) {
	Vector3 position = first.position - second.position;
	Vector3 size = first.extents + second.extents;
	return (position.x <=  size.x) && (position.y <=  size.y) && (position.z <=  size.z)
	    && (position.x >= -size.x) && (position.y >= -size.y) && (position.z >= -size.z);
}

constexpr inline float hit_aabb(Aabb3 aabb, Ray3 ray) {
	Vector3 position = aabb.position - ray.origin;
	Vector3 fraction1 = (position - aabb.extents) / ray.direction;
	Vector3 fraction2 = (position + aabb.extents) / ray.direction;
	Vector3 min_fractions = min(fraction1, fraction2);
	Vector3 max_fractions = max(fraction1, fraction2);
	float min_fraction = max(min_fractions);
	float max_fraction = min(max_fractions);
	if (min_fraction < 0) { return -1; }
	if (min_fraction >= max_fraction) { return -1; }
	return min_fraction;
}

constexpr inline bool is_hit_aabb(Aabb3 aabb, Ray3 ray) {
	// bool before = false;
	for (size_t i = 0; i < 3; ++i) {
		float position = aabb.position[i] - ray.origin[i];
		float fraction1 = (position - aabb.extents[i]) / ray.direction[i];
		float fraction2 = (position + aabb.extents[i]) / ray.direction[i];
		float min_fraction = min(fraction1, fraction2);
		float max_fraction = max(fraction1, fraction2);
		// before = before || (min_fraction >= 0);
		if (min_fraction >= max_fraction) { return false; }
	}
	return true;
}

constexpr inline Vector3 get_aabb_normal(Aabb3 aabb, Vector3 hit) {
	Vector3 offset = hit - aabb.position;
	Vector3 offset_squared = offset * offset;
	offset_squared = offset_squared - max(offset_squared);
	return {
		sign(offset.x) * (offset_squared.x == 0),
		sign(offset.y) * (offset_squared.y == 0),
		sign(offset.z) * (offset_squared.z == 0)
	};
}

//
// Sphere routines
//

inline float hit_sphere(Sphere sphere, Ray3 ray) {
	Vector3 oc = ray.origin - sphere.xyz;
	float a = dot_product(ray.direction, ray.direction);
	float b = 2 * dot_product(oc, ray.direction);
	float c = dot_product(oc, oc) - sphere.w * sphere.w;
	float discriminant = b * b - 4 * a * c;
	if (discriminant < 0) { return -1; }
	return (-b - square_root(discriminant)) / (2 * a);
}

constexpr inline Vector3 get_sphere_normal(Sphere sphere, Vector3 hit) {
	return (hit - sphere.xyz) / sphere.w;
}
