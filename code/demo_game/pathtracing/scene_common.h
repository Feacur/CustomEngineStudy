struct Scene_Node {
	Aabb3 aabb;
	s32 shape_index;
	s32 nodes[SHAPES_COUNT];
	s32 nodes_count;
};

static Scene_Node scene_nodes[SHAPES_COUNT * 2] = {};
static s32 scene_nodes_count = 0;

Aabb3 shape_get_aabb(Shape shape);
void reinit_shapes(Game_Data * game_data);

void reinit_nodes(Game_Data * game_data) {
	scene_nodes[0] = {
		aabb3({0, 0, 0}, {9999, 9999, 9999}),
		-1,
		{0},
		0
	};

	for (s32 i = 0; i < game_data->shapes.length; ++i) {
		scene_nodes[0].nodes[i] = i + 1;
		scene_nodes[0].nodes_count++;

		scene_nodes[i + 1] = {
			shape_get_aabb(game_data->shapes[i]),
			i,
			{0},
			0
		};
		scene_nodes_count++;
	}
}
namespace game {
	Game_Data * allocate_game_memory() {
		auto game_data = globals::allocate_permanent<Game_Data>();
		*game_data = {};

		game_data->shapes.set_capacity(SHAPES_COUNT, &globals::allocate_permanent);
		game_data->materials.set_capacity(SHAPES_COUNT, &globals::allocate_permanent);
		
		return game_data;
	}
	
	bool needs_init() {
		if (globals::permanent_memory->length == 0) { return true; }
		auto game_data = (Game_Data *)globals::permanent_memory->data;
		return memcmp(game_data->checksum, checksum, sizeof(checksum)) != 0;
	}

	void init() {
		auto game_data = allocate_game_memory();
		memcpy(game_data->checksum, checksum, sizeof(checksum));
		game_data->random_state = (u32)globals::frame_timestamp;

		game_data->camera_position = {-3, 2, -8};
		game_data->camera_rotation = quaternion_from_radians(
			vec3(20, 10, 0) * deg_to_rad
		);

		reinit_shapes(game_data);
		// reinit_nodes(game_data);
		
		clear_buffer(globals::render_buffer_f, {0, 0, 0, 0});
	}
}
