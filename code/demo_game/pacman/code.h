//
// game
//

namespace utils {
	static Vector2i const offsets[] = {
		{-1,  0},
		{ 1,  0},
		{ 0, -1},
		{ 0,  1},
	};
	
	Vector2i get_cardinal_direction(Vector2i direction, Vector2i previous) {
		if (previous.y != 0) {
			if (previous.y != -direction.y && direction.x != 0) {
				return {direction.x, 0};
			}
			return {0, direction.y};
		}
		
		if (previous.x != -direction.x && direction.y != 0) {
			return {0, direction.y};
		}
		return {direction.x, 0};
	}
	
	constexpr inline Vector2i wrap_position(Vector2i value, Vector2i dimensions) {
		return (value + dimensions) % dimensions;
	}

	inline Tile get_tile_by_type(Tile_Type type, Tile defult_tile) {
		for (Tile i = 0; i < TILE_FLAGS_COUNT; ++i) {
			if (TILE_FLAGS[i] == type) { return i; }
		}
		return defult_tile;
	}
}

namespace utils {
	struct Parsed_Map_Prefab {
		struct Spawn_Tile {
			uint8 team;
			int32 tile_index;
		};
		Array_Dynamic<Tile> tiles;
		int32 width;
		int32 dots_count;
		Array_Dynamic<Spawn_Tile> spawn_tiles;
	};

	Parsed_Map_Prefab parse_map_prefab(cstring prefab) {
		Parsed_Map_Prefab result = {};
		result.tiles.set_capacity(strlen(prefab), globals::allocate_transient);
		result.spawn_tiles.set_capacity(SPAWN_POINTS_LIMIT, globals::allocate_transient);

		int32 current_width = 0;
		while (char symbol = *(prefab++)) {
			switch (symbol) {
			case 'u': case '=': // none
				result.tiles.add(0);
				current_width += 1;
				break;
			case ' ': // floor
				result.tiles.add(1);
				current_width += 1;
				break;
			case '0': case '1': // floor with a spawn
				result.tiles.add(1);
				result.spawn_tiles.add({
					(uint8)(symbol - '0'),
					(int32)(result.tiles.length - 1)
				});
				current_width += 1;
				break;
			case 'v': // floor with no turn
				result.tiles.add(6);
				current_width += 1;
				break;
			case '.': // floor with dot
				result.tiles.add(2);
				result.dots_count += 1;
				current_width += 1;
				break;
			case 'x': // floor with no turn and dot
				result.tiles.add(7);
				result.dots_count += 1;
				current_width += 1;
				break;
			case 'o': // floor with energy
				result.tiles.add(3);
				result.dots_count += 1;
				current_width += 1;
				break;
			case '#': // wall
				result.tiles.add(4);
				current_width += 1;
				break;
			case '<': case '>': // floor with slow
				result.tiles.add(5);
				current_width += 1;
				break;
			case '\n':
				ASSERT_TRUE(
					current_width == 0 || result.width == 0 || result.width == current_width,
					"Inconsistent map prefab width"
				);
				result.width = max(result.width, current_width);
				current_width = 0;
				break;
			}
		}

		return result;
	}

	void flip_y(Array_Dynamic<Tile> tiles, int32 width) {
		Array_Dynamic<Tile> line = {};
		line.set_capacity(width, globals::allocate_transient);
		size_t line_bytes = width * sizeof(Tile);

		int32 height = (int32)tiles.length / width;
		int32 height_half = height / 2;
		for (int32 y = 0; y < height_half; ++y) {
			Tile * a = tiles.data + y * width;
			Tile * b = tiles.data + (height - y - 1) * width;

			memcpy(line.data, a,         line_bytes);
			memcpy(a,         b,         line_bytes);
			memcpy(b,         line.data, line_bytes);
		}
	}
}

namespace utils {
	struct Level_Compiled {
		Modes const  * modes;
		Speeds const * speeds;
		uint8 fright_tile_steps;
		uint8 fright_flashes;
		uint8 fast_dots_left;
	};

