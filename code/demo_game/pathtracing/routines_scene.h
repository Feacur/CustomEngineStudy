Hit_Data scene_trace_shapes(Game_Data * game_data, Ray3 ray, r32 near, r32 far) {
	Hit_Data result = { -1, far };
	for (s32 i = 0; i < game_data->shapes.length; ++i) {
		r32 distance_test = shape_get_distance(game_data->shapes[i], ray);
		if (distance_test > near && result.distance > distance_test) {
			result = { i, distance_test };
		}
	}
	return result;
}

Hit_Data scene_trace_nodes(Game_Data * game_data, s32 node_index, Ray3 ray, r32 near, r32 far) {
	Scene_Node const & node = scene_nodes[node_index];
	
	// AABB helps us to exit out early before tracing anything complicated
	if (!is_hit_aabb(node.aabb, ray)) { return { -1, far }; }
	
	if (node.shape_index >= 0) {
		r32 distance_test = shape_get_distance(game_data->shapes[node.shape_index], ray);
		if (distance_test > near && far > distance_test) {
			return { node.shape_index, distance_test };
		}
	}
	
	Hit_Data result = { -1, far };
	for (s32 i = 0; i < node.nodes_count; ++i) {
		Hit_Data node_result = scene_trace_nodes(game_data, node.nodes[i], ray, near, result.distance);
		if (node_result.distance > near && result.distance > node_result.distance) {
			result = node_result;
		}
	}
	return result;
}

Vector3 scene_sample_color(Game_Data * game_data, Ray3 ray, int depth) {
	Vector3 color = {};
	Vector3 attenuation = {1, 1, 1};
	for (s32 i = 0; i < depth; ++i) {
		static r32 const attenuation_threshold_squared = 0.01f;
		if (magnitude_squared(attenuation) < attenuation_threshold_squared) { break; }

		Hit_Data data = scene_trace_shapes(game_data, ray, 0.001f, INFINITY);
		// Hit_Data data = scene_trace_nodes(0, ray, 0.001f, INFINITY);
		if (data.index >= 0) {
			Shape const    & shape    = game_data->shapes[data.index];
			Material const & material = game_data->materials[data.index];
			
			Vector3 hit = ray_point(ray, data.distance);
			Vector3 normal = shape_get_normal(shape, hit);
			
			ray = {hit, material_scatter(game_data, material, ray.direction, normal)};
			
			color += (attenuation * material.emission);
			attenuation = (attenuation * material.albedo);
		}
		else {
			color += (attenuation * texture_sky_emission(ray));
			break; // attenuation = {};
		}
	}
	return color;
}
