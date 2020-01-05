static uint8 const checksum[8] = {'p', 'a', 'c', 'm', 'a', 'n', ' ', ' '};

//
// data layout
//

struct Character {
	Vector2i position;
	Vector2i direction;
};

struct Timer {
	int32 elapsed;
	int32 period;
};

enum struct Mode : uint8 {
	None    = 0,
	Scatter = 1,
	Chase   = 2,
	Fright  = 3,
};
UNDERLYING_TYPE_META(Mode, uint8)
IS_ENUM_META(Mode)

enum struct Tile_Type : uint8 {
	None   = 0,
	Floor  = BIT(uint8, 0),
	Dot    = BIT(uint8, 1),
	Energy = BIT(uint8, 2),
	Slow   = BIT(uint8, 3),
	NoTurn = BIT(uint8, 4),
};
UNDERLYING_TYPE_META(Tile_Type, uint8)
IS_ENUM_META(Tile_Type)
ENUM_FLAG_OPERATORS_IMPL(Tile_Type)

struct Spawn_Point {
	uint8 team;
	Vector2i position;
};

using Tile = uint8;
struct Game_Data {
	uint8  checksum[8];
	uint32 random_state;
	// characters data
	Array_Dynamic<Character> characters;
	Array_Dynamic<Timer>     characters_timers;
	// Array_Dynamic<bool>      characters_reverse; // @Note: scheduled reverse is less responsive
	// map data
	Array_Dynamic<Spawn_Point> spawn_points;
	Array_Dynamic<Tile>        map;
	Vector2i                   map_dimensions;
	int32                      dots_count;
	// controls
	uint8 level;
	Mode  mode;
	int32 tile_time;
	int32 tile_steps;
	int32 fright_tile_steps;
};

static uint8 const MODES_COUNT = 8;
struct Modes {
	struct Config {
		Mode  type;
		int32 tile_steps;
	};
	int32  level;
	Config modes[MODES_COUNT];
};

static uint8 const CHARACTERS_COUNT = 5;
struct Speeds {
	struct Config {
		uint8 norm, norm_dot;
		uint8 fright, fright_dot;
		uint8 slow, fast;
	};
	int32 level;
	Config percents[CHARACTERS_COUNT];
};

static uint8 const MODES_PRESETS_COUNT = 3;
static uint8 const SPEEDS_PRESETS_COUNT = 4;
static uint8 const LEVEL_PRESETS_COUNT = 21;
struct Levels_Presets {
	Modes  modes[MODES_PRESETS_COUNT];
	Speeds speeds[SPEEDS_PRESETS_COUNT];
	uint8  fright_tile_steps[LEVEL_PRESETS_COUNT];
	uint8  fright_flashes[LEVEL_PRESETS_COUNT];
	uint8  fast_dots_left[LEVEL_PRESETS_COUNT];
};

//
// game settings
//

static int32 const UPDATE_PERIOD_INTERNAL = 100;
static int32 const TIME_PRECISION = 1000;

static uint8 const SPAWN_POINTS_LIMIT = 10;
static uint8 const CHARACTERS_TYPES_COUNT = 7;

static Levels_Presets const LEVELS_PRESETS = {
	{ // modes
		{ 0, {
			{Mode::Scatter, 70}, {Mode::Chase, 200},
			{Mode::Scatter, 70}, {Mode::Chase, 200},
			{Mode::Scatter, 50}, {Mode::Chase, 200},
			{Mode::Scatter, 50}, {Mode::Chase,   0},
		}},
		{ 1, {
			{Mode::Scatter, 70}, {Mode::Chase,   200},
			{Mode::Scatter, 70}, {Mode::Chase,   200},
			{Mode::Scatter, 50}, {Mode::Chase, 10330},
			{Mode::Scatter,  1}, {Mode::Chase,     0},
		}},
		{ 4, {
			{Mode::Scatter, 50}, {Mode::Chase,   200},
			{Mode::Scatter, 50}, {Mode::Chase,   200},
			{Mode::Scatter, 50}, {Mode::Chase, 10370},
			{Mode::Scatter,  1}, {Mode::Chase,     0},
		}},
	},
	{ // speeds
		{ 0, {
			{80, 71, 90, 79, 100, 80},
			{75, 75, 50, 50,  40, 80},
			{75, 75, 50, 50,  40, 75},
			{75, 75, 50, 50,  40, 75},
			{75, 75, 50, 50,  40, 75},
		}},
		{ 1, {
			{90, 79, 95, 83, 100, 90},
			{85, 85, 55, 55,  45, 90},
			{85, 85, 55, 55,  45, 85},
			{85, 85, 55, 55,  45, 85},
			{85, 85, 55, 55,  45, 85},
		}},
		{ 4, {
			{100, 87, 100, 87, 100, 100},
			{ 95, 95,  60, 60,  50, 100},
			{ 95, 95,  60, 60,  50,  95},
			{ 95, 95,  60, 60,  50,  95},
			{ 95, 95,  60, 60,  50,  95},
		}},
		{ 20, {
			{90, 79, 90, 79, 100,  90},
			{95, 95, 95, 95,  50, 100},
			{95, 95, 95, 95,  50,  95},
			{95, 95, 95, 95,  50,  95},
			{95, 95, 95, 95,  50,  95},
		}},
	},
	// fright_tile_steps
	{60, 50, 40, 30, 20, 50, 20, 20, 10, 50, 20, 10, 10, 30, 10, 10, 0, 10, 0, 0, 0},
	// fright_flashes
	{5, 5, 5, 5, 5, 5, 5, 5, 3, 5, 5, 3, 4, 5, 3, 3, 0, 3, 0, 0, 0},
	// fast_dots_left
	{20, 30, 40, 40, 40, 50, 50, 50, 60, 60, 60, 80, 80, 80, 100, 100, 100, 100, 120, 120, 120}
};

