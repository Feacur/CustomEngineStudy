static s32 const trace_depth_limit = 10;
static s32 const samples_count = 4;
static r32 const camera_focus_distance = 1.0f;
static r32 const camera_aperture = 0.001f;

static r32 const air_refractive_index = 1;

static s32 const SHAPES_COUNT = 100;

void reinit_shapes(Game_Data * game_data) {
	s32 shape_i = 0;

	game_data->shapes[shape_i] = sphere({0, -1000, 0, 1000});
	game_data->materials[shape_i] = mat0(Material::Type::Diffuse)
		.set_albedo({0.5f, 0.5f, 0.5f})
		.set_roughness(0.5f);
	shape_i++;

	game_data->shapes[shape_i] = sphere({-2, 1, -2, 1});
	game_data->materials[shape_i] = mat0(Material::Type::Dielectric)
		.set_albedo({1, 1, 1})
		.set_refractive_index(1.5f);
	shape_i++;
	
	game_data->shapes[shape_i] = sphere({2, 1, 4, 1});
	game_data->materials[shape_i] = mat0(Material::Type::Diffuse)
		.set_albedo({0.4f, 0.2f, 0.1f})
		.set_roughness(1);
	shape_i++;
	
	game_data->shapes[shape_i] = sphere({3, 1, -2, 1});
	game_data->materials[shape_i] = mat0(Material::Type::Metal)
		.set_albedo({0.7f, 0.6f, 0.5f})
		.set_roughness(0.01f);
	shape_i++;
	
	game_data->shapes[shape_i] = sphere({-4, 3, 4, 1});
	game_data->materials[shape_i] = mat0(Material::Type::Diffuse)
		.set_emission({2, 2, 2});
	shape_i++;

	for (int a = -4; a < 4; ++a) {
		if (shape_i == SHAPES_COUNT) { break; }
		for (int b = -4; b < 4; ++b) {
			if (shape_i == SHAPES_COUNT) { break; }

			r32 y = 0.2f + random_01(&game_data->random_state) * 0.1f;

			Vector2 offset = random2_radius01(&game_data->random_state) * 0.4f;
			Vector3 center = {a + offset.x, y, b + offset.y};
			
			if (magnitude(center - vec3(4, y, 0)) > 0.9f) {
				game_data->shapes[shape_i] = sphere({center, 0.2f});

				r32 choose_mat = random_radius01(&game_data->random_state);
				if (choose_mat < 0.5f) {
					game_data->materials[shape_i] = mat0(Material::Type::Diffuse)
						.set_albedo(random3_radius01(&game_data->random_state) * 0.8 + 0.2f)
						.set_roughness(random_radius01(&game_data->random_state) * 0.4f * 0.6f);
				} else if (choose_mat < 0.8f) {
					game_data->materials[shape_i] = mat0(Material::Type::Metal)
						.set_albedo(random3_radius01(&game_data->random_state) * 0.4f + 0.6f)
						.set_roughness(random_radius01(&game_data->random_state) * 0.01f);
				} else {
					game_data->materials[shape_i] = mat0(Material::Type::Dielectric)
						.set_albedo({1, 1, 1})
						.set_refractive_index(1.5f);
				}
				shape_i++;
			}
		}

		game_data->shapes.length = SHAPES_COUNT;
		game_data->materials.length = SHAPES_COUNT;
	}
}
