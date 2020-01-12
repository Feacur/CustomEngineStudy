/*
# Sphere intersection algorithm

## Here is initial data
* ray: ray_origin, ray_direction
* sphere: sphere_centre, sphere_radius

## This is a formula of an arbitrary point on the ray
ray_point = ray_origin + ray_direction * distance_from_the_origin

## This formula says that radius vector has sphere_radius length
dot_product(
	sphere_point - sphere_centre,
	sphere_point - sphere_centre
) = sphere_radius * sphere_radius

## Obviously we want to find the intersection point
* ray_point = sphere_point
* hit_distance = distance_from_the_origin

## Substitute sphere_point by ray_point formula
dot_product(
	ray_origin + ray_direction * hit_distance - sphere_centre,
	ray_origin + ray_direction * hit_distance - sphere_centre
) = sphere_radius * sphere_radius

## Shorten and expand the expression; dot product is replaced by *
(o + d * L - c) * (o + d * L - c) = r * r
(d * L + (o - c)) * (d * L + (o - c)) = r * r
(d * L) * (d * L) + 2 * (d * L) * (o - c) + (o - c) * (o - c) = r * r
(d * d) * L * L + (2 * d * (o - c)) * L + (o - c) * (o - c) = r * r

## Solve quadratic equation in order to calculate hit distance
a = dot_product(ray_direction, ray_direction)
b = 2 * dot_product(ray_direction, ray_origin - sphere_centre)
c = dot_product(ray_origin - sphere_centre, ray_origin - sphere_centre) - r * r

L1 = (-b + square_root(b * b - 4 * a * c)) / (2 * a)
L2 = (-b - square_root(b * b - 4 * a * c)) / (2 * a)
*/

Raytrace_Result raytrace_sphere(Sphere sphere, Ray3 ray) {
	Raytrace_Result result = {};

	Vector3 vector_to_centre = ray.origin - sphere.xyz;
	r32 cosine_between_ray_and_vector_from_sphere_to_origin = dot_product(ray.direction, vector_to_centre);
	if (cosine_between_ray_and_vector_from_sphere_to_origin > 0) {
		result.depth = too_far;
		return result;
	}

	r32 a = dot_product(ray.direction, ray.direction);
	r32 b = 2 * cosine_between_ray_and_vector_from_sphere_to_origin;
	r32 c = dot_product(vector_to_centre, vector_to_centre) - sphere.w * sphere.w;

	r32 D = b * b - 4 * a * c;
	if (D < 0) {
		result.depth = too_far;
		return result;
	}
	r32 square_root_of_D = square_root(D);

	r32 double_a = 2 * a;
	r32 normal_direction = 1;
	result.depth = (-b - square_root_of_D) / double_a;
	if (result.depth < 0) {
		normal_direction = -1;
		result.depth = (-b + square_root_of_D) / double_a;
	}

	result.point = ray.origin + ray.direction * result.depth;

	result.normal = result.point - sphere.xyz;
	result.normal = result.normal * (normal_direction / magnitude(result.normal));

	return result;
}
