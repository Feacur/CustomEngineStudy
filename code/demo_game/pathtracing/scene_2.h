static s32 const trace_depth_limit = 10;
static s32 const samples_count = 4;
static r32 const camera_focus_distance = 1.0f;
static r32 const camera_aperture = 0.001f;

static r32 const air_refractive_index = 1;

static Shape const shapes[] = {
	// near
	sphere({ 0.2f,  0.5f,  -1.2f, 0.3f}),
	sphere({ 1.4f,  0.3f,  -1,    0.3f}),
	sphere({-1,     0.3f,  -0.5f, 0.3f}),
	// middle
	sphere({ 0.2f,  0.7f,   0.0f, 0.5f}),
	// aabb({{0.2f,  0.7f, 0.0f}, {1.0f, 1.0f, 1.0f}}),
	// far
	sphere({ 2,     0.6f,   1,    0.4f}),
	sphere({ 1,     0.3f,   1.5f, 0.3f}),
	sphere({-0.5f,  0.4f,   2,    0.3f}),
	// ground
	sphere({ 0,    -9999,   0,    9999}),
};
static s32 const shapes_count = C_ARRAY_LENGTH(shapes);

static Material const materials[] = {
	// near
	mat0(Material::Type::Metal).set_albedo({0.8f, 0.2f, 0.5f}),
	mat0(Material::Type::Metal).set_albedo({0.2f, 0.8f, 0.5f}).set_roughness(0.2f),
	mat0(Material::Type::Metal).set_albedo({0.2f, 0.5f, 0.8f}),
	// middle
	mat0(Material::Type::Dielectric).set_albedo({1, 1, 1}).set_roughness(0.05f).set_refractive_index(2),
	// mat0(Material::Type::Diffuse).set_albedo({0.3f, 1.0f, 1.0f}).set_roughness(1),
	// far
	mat0(Material::Type::Diffuse).set_emission({2, 2, 2}),
	mat0(Material::Type::Diffuse).set_albedo({0.8f, 0.8f, 0.2f}).set_roughness(1),
	mat0(Material::Type::Diffuse).set_albedo({0.2f, 0.8f, 0.8f}).set_roughness(1),
	// ground
	mat0(Material::Type::Diffuse).set_albedo({0.5f, 0.5f, 0.5f}).set_roughness(1),
};

void reinit_shapes(Game_Data * game_data) { }
