struct Raymarch_Result {
	r32 depth;
	s32 steps;
	r32 attenuation;
};

/*
1) Find the shortest distance from a given point
2) Move along the ray by this distance
3) Repeat until close enough or out of steps
*/
Raymarch_Result raymarch_scene(Ray3 ray, s32 steps_limit, r32 depth_limit) {
	static r32 const proximity_epsilon = 0.1f;

	Raymarch_Result result = {0, 0, 1};
	for (s32 i = 0; i < steps_limit; ++i) {
		r32 proximity = sdf_scene(ray.origin + ray.direction * result.depth);
		result.depth += proximity;
		result.steps = i;
		if (proximity < proximity_epsilon) { result.attenuation = 0; break; }
		result.attenuation = min(result.attenuation, 16 * proximity / result.depth);
		if (result.depth >= depth_limit) { break; }
	}

	return result;
}

r32 raymarch_attenuation(Vector3 point, Vector3 normal, Vector3 light, s32 steps_limit, r32 depth_limit) {
	static r32 const proximity_epsilon = 0.1f;
	Vector3 offset_point = point + normal * (proximity_epsilon * 2);
	Raymarch_Result raymarch_result = raymarch_scene({offset_point, light}, steps_limit, depth_limit);
	return raymarch_result.attenuation;
}
