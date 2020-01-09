/*
# Plane intersection algorithm

## Here is initial data
* ray: ray_origin, ray_direction
* plane: plane_point, plane_normal

## This is a formula of an arbitrary point on the ray
ray_point = ray_origin + ray_direction * distance_from_the_origin

## This formula says that a line on a plane is perpendicular to the normal
dot_product(plane_point - another_plane_point, plane_normal) = 0

## Obviously we want to find the intersection point
* ray_point = another_plane_point
* hit_distance = distance_from_the_origin

## Substitute another_plane_point by ray_point formula
dot_product(plane_point - (ray_origin + ray_direction * hit_distance), plane_normal) = 0

## Dot product is distributive; scalar multiplication with hit_distance can be factored out
dot_product(plane_point - ray_origin, plane_normal) - dot_product(ray_direction, plane_normal) * hit_distance = 0

## Calculate hit distance
hit_distance = dot_product(plane_point - ray_origin, plane_normal) / dot_product(ray_direction, plane_normal)
*/

struct Plane {
	Vector3 point;
	Vector3 normal;
};

Raytrace_Result raytrace_plane(Plane plane, Ray3 ray) {
	Raytrace_Result result = {};
	float cosine_between_ray_and_normal = dot_product(ray.direction, plane.normal);
	if (cosine_between_ray_and_normal <= -0.0001f) {
		result.depth = dot_product(plane.point - ray.origin, plane.normal) / cosine_between_ray_and_normal;
		result.point = ray.origin + ray.direction * result.depth;
		result.normal = plane.normal;
	}
	else if (cosine_between_ray_and_normal >= 0.0001f) {
		result.depth = dot_product(plane.point - ray.origin, plane.normal) / cosine_between_ray_and_normal;
		result.point = ray.origin + ray.direction * result.depth;
		result.normal = -plane.normal;
	}
	else {
		result.depth = too_far;
	}

	if (result.depth < 0) {
		result.depth = too_far;
	}

	return result;
}
