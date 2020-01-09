//
// game
//

namespace field {
	void set_cell(Game_Data * game_data, Vector2i cell, Field_Cell value) {
		Array_Dynamic<Field_Cell> & field = game_data->field;
		int32 FIELD_WIDTH  = game_data->field_dimensions.x;
		int32 FIELD_HEIGHT = game_data->field_dimensions.y;

		if ((cell.x >= 0) && (cell.y >= 0) && (cell.x < FIELD_WIDTH) && (cell.y < FIELD_HEIGHT)) {
			field[cell.y * FIELD_WIDTH + cell.x] = value;
		}
	}

	#define GET_CELL_FUNC(ROUTINE_NAME) Field_Cell ROUTINE_NAME(Game_Data * game_data, int32 x, int32 y)
	typedef GET_CELL_FUNC(get_cell_func);

	GET_CELL_FUNC(get_cell_clamp) {
		Array_Dynamic<Field_Cell> const & field = game_data->field;
		int32 FIELD_WIDTH  = game_data->field_dimensions.x;
		int32 FIELD_HEIGHT = game_data->field_dimensions.y;

		if ((x >= 0) && (y >= 0) && (x < FIELD_WIDTH) && (y < FIELD_HEIGHT)) {
			return field[y * FIELD_WIDTH + x];
		}
		return false;
	}

	GET_CELL_FUNC(get_cell_wrap) {
		Array_Dynamic<Field_Cell> const & field = game_data->field;
		int32 FIELD_WIDTH  = game_data->field_dimensions.x;
		int32 FIELD_HEIGHT = game_data->field_dimensions.y;

		x = (x + FIELD_WIDTH) % FIELD_WIDTH;
		y = (y + FIELD_HEIGHT) % FIELD_HEIGHT;
		return field[y * FIELD_WIDTH + x];
	}
	#undef GET_CELL_FUNC

	bool get_cell_will_be_alive(Game_Data * game_data, int32 x, int32 y) {
		Array_Dynamic<Field_Cell> & field = game_data->field;
		int32 FIELD_WIDTH  = game_data->field_dimensions.x;
		int32 FIELD_HEIGHT = game_data->field_dimensions.y;

		get_cell_func * get_cell = &field::get_cell_wrap;
		int32 neighbours_number =
			  (*get_cell)(game_data, x - 1, y + 1)
			+ (*get_cell)(game_data, x    , y + 1)
			+ (*get_cell)(game_data, x + 1, y + 1)
			+ (*get_cell)(game_data, x - 1, y    )
			+ (*get_cell)(game_data, x + 1, y    )
			+ (*get_cell)(game_data, x - 1, y - 1)
			+ (*get_cell)(game_data, x    , y - 1)
			+ (*get_cell)(game_data, x + 1, y - 1);

		// Cell carries on
		if (neighbours_number == 2) {
			return field[y * FIELD_WIDTH + x];
		}
		
		// Cell borns
		if (neighbours_number == 3) {
			return true;
		}
		
		// Cell dies due to underpopulation ot overpopulation
		return false;
	}
	#undef GET_CELL

	void reset(Game_Data * game_data) {
		Array_Dynamic<Field_Cell> & field = game_data->field;
		memset(field.data, 0, field.capacity * sizeof(Field_Cell));
		field.length = field.capacity;
	}

	Field_Prefab parse_prefab(cstring source) {
		Field_Prefab prefab = {};
		prefab.tiles.set_capacity(strlen(source), globals::allocate_transient);

		int32 width = 0;
		while (char symbol = *(source++)) {
			switch (symbol) {
			case ' ': // empty
				width += 1;
				prefab.tiles.add(0);
				break;
			case '#': // filled
				width += 1;
				prefab.tiles.add(1);
				break;
			case '\n':
				ASSERT_TRUE(
					width == 0 || prefab.width == 0 || prefab.width == width,
					"Inconsistent field prefab width"
				);
				prefab.width = max(prefab.width, width);
				width = 0;
				break;
			}
		}

		return prefab;
	}

	void apply_prefab(Game_Data * game_data, cstring source) {
		Field_Prefab field_prefab = field::parse_prefab(source);
		Vector2i field_prefab_size = {
			field_prefab.width,
			(int32)field_prefab.tiles.length / field_prefab.width
		};

		Array_Dynamic<Field_Cell> & field = game_data->field;
		Vector2i point_zero = (game_data->field_dimensions - field_prefab_size) / 2;
		for (int32 y = 0; y < field_prefab_size.y; ++y) {
			for (int32 x = 0; x < field_prefab_size.x; ++x) {
				Vector2i cell = {point_zero.x + x, point_zero.y + y};
				field::set_cell(game_data, cell, field_prefab.tiles.data[y * field_prefab_size.x + x]);
			}
		}
	}
}

namespace game {
	Game_Data * allocate_game_memory(int32 cells_count) {
		auto game_data = globals::allocate_permanent<Game_Data>();
		*game_data = {};

		game_data->field.set_capacity(cells_count, &globals::allocate_permanent);
		
		return game_data;
	}
	
	bool needs_init() {
		if (globals::permanent_memory->length == 0) { return true; }
		auto game_data = (Game_Data *)globals::permanent_memory->data;
		return memcmp(game_data->checksum, checksum, sizeof(checksum)) != 0;
	}

	void init() {
		Vector2i dimensions = {70, 70};
		int32 cells_count = dimensions.x * dimensions.y;

		auto game_data = allocate_game_memory(cells_count);
		memcpy(game_data->checksum, checksum, sizeof(checksum));
		game_data->random_state = (uint32)globals::frame_timestamp;

		game_data->field_dimensions = dimensions;
		field::reset(game_data);
		field::apply_prefab(game_data, figure_prefab_glider_gun);
	}

	void update(Game_Data * game_data) {
		Array_Dynamic<Field_Cell> & field = game_data->field;
		int32 FIELD_WIDTH  = game_data->field_dimensions.x;
		int32 FIELD_HEIGHT = game_data->field_dimensions.y;

		Array_Dynamic<Field_Cell> copy = {};
		copy.set_capacity(field.length, &globals::allocate_transient);
		copy.length = field.length;

		for (int32 y = 0; y < FIELD_HEIGHT; ++y) {
			for (int32 x = 0; x < FIELD_WIDTH; ++x) {
				copy[y * FIELD_WIDTH + x] = field::get_cell_will_be_alive(game_data, x, y);
			}
		}

		memcpy(field.data, copy.data, copy.length);
		field.length = copy.length;
	}
}
