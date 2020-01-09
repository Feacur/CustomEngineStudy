float raytrace_attenuation(Vector3 point, Vector3 normal, Vector3 to_light) {
	static float const proximity_epsilon = 0.1f;
	Vector3 offset_point = point + normal * proximity_epsilon;
	auto raytrace_result = raytrace_scene({offset_point, to_light});
	return (raytrace_result.id == 0) ? 1.0f : 0.0f;
}
