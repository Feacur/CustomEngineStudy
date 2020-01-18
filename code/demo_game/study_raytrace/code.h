namespace game {
	Game_Data * allocate_game_memory() {
		auto game_data = globals::allocate_permanent<Game_Data>();
		*game_data = {};
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
	}
}
