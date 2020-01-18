//
// game
//

namespace field {
	void imprint_figure(Game_Data * game_data) {
		Array_Dynamic<Field_Cell> & field  = game_data->field;
		Array_Dynamic<Vector2i>   & figure = game_data->figure;
		s32 FIELD_WIDTH  = game_data->field_dimensions.x;
		s32 FIELD_HEIGHT = game_data->field_dimensions.y;

		Vector2i zero_point = game_data->position;
		for (s32 i = 0; i < figure.length; ++i) {
			Vector2i p = zero_point + figure[i];
			if (p.y < FIELD_HEIGHT) {
				field[p.y * FIELD_WIDTH + p.x] = true;
			}
		}
		
		figure.length = 0;
	}

	void reset(Game_Data * game_data) {
		Array_Dynamic<Field_Cell> & field = game_data->field;
		memset(field.data, 0, field.capacity * sizeof(Field_Cell));
		field.length = field.capacity;
	}

	bool get_line_is_value(Game_Data * game_data, s32 y, Field_Cell value) {
		s32 FIELD_WIDTH  = game_data->field_dimensions.x;
		s32 FIELD_HEIGHT = game_data->field_dimensions.y;

		Array_Dynamic<Field_Cell> & field = game_data->field;

		for (s32 x = 0; x < FIELD_WIDTH; ++x) {
			if (field[y * FIELD_WIDTH + x] != value) {
				return false;
			}
		}
		return true;
	}

	void copy_line(Game_Data * game_data, s32 from, s32 to) {
		if (from == to) { return; }

		Array_Dynamic<Field_Cell> & field = game_data->field;
		s32 FIELD_WIDTH  = game_data->field_dimensions.x;

		Field_Cell * source      = field.data + from * FIELD_WIDTH;
		Field_Cell * destination = field.data + to * FIELD_WIDTH;
		memcpy(destination, source, FIELD_WIDTH * sizeof(Field_Cell));
	}

	void collapse(Game_Data * game_data) {
		Array_Dynamic<Field_Cell> & field = game_data->field;
		s32 FIELD_WIDTH  = game_data->field_dimensions.x;
		s32 FIELD_HEIGHT = game_data->field_dimensions.y;

		s32 collapse_target = 0;
		for (s32 y = 0; y < FIELD_HEIGHT; ++y) {
			if (field::get_line_is_value(game_data, y, true)) { continue; }
			if (field::get_line_is_value(game_data, y, false)) { continue; }
			field::copy_line(game_data, y, collapse_target);
			collapse_target += 1;
		}
		
		if (collapse_target < FIELD_HEIGHT) {
			Field_Cell * destination = field.data + collapse_target * FIELD_WIDTH;
			memset(destination, 0, (field.length - collapse_target * FIELD_WIDTH) * sizeof(Field_Cell));
		}
	}
}

namespace game {
	Game_Data * allocate_game_memory(s32 cells_count) {
		auto game_data = globals::allocate_permanent<Game_Data>();
		*game_data = {};

		game_data->figure.set_capacity(FIGURE_PREFAB_LENGTH, &globals::allocate_permanent);
		game_data->field.set_capacity(cells_count, &globals::allocate_permanent);
		
		return game_data;
	}
	
	bool needs_init() {
		if (globals::permanent_memory->length == 0) { return true; }
		auto game_data = (Game_Data *)globals::permanent_memory->data;
		return memcmp(game_data->checksum, checksum, sizeof(checksum)) != 0;
	}

	void init() {
		Vector2i dimensions = {10, 20};
		s32 cells_count = dimensions.x * dimensions.y;

		auto game_data = allocate_game_memory(cells_count);
		memcpy(game_data->checksum, checksum, sizeof(checksum));
		game_data->random_state = (u32)globals::frame_timestamp;

		game_data->field_dimensions = dimensions;
		field::reset(game_data);
	}

	bool move_is_valid(Game_Data * game_data, Array_Dynamic<Vector2i> figure, Vector2i move) {
		// 1) inside the field bounds
		// 2) except the top, which is ignored
		// 3) does not overlap with other figures
		Array_Dynamic<Field_Cell> & field  = game_data->field;
		s32 FIELD_WIDTH  = game_data->field_dimensions.x;
		s32 FIELD_HEIGHT = game_data->field_dimensions.y;

		bool is_valid = true;
		
		Vector2i position = game_data->position + move;
		
		for (s32 i = 0; (i < figure.length) && is_valid; ++i) {
			Vector2i p = figure[i] + position;
			if ((p.x >= 0) && (p.x < FIELD_WIDTH) && (p.y >= 0)) {
				if (p.y < FIELD_HEIGHT) {
					bool is_filled = field[p.y * FIELD_WIDTH + p.x];
					is_valid = !is_filled;
				}
			}
			else {
				is_valid = false;
			}
		}
		
		return is_valid;
	}

	void set_figure(Game_Data * game_data, Array_Dynamic<Vector2i> source) {
		if (game::move_is_valid(game_data, source, {0, 0})) {
			Array_Dynamic<Vector2i> & figure = game_data->figure;
			memcpy(figure.data, source.data, figure.capacity * sizeof(Vector2i));
			figure.length = figure.capacity;
		}
	}

	void rotate_the_figure(Game_Data * game_data) {
		static Complexi const ROTATION = {0, 1};

		Array_Dynamic<Vector2i> & figure = game_data->figure;

		Array_Dynamic<Vector2i> copy = {};
		copy.set_capacity(figure.length, &globals::allocate_transient);
		copy.length = figure.length;

		for (s32 i = 0; i < figure.length; ++i) {
			copy[i] = complex_rotate_vector(figure[i], ROTATION);
		}

		set_figure(game_data, copy);
	}
}
