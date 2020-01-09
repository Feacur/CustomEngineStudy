static float fraction = 0;
float sdf_blob_test_1(Vector3 point) {
	// Early out if we are too far even from the largest bounds
	float distance = sdf_sphere(point, 10);
	if (distance > 1.0f) { return distance; }

	return sdf_intersection(
		distance, sdf_box(point, {8, 8, 8})
	);
}

float sdf_blob_test_2(Vector3 point) {
	// Early out if we are too far even from the largest bounds
	float distance = sdf_sphere(point, 10);
	if (distance > 1.0f) { return distance; }

	distance = sdf_intersection(
		distance, sdf_box(point, {8, 8, 8})
	);

	return sdf_interpolate(
		distance,
		sdf_torus(point, 7, 3), fraction
	);
}

float sdf_scene(Vector3 point) {
	// return sdf_union(
	// 	sdf_sphere(point - vec3(0.0f, 6.0f, 0.0f), 10),
	// 	sdf_plane(point, {0, 1, 0})
	// );
	return sdf_union(
		sdf_blob_test_2(point - vec3(0.0f, 8.0f, 0.0f)),
		sdf_plane(point, {0, 1, 0})
	);
}
