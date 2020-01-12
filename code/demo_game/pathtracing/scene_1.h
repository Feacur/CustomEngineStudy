static s32 const trace_depth_limit = 10;
static s32 const samples_count = 4;
static r32 const camera_focus_distance = 1.0f;
static r32 const camera_aperture = 0.001f;

static r32 const air_refractive_index = 1;

static Shape const shapes[] = {
	// floor
	sphere({0,    -100,   0,    100 }),
	// back
	sphere({ 2,    0.5f,  2.0f, 0.5f}),
	sphere({ 0,    0.5f,  2.0f, 0.5f}),
	sphere({-2,    0.5f,  2.0f, 0.5f}),
	// front
	sphere({ 2,    0.5f, -0.5f, 0.5f}),
	sphere({ 0,    0.5f, -0.5f, 0.5f}),
	sphere({-2,    0.5f, -0.5f, 0.5f}),
	// transparent
	sphere({ 0.5f, 1.5f, -0.5f, 0.5f}),
	// light
	sphere({-1.5f, 2.0f,  0.5f, 0.3f}),
};
static s32 const shapes_count = C_ARRAY_LENGTH(shapes);

static Material const materials[] = {
	// floor
	mat0(Material::Type::Diffuse).set_albedo({0.8f, 0.8f, 0.8f}).set_roughness(1),
	// back
	mat0(Material::Type::Diffuse).set_albedo({0.8f, 0.4f, 0.4f}).set_roughness(1),
	mat0(Material::Type::Diffuse).set_albedo({0.4f, 0.8f, 0.4f}).set_roughness(1),
	mat0(Material::Type::Metal).set_albedo({0.4f, 0.4f, 0.8f}),
	// front
	mat0(Material::Type::Metal).set_albedo({0.4f, 0.8f, 0.4f}),
	mat0(Material::Type::Metal).set_albedo({0.4f, 0.8f, 0.4f}).set_roughness(0.2f),
	mat0(Material::Type::Metal).set_albedo({0.4f, 0.8f, 0.4f}).set_roughness(0.6f),
	// transparent
	mat0(Material::Type::Dielectric).set_albedo({1, 1, 1}).set_refractive_index(1.7f),
	// light
	mat0(Material::Type::Diffuse).set_emission({2, 2, 2}),
};

void reinit_shapes(Game_Data * game_data) { }
