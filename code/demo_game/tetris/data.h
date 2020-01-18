static u8 const checksum[8] = {'t', 'e', 't', 'r', 'i', 's', ' ', ' '};

//
// data layout
//

using Field_Cell = u8;
struct Game_Data {
	u8                     checksum[8];
	u32                    random_state;
	// actual data
	Array_Dynamic<Field_Cell> field;
	Vector2i                  field_dimensions;
	Array_Dynamic<Vector2i>   figure;
	Vector2i                  position;
	// controls
	s32                     step_time_side;
	s32                     step_time_down;
};

//
// game settings
//

static s32 const TIME_STEP_SIDE        = 100 * 1000;
static s32 const TIME_STEP_DOWN_NORMAL = 200 * 1000;
static s32 const TIME_STEP_DOWN_FAST   = 50 * 1000;

static s32 const FIGURE_PREFAB_LENGTH = 4;
static Vector2i const figure_prefabs[][FIGURE_PREFAB_LENGTH] {
	{{ 0,  2}, {0, 1}, {0, 0}, { 1, 0}}, // L
	{{ 0,  2}, {0, 1}, {0, 0}, {-1, 0}}, // L inverse
	{{-1,  0}, {0, 0}, {0, 1}, { 1, 1}}, // S
	{{-1,  1}, {0, 1}, {0, 0}, { 1, 0}}, // S inverse
	{{ 0, -1}, {0, 0}, {0, 1}, { 0, 2}}, // I
	{{ 0,  0}, {0, 1}, {1, 0}, { 1, 1}}, // O
	{{-1,  0}, {0, 0}, {1, 0}, { 0, 1}}, // T
};
static s32 const FIGURES_NUMBER = C_ARRAY_LENGTH(figure_prefabs);

//
// rendering settings
//

static Vector2i const TILE_SIZE = {10, 10};
static Vector2i const CELL_SIZE = { 8,  8};

static Vector4 const color_background_odd  = {0.25f, 0.25f, 0.25f, 1};
static Vector4 const color_background_even = {0.1f, 0.1f, 0.1f, 1};
static Vector4 const color_field           = {0.8f, 0.8f, 0.8f, 1};
static Vector4 const color_figure          = {0.95f, 0.95f, 0.95f, 1};

static Vector4 const color_input_down = {1, 0, 0, 0.5f};
static Vector4 const color_input_up   = {1, 0, 0, 0.25f};