static uint8 const CHARACTER_TYPES[CHARACTERS_COUNT]   = {  1,  2,  3,  4,  5, };
static int8  const CHARACTER_TARGETS[CHARACTERS_COUNT] = { -1,  0,  0,  0,  0, };
static int8  const CHARACTER_ALLIES[CHARACTERS_COUNT]  = { -1, -1, -1,  1, -1, };
static uint8 const CHARACTER_TEAM[CHARACTERS_COUNT]    = {  0,  1,  1,  1,  1, };
static uint8 const CHARACTER_FAST[CHARACTERS_COUNT]    = {  0,  2,  0,  0,  0, };

static Vector2i const CHARACTER_CORNER_MUL[CHARACTERS_COUNT] = {
	{0, 0}, {1, 1}, {0, 1}, {1, 0}, {0, 0}
};

static Vector2i const CHARACTER_CORNER_ADD[CHARACTERS_COUNT] = {
	{0, 0}, {-3, 0}, {3, 0}, {0, 0}, {0, 0}
};

static uint8 const COLLISTION_PAIRS[][2] = {
	{0, 1}, {0, 2}, {0, 3}, {0, 4},
};
static uint8 const COLLISTION_PAIRS_COUNT = C_ARRAY_LENGTH(COLLISTION_PAIRS);

static Tile_Type const TILE_FLAGS[] = {
	Tile_Type::None,
	Tile_Type::Floor,
	Tile_Type::Floor | Tile_Type::Dot,
	Tile_Type::Floor | Tile_Type::Energy,
	Tile_Type::None,
	Tile_Type::Floor | Tile_Type::Slow,
	Tile_Type::Floor | Tile_Type::NoTurn,
	Tile_Type::Floor | Tile_Type::NoTurn | Tile_Type::Dot,
};
static Tile const TILE_FLAGS_COUNT = C_ARRAY_LENGTH(TILE_FLAGS);

//
// rendering settings
//

static Vector2i const TILE_SIZE      = { 8,  8};
static Vector2i const CHARACTER_SIZE = {12, 12};
static Vector2i const DOT_SIZE       = { 2,  2};
static Vector2i const ENERGY_SIZE    = { 4,  4};

static Vector4 const FRIGHTENED_COLOR = {0.0f, 0.2f, 1.0f, 1.0f};

static Vector4 const CHARACTER_COLORS[CHARACTERS_COUNT] = {
	{1.0f, 1.0f, 0.0f, 1.0f},
	{1.0f, 0.0f, 0.0f, 1.0f},
	{1.0f, 0.7f, 1.0f, 1.0f},
	{0.0f, 1.0f, 1.0f, 1.0f},
	{1.0f, 0.7f, 0.3f, 1.0f},
};

static Vector4 const TILE_COLORS[TILE_FLAGS_COUNT] = {
	{0.0f, 0.0f, 0.0f, 0.0f}, // none
	{0.0f, 0.0f, 0.0f, 1.0f}, // floor
	{0.0f, 0.0f, 0.0f, 1.0f}, // floor with dot
	{0.0f, 0.0f, 0.0f, 1.0f}, // floor with energy
	{0.0f, 0.0f, 0.8f, 1.0f}, // wall
	{0.0f, 0.0f, 0.0f, 1.0f}, // floor with slow
	{0.0f, 0.0f, 0.0f, 1.0f}, // floor with no turn
	{0.0f, 0.0f, 0.0f, 1.0f}, // floor with no turn and dot
};

static Vector4 const DOT_COLOR    = {0.8f, 0.6f, 0.4f, 1.0f};
static Vector4 const ENERGY_COLOR = {0.8f, 0.6f, 0.4f, 1.0f};