	Level_Compiled get_level_settings(uint8 level) {
		Level_Compiled result;

		for (uint8 i = 0; i < MODES_PRESETS_COUNT; ++i) {
			if (LEVELS_PRESETS.modes[i].level > level) { break; }
			result.modes = &LEVELS_PRESETS.modes[i];
		}

		for (uint8 i = 0; i < SPEEDS_PRESETS_COUNT; ++i) {
			if (LEVELS_PRESETS.speeds[i].level > level) { break; }
			result.speeds = &LEVELS_PRESETS.speeds[i];
		}

		uint8 preset_index = min(level, LEVEL_PRESETS_COUNT);
		result.fright_tile_steps = LEVELS_PRESETS.fright_tile_steps[preset_index];
		result.fright_flashes    = LEVELS_PRESETS.fright_flashes[preset_index];
		result.fast_dots_left    = LEVELS_PRESETS.fast_dots_left[preset_index];

		return result;
	}
}

namespace map {
	Tile get_tile(Game_Data * game_data, Vector2i position) {
		size_t index = position.y * game_data->map_dimensions.x + position.x;
		return game_data->map[index];
	}

	Tile get_tile(Game_Data * game_data, size_t index) {
		return game_data->map[index];
	}

	bool is_walkable(Game_Data * game_data, Vector2i position) {
		Tile tile = map::get_tile(game_data, position);
		return bits_are_set(TILE_FLAGS[tile], Tile_Type::Floor);
	}

	bool is_walkable(Game_Data * game_data, size_t index) {
		Tile tile = map::get_tile(game_data, index);
		return bits_are_set(TILE_FLAGS[tile], Tile_Type::Floor);
	}

	bool is_slow(Game_Data * game_data, Vector2i position) {
		Tile tile = map::get_tile(game_data, position);
		return bits_are_set(TILE_FLAGS[tile], Tile_Type::Slow);
	}

	bool is_no_turn(Game_Data * game_data, Vector2i position) {
		Tile tile = map::get_tile(game_data, position);
		return bits_are_set(TILE_FLAGS[tile], Tile_Type::NoTurn);
	}

	Vector2i get_spawn_point(Game_Data * game_data, uint8 character_index) {
		uint8 team = CHARACTER_TEAM[character_index];
		for (int32 i = 0; i < game_data->spawn_points.length; ++i) {
			Spawn_Point spawn_point = game_data->spawn_points[i];
			if (spawn_point.team != team) { continue; }
			return spawn_point.position;
		}
		return {0, 0};
	}
}

namespace path_finding {
	struct Walls {
		uint8 data[4];
		uint8 count;
	};

	Walls get_walls(Game_Data * game_data, Vector2i position) {
		Vector2i dimensions = game_data->map_dimensions;

		Vector2i targets[4] = {};
		Tile     tiles[4]   = {};
		Walls    walls      = {};

		for (uint8 i = 0; i < 4; ++i) {
			targets[i] = utils::wrap_position(position + utils::offsets[i], dimensions);
		}
		for (uint8 i = 0; i < 4; ++i) {
			tiles[i] = map::get_tile(game_data, targets[i]);
		}
		for (uint8 i = 0; i < 4; ++i) {
			Tile tile = tiles[i];
			walls.data[i] = !bits_are_set(TILE_FLAGS[tile], Tile_Type::Floor);
		}
		for (uint8 i = 0; i < 4; ++i) {
			walls.count += walls.data[i];
		}

		return walls;
	}
	
	constexpr inline int32 estimate_distance(Vector2i value) {
		// return abs(value.x) + abs(value.y);
		return magnitude_squared(value);
	}
}

