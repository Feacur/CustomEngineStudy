struct Disc {
	Plane plane;
	float radius;
};

Raytrace_Result raytrace_disc(Disc disc, Ray3 ray) {
	auto raytrace_result = raytrace_plane(disc.plane, ray);
	if (raytrace_result.depth == too_far) {
		return raytrace_result;
	}

	Vector3 radius_vector = raytrace_result.point - disc.plane.point;

	float radius_vector_squared = magnitude_squared(radius_vector);
	float radius_squared = disc.radius * disc.radius;
	if (radius_vector_squared > radius_squared) {
		raytrace_result.depth = too_far;
		return raytrace_result;
	}
	
	return raytrace_result;
}
