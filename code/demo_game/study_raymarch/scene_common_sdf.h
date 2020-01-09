Vector3 sdf_normal(Vector3 point) {
	static float const gradient_step = 0.1f;
	Vector3 normal = {
		sdf_scene({point.x + gradient_step, point.y, point.z}) - sdf_scene({point.x - gradient_step, point.y, point.z}),
		sdf_scene({point.x, point.y + gradient_step, point.z}) - sdf_scene({point.x, point.y - gradient_step, point.z}),
		sdf_scene({point.x, point.y, point.z + gradient_step}) - sdf_scene({point.x, point.y, point.z - gradient_step})
	};
	return normal / magnitude(normal);
}
