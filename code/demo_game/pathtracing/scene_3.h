static s32 const trace_depth_limit = 10;
static s32 const samples_count = 4;
static r32 const camera_focus_distance = 1.0f;
static r32 const camera_aperture = 0.001f;

static r32 const air_refractive_index = 1;

static s32 const SHAPES_COUNT = 40;

void reinit_shapes(Game_Data * game_data) {
	// shapes
	s32 shape_i = 0;
	game_data->shapes[shape_i++] = sphere({ 0.2f,  0.7f,   0.0f, 0.5f});
	game_data->shapes[shape_i++] = sphere({ 2,     0.6f,   1,    0.4f});
	game_data->shapes[shape_i++] = sphere({ 0,    -9999,   0,    9999});
	
	for(; shape_i < SHAPES_COUNT; ++shape_i) {
		game_data->shapes[shape_i] = sphere({
			random_radius01(&game_data->random_state) * 2.0f + 1.0f,
			0.1f + random_01(&game_data->random_state) * 0.2f,
			random_radius01(&game_data->random_state) * 2.0f + 1.0f,
			0.05f + random_01(&game_data->random_state) * 0.1f
		});
	}
	
	// materials
	shape_i = 0;
	game_data->materials[shape_i++] = mat0(Material::Type::Dielectric)
		.set_albedo({1, 1, 1})
		.set_roughness(0.05f)
		.set_refractive_index(2);
	
	game_data->materials[shape_i++] = mat0(Material::Type::Diffuse)
		.set_emission({2, 2, 2});
	
	game_data->materials[shape_i++] = mat0(Material::Type::Diffuse)
		.set_albedo({0.5f, 0.5f, 0.5f})
		.set_roughness(1);
	
	for(; shape_i < SHAPES_COUNT / 3; ++shape_i) {
		game_data->materials[shape_i] = mat0(Material::Type::Diffuse)
			.set_albedo(random3_01(&game_data->random_state) * 0.8f + 0.2f)
			.set_roughness(random_01(&game_data->random_state) * 0.1f + 0.9f);
	}
	
	for(; shape_i < SHAPES_COUNT * 2 / 3; ++shape_i) {
		game_data->materials[shape_i] = mat0(Material::Type::Metal)
			.set_albedo(random3_01(&game_data->random_state) * 0.5f + 0.5f)
			.set_roughness(random_01(&game_data->random_state) * 0.2f);
	}
	
	for(; shape_i < SHAPES_COUNT; ++shape_i) {
		game_data->materials[shape_i] = mat0(Material::Type::Dielectric)
			.set_albedo(random3_01(&game_data->random_state) * 0.1f + 0.9f)
			.set_roughness(random_01(&game_data->random_state) * 0.05f)
			.set_refractive_index(random_01(&game_data->random_state) + 1.5f);
	}

	game_data->shapes.length = SHAPES_COUNT;
	game_data->materials.length = SHAPES_COUNT;
}
