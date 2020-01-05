static uint8 const checksum[8] = {'e', 'n', 't', 'i', 't', 'y', 's', 'y'};

//
// data layout
//

// typedef size_t Entity;
using Entity = size_t;

enum struct Component_Type : uint8 {
	None         = 0,
	Position     = BIT(uint8, 0),
	Rotation     = BIT(uint8, 1),
	Velocity     = BIT(uint8, 2),
	Sprite       = BIT(uint8, 3),
};
UNDERLYING_TYPE_META(Component_Type, uint8)
IS_ENUM_META(Component_Type)
ENUM_FLAG_OPERATORS_IMPL(Component_Type)

struct Position_Component {
	Vector2 value;
};

struct Rotation_Component {
	Complex value;
};

struct Velocity_Component {
	Vector2 value;
};

struct Sprite_Component {
	Vector4 color;
};

struct Entity_SoA {
	Array_Dynamic<Component_Type>     flags;
	Array_Dynamic<Position_Component> position;
	Array_Dynamic<Rotation_Component> rotation;
	Array_Dynamic<Velocity_Component> velocity;
	Array_Dynamic<Sprite_Component>   sprite;
};

struct Systems_Data {
	Array_Dynamic<Entity> rotation;
	Array_Dynamic<Entity> movement;
	Array_Dynamic<Entity> world_bounds;
	Array_Dynamic<Entity> rendering;
	Array_Dynamic<Entity> rendering_no_rotation;
};

struct Renderable {
	Vector2 position;
	Complex rotation;
	Vector2 size;
	Vector4 color;
};

struct Game_Data {
	uint8        checksum[8];
	uint32       random_state;
	// all entities data
	Entity_SoA   entities;
	Aabb2        world_bounds;
	Systems_Data systems;
	Array_Dynamic<Renderable> renderables;
};

//
// game settings
//

static size_t const ENTITIES_COUNT = 10;
static Aabb2 const BOUNDS = {{0, 0}, {200, 200}};
