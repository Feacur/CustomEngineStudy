namespace debug {
	static Vector4 const color_input_down = {1, 0, 0, 0.5f};
	static Vector4 const color_input_up   = {1, 0, 0, 0.25f};
}

namespace debug {
	void draw_input(Game_Data * game_data) {
		static Keyboard_Keys const input_keys[] = {
			Keyboard_Keys::Right,
			Keyboard_Keys::Left,
			Keyboard_Keys::Up,
			Keyboard_Keys::Down,
		};

		s32 MAP_WIDTH  = game_data->map_dimensions.x;
		s32 MAP_HEIGHT = game_data->map_dimensions.y;
		Vector2i input_points[] = {
			{(MAP_WIDTH - 1), (MAP_HEIGHT - 2)},
			{(MAP_WIDTH - 3), (MAP_HEIGHT - 2)},
			{(MAP_WIDTH - 2), (MAP_HEIGHT - 1)},
			{(MAP_WIDTH - 2), (MAP_HEIGHT - 3)},
		};

		Vector2 size = {(r32)TILE_SIZE.x, (r32)TILE_SIZE.y};
		for (s32 i = 0; i < 4; ++i) {
			Vector4 color = input::get_current(input_keys[i]) ? color_input_down : color_input_up;
			Vector2i xy = input_points[i] * TILE_SIZE;
			Vector2 position = {(r32)xy.x, (r32)xy.y};
			draw_rectangle(globals::render_buffer, position, size, color);
		}
	}
	
	void draw_pointer(Game_Data * game_data) {
		Vector2i pointer = input::get_pointer();
		Vector2i xy = pointer - TILE_SIZE / 2;
		Vector2 position = {(r32)xy.x, (r32)xy.y};
		Vector2 size = {(r32)TILE_SIZE.x, (r32)TILE_SIZE.y};
		draw_rectangle(globals::render_buffer, position, size, color_input_down);
	}
}