namespace path_finding {
	Array_Dynamic<Vector2i> get_path(Game_Data * game_data, Vector2i from, Vector2i to, Vector2i direction) {
		Vector2i dimensions = game_data->map_dimensions;
		int32 width = game_data->map_dimensions.x;
		int32 height = game_data->map_dimensions.y;

		int32 start = from.y * width + from.x;
		int32 target = to.y * width + to.x;

		// allocate memory
		Array_Dynamic<int32> path_costs = {};
		Array_Dynamic<int32> path_parents = {};
		Array_Dynamic<Vector2i> path_positions = {};
		path_costs.set_capacity(game_data->map.length, globals::allocate_transient);
		path_parents.set_capacity(game_data->map.length, globals::allocate_transient);

		Array_Dynamic<int32> nodes_frontier = {};
		Array_Dynamic<int32> nodes_priorities = {};
		nodes_frontier.set_capacity(game_data->map.length, globals::allocate_transient);
		nodes_priorities.set_capacity(game_data->map.length, globals::allocate_transient);
		
		Array_Dynamic<int32> visited = {};
		visited.set_capacity(game_data->map.length, globals::allocate_transient);
		
		// set initial
		memset(path_costs.data, INT32_MAX, path_costs.capacity * sizeof(int32));
		memset(path_parents.data, -1, path_parents.capacity * sizeof(int32));
		path_costs.length = path_costs.capacity;
		path_parents.length = path_parents.capacity;

		nodes_frontier.add(start);
		nodes_priorities.add(0);

		// search
		while (nodes_frontier.length > 0) {
			// select current
			size_t node_index = get_index_minimum(nodes_priorities);
			int32 current = nodes_frontier[node_index];
			if (current == target) { break; }

			int32 current_cost = path_costs[current];
			Vector2i current_position = {current % width, current / width};

			// mark current as visited
			nodes_frontier[node_index] = nodes_frontier[--nodes_frontier.length];
			nodes_priorities[node_index] = nodes_priorities[--nodes_priorities.length];
			visited.add(current);

			// iterate possible paths
			for (uint8 offset_index = 0; offset_index < 4; ++offset_index) {
				Vector2i offset = utils::offsets[offset_index];
				Vector2i next_position = utils::wrap_position(current_position + offset, dimensions);
				int32 next_i = next_position.y * width + next_position.x;
				if (!map::is_walkable(game_data, next_i)) { continue; }

				int32 next_cost = current_cost + path_finding::estimate_distance(offset);
				if (contains(visited, next_i) && next_cost > path_costs[next_i]) { continue; }

				int32 next_priority = next_cost + path_finding::estimate_distance(to - next_position);

				path_costs[next_i] = next_cost;
				path_parents[next_i] = current;

				nodes_frontier.add(next_i);
				nodes_priorities.add(next_priority);
			}
		}

		// reconstruct path
		Array_Dynamic<Vector2i> path = {};
		path.set_capacity(visited.length, globals::allocate_transient);

		int32 path_index = to.y * width + to.x;
		while (path_parents[path_index] >= 0) {
			path.add({path_index % width, path_index / width});
			path_index = path_parents[path_index];
		}
		if (path_index != start) { path.length = 0; }

		return path;
	}
}

namespace path_finding {
	Vector2i get_direction(Game_Data * game_data, Vector2i from, Vector2i to, Vector2i direction) {
		Walls walls = get_walls(game_data, from);

		// stay if can't move
		if (walls.count == 4) {
			return {0, 0};
		}

		// turn around at dead-ends
		if (walls.count == 3) {
			for (uint8 i = 0; i < 4; ++i) {
				if (walls.data[i]) { continue; }
				return utils::offsets[i];
			}
		}
		
		// move down the corridor
		bool is_no_turn = map::is_no_turn(game_data, from);
		if (walls.count == 2 || is_no_turn) {
			for (uint8 i = 0; i < 4; ++i) {
				if (walls.data[i]) { continue; }
				if (direction == -utils::offsets[i]) { continue; }
				return utils::offsets[i];
			}
		}

		// do not bash into the walls
		Vector2i distance_vector = to - from;
		int32 priority = path_finding::estimate_distance(distance_vector - direction);
		for (uint8 i = 0; i < 4; ++i) {
			if (direction != utils::offsets[i]) { continue; }
			if (walls.data[i]) { priority = INT32_MAX; }
			break;
		}

		// evaluate the best direction
		Vector2i new_direction = direction;
		for (uint8 i = 0; i < 4; ++i) {
			if (walls.data[i]) { continue; }
			if (direction == -utils::offsets[i]) { continue; }

			int32 priority_b = path_finding::estimate_distance(distance_vector - utils::offsets[i]);
			if (priority > priority_b) {
				priority = priority_b;
				new_direction = utils::offsets[i];
			}
		}
		return new_direction;
	}
}

