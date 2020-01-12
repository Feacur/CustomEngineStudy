constexpr inline r32 schlick(r32 one_minus_cosine, r32 factor) {
	// approximates reflectivity
	r32 r = (factor - 1) / (factor + 1);
	r32 c = one_minus_cosine * one_minus_cosine;
	return interpolate(c * c * one_minus_cosine, 1.0f, r * r);
}

Vector3 material_scatter(Game_Data * game_data, Material material, Vector3 incident, Vector3 normal) {
	static r32 const factor = 2;
	
	normal = normalize(normal + random3_radius01(&game_data->random_state) * material.roughness);

	if (material.type == Material::Type::Metal) {
		return reflect(incident, normal, factor);
	}

	if (material.type == Material::Type::Dielectric) {
		r32 refraction_factor;
		r32 cosine = dot_product(incident, normal);
		if (cosine < 0) {
			refraction_factor = air_refractive_index / material.refractive_index;
			cosine = -cosine;
		}
		else {
			refraction_factor = material.refractive_index / air_refractive_index;
			normal = -normal;
		}

		// decide between refraction and reflection paths
		if (random_01(&game_data->random_state) >= schlick(1 - cosine, refraction_factor)) {
			Vector3 scattered = refract(incident, normal, refraction_factor);
			if (scattered != vec3(0, 0, 0)) { return scattered; }
		}

		return reflect(incident, normal, factor);
	}

	return normal;
}
