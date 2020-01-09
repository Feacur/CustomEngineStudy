struct Raymarch_Result {
	float depth;
	int32 steps;
	float attenuation;
};

/*
1) Find the shortest distance from a given point
2) Move along the ray by this distance
3) Repeat until close enough or out of steps
*/
Raymarch_Result raymarch_scene(Ray3 ray, int32 steps_limit, float depth_limit) {
	static float const proximity_epsilon = 0.1f;

	Raymarch_Result result = {0, 0, 1};
	for (int32 i = 0; i < steps_limit; ++i) {
		float proximity = sdf_scene(ray.origin + ray.direction * result.depth);
		result.depth += proximity;
		result.steps = i;
		if (proximity < proximity_epsilon) { result.attenuation = 0; break; }
		result.attenuation = min(result.attenuation, 16 * proximity / result.depth);
		if (result.depth >= depth_limit) { break; }
	}

	return result;
}

float raymarch_attenuation(Vector3 point, Vector3 normal, Vector3 light, int32 steps_limit, float depth_limit) {
	static float const proximity_epsilon = 0.1f;
	Vector3 offset_point = point + normal * (proximity_epsilon * 2);
	Raymarch_Result raymarch_result = raymarch_scene({offset_point, light}, steps_limit, depth_limit);
	return raymarch_result.attenuation;
}