namespace path_finding {
	Vector2i get_random_direction(Game_Data * game_data, Vector2i from, Vector2i direction) {
		Walls walls = get_walls(game_data, from);

		// stay if can't move
		if (walls.count == 4) {
			return {0, 0};
		}

		// turn around at dead-ends
		if (walls.count == 3) {
			for (uint8 i = 0; i < 4; ++i) {
				if (walls.data[i]) { continue; }
				return utils::offsets[i];
			}
		}
		
		// move down the corridor
		if (walls.count == 2) {
			for (uint8 i = 0; i < 4; ++i) {
				if (walls.data[i]) { continue; }
				if (direction == -utils::offsets[i]) { continue; }
				return utils::offsets[i];
			}
		}

		// select random direction
		uint8 index_offset = random_in_range(&game_data->random_state, (uint8)0, (uint8)4);
		for (uint8 i = 0; i < 4; ++i) {
			uint8 offset_i = (i + index_offset) % 4;
			if (walls.data[offset_i]) { continue; }
			return utils::offsets[offset_i];
		}

		return direction;
	}
}

namespace character {
	void add(Game_Data * game_data) {
		game_data->characters.add();
		game_data->characters_timers.add();
		// game_data->characters_reverse.add();
	}

	void reverse(Game_Data * game_data, uint8 i) {
		Character * character = &game_data->characters[i];
		Timer * timer = &game_data->characters_timers[i];

		Vector2i next_position = utils::wrap_position(character->position + character->direction, game_data->map_dimensions);
		int32 next_i = next_position.y * game_data->map_dimensions.x + next_position.x;
		if (map::is_walkable(game_data, next_i)) {
			character->position = next_position;
			timer->elapsed = timer->period - timer->elapsed;
		}
		else {
			timer->elapsed = 0;
		}

		character->direction = -character->direction;
	}

	void respawn(Game_Data * game_data, uint8 i) {
		Character * character = &game_data->characters[i];
		Timer     * timer     = &game_data->characters_timers[i];

		character->position  = map::get_spawn_point(game_data, i);
		character->direction = {0, 0};
		
		timer->period  = 1;
		timer->elapsed = 0;

		// game_data->characters_reverse[i] = false;
	}
}

namespace ai {
	#define AI_GET_ACTION_FUNC(ROUTINE_NAME) Vector2i ROUTINE_NAME(Game_Data * game_data, Mode mode, uint8 character_index, bool finished_move)
	typedef AI_GET_ACTION_FUNC(get_action_func);

	AI_GET_ACTION_FUNC(get_action_none) { return {0, 0}; }

	AI_GET_ACTION_FUNC(get_action_player) {
		// if (!finished_move) { return {0, 0}; } // @Note: always allow user input for responsiveness
		if (mode == Mode::None) { return {0, 0}; }

		int32 input_left  = input::get_current(Keyboard_Keys::Left);
		int32 input_right = input::get_current(Keyboard_Keys::Right);
		int32 input_down  = input::get_current(Keyboard_Keys::Down);
		int32 input_up    = input::get_current(Keyboard_Keys::Up);

		Vector2i input_move = { input_right - input_left, input_up - input_down };

		return input_move;
	}

	AI_GET_ACTION_FUNC(get_action_blinky) {
		if (!finished_move) { return {0, 0}; }
		if (mode == Mode::Scatter) {
			return game_data->map_dimensions * CHARACTER_CORNER_MUL[character_index]
				+ CHARACTER_CORNER_ADD[character_index];
		}

		if (mode == Mode::Chase) {
			uint8 target_index = CHARACTER_TARGETS[character_index];
			Character target = game_data->characters[target_index];
			return target.position;
		}
		
		return game_data->characters[character_index].position;
	}

	AI_GET_ACTION_FUNC(get_action_pinky) {
		if (!finished_move) { return {0, 0}; }
		if (mode == Mode::Scatter) {
			return game_data->map_dimensions * CHARACTER_CORNER_MUL[character_index]
				+ CHARACTER_CORNER_ADD[character_index];
		}

		if (mode == Mode::Chase) {
			static int32 const ambush_offset = 4;
			uint8 target_index = CHARACTER_TARGETS[character_index];
			Character target = game_data->characters[target_index];
			return target.position + target.direction * ambush_offset;
		}
		
		return game_data->characters[character_index].position;
	}

