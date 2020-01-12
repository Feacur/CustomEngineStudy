//
// game
//

namespace game {
	Game_Data * allocate_game_memory() {
		auto game_data = globals::allocate_permanent<Game_Data>();
		*game_data = {};
		
		game_data->entities.set_capacity(ENTITIES_COUNT, globals::allocate_permanent);

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

		for (u8 i = 0; i < ENTITIES_COUNT; ++i) {
			game_data->entities.add({
				{0, 0},
				{1, 0},
				{10, 10}
			});
		}
	}
	
	void update(Game_Data * game_data) {
		r32 delta_time = globals::get_delta_seconds();

		Vector2i pointer = input::get_pointer();
		Vector2 CAMERA_OFFSET = {
			globals::render_buffer.size.x * 0.5f,
			globals::render_buffer.size.y * 0.5f
		};

		Vector2i target_i = pointer - globals::render_buffer.size / 2;
		Vector2 target = {(r32)target_i.x, (r32)target_i.y};
		
		for (u8 i = 0; i < game_data->entities.length; ++i) {
			Entity * entity = &game_data->entities[i];
			Vector2 direction = target - entity->position;
			r32 direction_length = magnitude(direction);
			if (direction_length < 1) {
				entity->acceleration = {0, 0};
			}
			else {
				entity->acceleration = direction * 1000 / direction_length;
			}
		}

		for (u8 i = 0; i < game_data->entities.length; ++i) {
			Entity * entity = &game_data->entities[i];
			entity->velocity *= 0.95f;
		}

		for (u8 i = 0; i < game_data->entities.length; ++i) {
			Entity * entity = &game_data->entities[i];
			if (magnitude_squared(entity->velocity) < 0.01f) {
				entity->velocity = {0, 0};
			}
		}

		for (u8 i = 0; i < game_data->entities.length; ++i) {
			Entity * entity = &game_data->entities[i];
			entity->velocity += entity->acceleration * delta_time;
			entity->position += entity->velocity * delta_time;
		}

		// stable cosine/sine:
		// static r32 c = 50;
		// static r32 s = 0;
		// r32 trig_delta = 10 * delta_time;
		// c -= s * trig_delta;
		// s += c * trig_delta;
		// Entity * entity2 = &game_data->entities[1];
		// entity2->position = {c, s};
	}
}
