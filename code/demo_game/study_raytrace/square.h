struct Square {
	Plane plane;
	Vector3 up;
	Vector2 size;
};

Raytrace_Result raytrace_square(Square square, Ray3 ray) {
	auto raytrace_result = raytrace_plane(square.plane, ray);
	if (raytrace_result.depth == too_far) {
		return raytrace_result;
	}

	Vector3 radius_vector = raytrace_result.point - square.plane.point;

	Vector2 half_size = square.size / 2;
	r32 up = dot_product(square.up, radius_vector);
	if (up > half_size.y || up < -half_size.y) {
		raytrace_result.depth = too_far;
		return raytrace_result;
	}

	Vector3 right_vector = cross_product(square.up, square.plane.normal);
	r32 right = dot_product(right_vector, radius_vector);
	if (right > half_size.x || right < -half_size.x) {
		raytrace_result.depth = too_far;
		return raytrace_result;
	}

	return raytrace_result;
}