	AI_GET_ACTION_FUNC(get_action_inky) {
		if (!finished_move) { return {0, 0}; }
		if (mode == Mode::Scatter) {
			return game_data->map_dimensions * CHARACTER_CORNER_MUL[character_index]
				+ CHARACTER_CORNER_ADD[character_index];
		}

		if (mode == Mode::Chase) {
			static int32 const pivot_offset = 2;

			uint8 target_index = CHARACTER_TARGETS[character_index];
			uint8 ally_index = CHARACTER_ALLIES[character_index];

			Character target = game_data->characters[target_index];
			Character ally = game_data->characters[ally_index];

			Vector2i pivot = target.position + target.direction * pivot_offset;
			return pivot + (pivot - ally.position);
		}
		
		return game_data->characters[character_index].position;
	}

	AI_GET_ACTION_FUNC(get_action_clyde) {
		if (!finished_move) { return {0, 0}; }
		if (mode == Mode::Scatter) {
			return game_data->map_dimensions * CHARACTER_CORNER_MUL[character_index]
				+ CHARACTER_CORNER_ADD[character_index];
		}

		if (mode == Mode::Chase) {
			static int32 const chase_radius = 8;

			uint8 target_index = CHARACTER_TARGETS[character_index];

			Character character = game_data->characters[character_index];
			Character target = game_data->characters[target_index];

			if (magnitude_squared(target.position - character.position) < chase_radius * chase_radius) {
			return game_data->map_dimensions * CHARACTER_CORNER_MUL[character_index]
				+ CHARACTER_CORNER_ADD[character_index];
			}
			return target.position;
		}
		
		return game_data->characters[character_index].position;
	}
	
	AI_GET_ACTION_FUNC(get_action_pointer) {
		Vector2i pointer = input::get_pointer();
		return pointer / TILE_SIZE;
	}
	#undef AI_GET_ACTION_FUNC
}

namespace ai {	
	#define AI_DO_ACTION_FUNC(ROUTINE_NAME) void ROUTINE_NAME(Game_Data * game_data, Mode mode, uint8 character_index, bool finished_move, Vector2i value)
	typedef AI_DO_ACTION_FUNC(do_action_func);

	AI_DO_ACTION_FUNC(do_action_none) { }

	AI_DO_ACTION_FUNC(do_action_set_position) {
		Character * character = &game_data->characters[character_index];
		value = clamp(value, {0, 0}, game_data->map_dimensions - 1);
		character->position = value;
	}

	AI_DO_ACTION_FUNC(do_action_follow_direction) {
		// if (!finished_move) { return; } // @Note: always allow user input for responsiveness
		if (mode == Mode::None) { return; }

		Character * character = &game_data->characters[character_index];

		if (value != vec2i(0, 0)) {
			Vector2i direction = (character->direction == vec2i(0, 0))
				? utils::get_cardinal_direction(value, value)
				: utils::get_cardinal_direction(value, character->direction);
			if (character->direction == -direction) {
				character::reverse(game_data, character_index);
			}
			else /*if (finished_move)*/ {
				// @Note: allow turns upon midway to another tile,
				// though visually it looks jumpy at intersections
				Vector2i next_position = utils::wrap_position(character->position + direction, game_data->map_dimensions);
				if (map::is_walkable(game_data, next_position)) {
					character->direction = direction;
				}
			}
		}
	}

	AI_DO_ACTION_FUNC(do_action_follow_path) {
		if (!finished_move) { return; }
		if (mode == Mode::None) { return; }

		Character * character = &game_data->characters[character_index];
		Array_Dynamic<Vector2i> path = path_finding::get_path(game_data, character->position, value, character->direction);
		if (path.length > 0) {
			Vector2i to = path[--path.length];
			character->direction = sign(to - character->position);
		}
	}

	AI_DO_ACTION_FUNC(do_action_follow_target) {
		if (!finished_move) { return; }
		if (mode == Mode::None) { return; }

		Character * character = &game_data->characters[character_index];
		Vector2i direction = (mode == Mode::Fright) && (CHARACTER_TEAM[character_index] == 1)
			? path_finding::get_random_direction(game_data, character->position, character->direction)
			: path_finding::get_direction(game_data, character->position, value, character->direction);

		character->direction = direction;
	}
	#undef AI_DO_ACTION_FUNC
}

namespace ai {
	#define AI_MOVE_FUNC(ROUTINE_NAME) void ROUTINE_NAME(Game_Data * game_data, uint8 character_index, bool finished_move)
	typedef AI_MOVE_FUNC(move_func);

	AI_MOVE_FUNC(move_none) { }

	AI_MOVE_FUNC(move_direction) {
		if (!finished_move) { return; }
		Character * character = &game_data->characters[character_index];
		if (character->direction == vec2i(0, 0)) { return; }

		Vector2i next_position = utils::wrap_position(character->position + character->direction, game_data->map_dimensions);
		int32 next_i = next_position.y * game_data->map_dimensions.x + next_position.x;
		if (map::is_walkable(game_data, next_i)) {
			character->position = next_position;
		}
	}
	#undef AI_MOVE_FUNC
}

namespace game {
	Game_Data * allocate_game_memory(int32 tiles_count) {
		auto game_data = globals::allocate_permanent<Game_Data>();
		*game_data = {};

		// characters data
		game_data->characters.set_capacity(CHARACTERS_COUNT, &globals::allocate_permanent);
		game_data->characters_timers.set_capacity(CHARACTERS_COUNT, &globals::allocate_permanent);
		// game_data->characters_reverse.set_capacity(CHARACTERS_COUNT, &globals::allocate_permanent);

		// map data
		game_data->map.set_capacity(tiles_count, &globals::allocate_permanent);
		game_data->spawn_points.set_capacity(SPAWN_POINTS_LIMIT, &globals::allocate_permanent);
		
		return game_data;
	}
	
	bool needs_init() {
		if (globals::permanent_memory->length == 0) { return true; }
		auto game_data = (Game_Data *)globals::permanent_memory->data;
		return memcmp(game_data->checksum, checksum, sizeof(checksum));
	}

	void init() {
		using Map_Prefab = utils::Parsed_Map_Prefab;

		Map_Prefab map_prefab = utils::parse_map_prefab(map_prefab_1);
		utils::flip_y(map_prefab.tiles, map_prefab.width);

		auto game_data = allocate_game_memory((int32)map_prefab.tiles.length);
		memcpy(game_data->checksum, checksum, sizeof(checksum));
		game_data->random_state = (uint32)globals::frame_timestamp;

		{
			Array_Dynamic<Tile> * map = &game_data->map;
			memcpy(map->data, map_prefab.tiles.data, map_prefab.tiles.length * sizeof(Tile));
			map->length = map_prefab.tiles.length;
			game_data->map_dimensions = {
				map_prefab.width,
				(int32)map_prefab.tiles.length / map_prefab.width
			};
			game_data->dots_count = map_prefab.dots_count;
		}

		for (int32 i = 0; i < map_prefab.spawn_tiles.length; ++i) {
			Map_Prefab::Spawn_Tile spawn_tile = map_prefab.spawn_tiles[i];
			game_data->spawn_points.add({
				spawn_tile.team,
				{
					spawn_tile.tile_index % map_prefab.width,
					game_data->map_dimensions.y - (spawn_tile.tile_index / map_prefab.width) - 1
				}
			});
		}

		for (uint8 i = 0; i < CHARACTERS_COUNT; ++i) {
			character::add(game_data);
		}

		for (uint8 i = 0; i < CHARACTERS_COUNT; ++i) {
			character::respawn(game_data, i);
		}
	}
	
	// typedef ai::get_action_func * get_action_func_pointer;
	using get_action_func_pointer = ai::get_action_func *;
	static get_action_func_pointer const type_to_get_action[CHARACTERS_TYPES_COUNT] = {
		&ai::get_action_none,
		&ai::get_action_player,
		&ai::get_action_blinky,
		&ai::get_action_pinky,
		&ai::get_action_inky,
		&ai::get_action_clyde,
		&ai::get_action_pointer,
	};
	
	// typedef ai::do_action_func * do_action_func_pointer;
	using do_action_func_pointer = ai::do_action_func *;
	static do_action_func_pointer const type_to_do_action[CHARACTERS_TYPES_COUNT] = {
		&ai::do_action_none,
		&ai::do_action_follow_direction,
		&ai::do_action_follow_target,
		&ai::do_action_follow_target,
		&ai::do_action_follow_target,
		&ai::do_action_follow_target,
		&ai::do_action_set_position,
	};
	
	// typedef ai::move_func * move_func_pointer;
	using move_func_pointer = ai::move_func *;
	static move_func_pointer const type_to_move[CHARACTERS_TYPES_COUNT] = {
		&ai::move_none,
		&ai::move_direction,
		&ai::move_direction,
		&ai::move_direction,
		&ai::move_direction,
		&ai::move_direction,
		&ai::move_none,
	};

	// mode: chase
	// red    blinky - engage; offset 0 (position of the player)
	// pink   pinky  - ambush; offset 4 (ahead of the player)
	// cyan   inky   - ambush; offset 2 (mirror blinky's position by player's position or something)
	// orange clyde  - defend; offset 8 (engage, but return back to a corner if too close)

	// mode: scatter (flee to the corners every so often)
	// pinky blinky
	// clyde inky

	// mode: frightened (wander around after a pick-up)

	Mode get_mode(Game_Data * game_data, Modes const * level_modes) {
		if (game_data->fright_tile_steps > 0) {
			return Mode::Fright;
		}

		Modes::Config const * modes = level_modes->modes;

		int32 tile_steps = game_data->tile_steps;
		for (uint8 i = 0; i < MODES_COUNT; ++i) {
			tile_steps -= modes[i].tile_steps;
			if (tile_steps < 0) { return modes[i].type; }
		}

		return Mode::Chase;
	}

	bool collect(Game_Data * game_data, int8 level_fright_tile_steps, uint8 character_index) {
		static Tile_Type const to_collect = Tile_Type::Dot | Tile_Type::Energy;
		
		bool is_dot = false;
		Character character = game_data->characters[character_index];

		size_t index = character.position.y * game_data->map_dimensions.x + character.position.x;
		Tile tile = game_data->map[index];

		if (bits_are_set(TILE_FLAGS[tile], Tile_Type::Dot)) {
			game_data->dots_count -= 1;
			is_dot = true;
		}
		
		if (bits_are_set(TILE_FLAGS[tile], Tile_Type::Energy)) {
			game_data->fright_tile_steps += level_fright_tile_steps;
			game_data->dots_count -= 1;
			is_dot = true;
		}
		
		Tile_Type type = bits_to_zero(TILE_FLAGS[tile], to_collect);
		game_data->map[index] = utils::get_tile_by_type(type, tile);

		return is_dot;
	}

	bool collide(Game_Data * game_data, uint8 character_index_a, uint8 character_index_b) {
		Character character_a = game_data->characters[character_index_a];
		Character character_b = game_data->characters[character_index_b];
		return character_a.position == character_b.position;
	}
	
	uint8 get_speed_percent(Speeds const * level_speeds, uint8 character_index, Mode mode, bool is_dot, bool is_slow, uint8 fast) {
		Speeds::Config config = level_speeds->percents[character_index];

		uint8 result = 100;

		if (mode == Mode::Fright) {
			result = is_dot ? config.fright_dot : config.fright;
		}
		else {
			result = is_dot ? config.norm_dot : config.norm;
			result += (config.fast - config.norm) * fast;
		}

		if (is_slow) {
			result = min(result, config.slow);
		}
		
		return result;
	}

	void update(Game_Data * game_data) {
		using Level = utils::Level_Compiled;

		int32 update_period = UPDATE_PERIOD_INTERNAL * (globals::time_precision / TIME_PRECISION);

		Level level = utils::get_level_settings(game_data->level);

		Mode mode = get_mode(game_data, level.modes);

		bool is_mode_change = false;
		if (game_data->mode != mode) {
			game_data->mode = mode;
			is_mode_change = true;

			// for (uint8 i = 0; i < game_data->characters.length; ++i) {
			// 	if (CHARACTER_TEAM[i] == 0) { continue; }
			// 	game_data->characters_reverse[i] = true;
			// }
		}
		
		bool finished_move[CHARACTERS_COUNT] = {};
		for (uint8 i = 0; i < game_data->characters.length; ++i) {
			Timer * timer = &game_data->characters_timers[i];
			timer->elapsed += globals::delta_time;
			if (timer->elapsed < timer->period) { continue; }
			timer->elapsed -= timer->period;
			finished_move[i] = true;
		}

		game_data->tile_time += globals::delta_time;
		if (game_data->tile_time > update_period) {
			game_data->tile_time -= update_period;
			game_data->tile_steps += 1;
			if (game_data->fright_tile_steps > 0) {
				game_data->fright_tile_steps -= 1;
			}
		}
		
		// move characters
		for (uint8 i = 0; i < game_data->characters.length; ++i) {
			uint8 type = CHARACTER_TYPES[i];
			(*game::type_to_move[type])(game_data, i, finished_move[i]);
		}

		// do AI actions:
		// - towards target position or simply set
		// - reverse direction upon game mode change
		for (uint8 i = 0; i < game_data->characters.length; ++i) {
			uint8 type = CHARACTER_TYPES[i];
			Vector2i action = (*game::type_to_get_action[type])(game_data, mode, i, finished_move[i]);
			(*game::type_to_do_action[type])(game_data, mode, i, finished_move[i], action);
		}
		
		// for (uint8 i = 0; i < game_data->characters.length; ++i) {
		// 	if (!finished_move[i]) { continue; }
		// 
		// 	if (!game_data->characters_reverse[i]) { continue; }
		// 	game_data->characters_reverse[i] = false;
		// 
		// 	Character * character = &game_data->characters[i];
		// 	Timer * timer = &game_data->characters_timers[i];
		// 	character->direction = -character->direction;
		// 	timer->elapsed = 0;
		// }
		
		if (is_mode_change) { // @Note: immediate reverse is more responsive
			for (uint8 i = 0; i < game_data->characters.length; ++i) {
				if (CHARACTER_TEAM[i] == 0) { continue; }
				character::reverse(game_data, i);
			}
		}
		
		// update game state:
		// - collect items
		// - process collisions
		bool is_dot[CHARACTERS_COUNT] = {};
		for (uint8 i = 0; i < game_data->characters.length; ++i) {
			if (!finished_move[i]) { continue; }
			if (CHARACTER_TEAM[i] == 1) { continue; }
			
			is_dot[i] = game::collect(game_data, level.fright_tile_steps, i);
		}

		bool was_eaten[CHARACTERS_COUNT] = {};
		uint8 chaser_in_pair = (mode != Mode::Fright);
		for (uint8 i = 0; i < COLLISTION_PAIRS_COUNT; ++i) {
			uint8 chaser_index = COLLISTION_PAIRS[i][chaser_in_pair];
			uint8 target_index = COLLISTION_PAIRS[i][1 - chaser_in_pair];
			was_eaten[target_index] |= game::collide(game_data, chaser_index, target_index);
		}

		// update movement periods
		for (uint8 i = 0; i < game_data->characters.length; ++i) {
			if (!finished_move[i]) { continue; }

			uint8 fast = 0;
			for (uint8 f = 1; f <= CHARACTER_FAST[i]; ++f) {
				if (game_data->dots_count > level.fast_dots_left / f) { break; }
				fast++;
			}

			Character character = game_data->characters[i];
			bool is_slow = map::is_slow(game_data, character.position);
			uint8 speed_percent = game::get_speed_percent(level.speeds, i, mode, is_dot[i], is_slow, fast);

			Timer * timer = &game_data->characters_timers[i];
			timer->period = update_period * 100 / speed_percent;
		}

		// track game over conditions:
		// - teleport away upon being eaten
		// - reset game upon collection of all dots
		for (uint8 i = 0; i < game_data->characters.length; ++i) {
			if (!was_eaten[i]) { continue; }
			character::respawn(game_data, i);
		}
		
		if (game_data->dots_count == 0) { // @Todo: advance between levels
			reset_memory_chunk(globals::permanent_memory);
		}
	}
}
