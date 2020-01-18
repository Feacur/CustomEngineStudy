namespace globals {
	static Memory_Chunk * permanent_memory;
	static Memory_Chunk * transient_memory;

	static Input_Keyboard * input_keyboard;
	static Input_Pointer  * input_pointer;

	static RGBA_Data  render_buffer;
	static Image_Data render_buffer_f;

	static s32  delta_time;
	static s64  uptime;
	static u64 frame_timestamp;

	static thread_local u32 random_state;
	
	static s64 const time_precision = 1000000;
}

//
// funcitons
//

namespace input {
	inline bool get_current(Keyboard_Keys key) {
		return keyboard_get_current_state(globals::input_keyboard, key);
	}

	inline bool get_pressed(Keyboard_Keys key) {
		return keyboard_get_transition_pressed(globals::input_keyboard, key);
	}

	inline bool get_released(Keyboard_Keys key) {
		return keyboard_get_transition_released(globals::input_keyboard, key);
	}
}

namespace input {
	inline bool get_current(Pointer_Keys key) {
		return pointer_get_current_state(globals::input_pointer, key);
	}

	inline bool get_pressed(Pointer_Keys key) {
		return pointer_get_transition_pressed(globals::input_pointer, key);
	}

	inline bool get_released(Pointer_Keys key) {
		return pointer_get_transition_released(globals::input_pointer, key);
	}

	inline Vector2i get_pointer() {
		return globals::input_pointer->pixel_position;
	}

	inline Vector2i get_pointer_delta() {
		return globals::input_pointer->raw_delta;
	}
}

namespace globals {
	template<typename T>
	T * allocate_permanent() {
		return (T*)allocate_data(globals::permanent_memory, sizeof(T));
	}

	template<typename T>
	T * allocate_transient() {
		return (T*)allocate_data(globals::transient_memory, sizeof(T));
	}

	template<typename T>
	T * allocate_permanent(size_t count) {
		return (T*)allocate_data(globals::permanent_memory, sizeof(T) * count);
	}

	template<typename T>
	T * allocate_transient(size_t count) {
		return (T*)allocate_data(globals::transient_memory, sizeof(T) * count);
	}
}

namespace globals {
	void cache(Platform_Data *platform_data) {
		globals::permanent_memory = &platform_data->permanent_memory;
		globals::transient_memory = &platform_data->transient_memory;
		
		globals::input_keyboard   = &platform_data->input_keyboard;
		globals::input_pointer    = &platform_data->input_pointer;

		globals::render_buffer    = platform_data->render_buffer_image;
		globals::render_buffer_f  = platform_data->render_buffer_image_f;

		globals::delta_time       = (s32)mul_div(platform_data->time.last_frame_duration, time_precision, platform_data->time.precision);
		globals::uptime           = mul_div(platform_data->time.since_start, time_precision, platform_data->time.precision);
		globals::frame_timestamp  = platform_data->time.frame_timestamp;
		
		reset_memory_chunk(globals::transient_memory);

		if (input::get_current(Keyboard_Keys::Alt) && input::get_pressed(Keyboard_Keys::Enter)) {
			platform_data->render_settings.fullscreen = !platform_data->render_settings.fullscreen;
			globals::render_buffer_f.exposure = 0;
			clear_buffer(globals::render_buffer_f, {0, 0, 0, 0});
		}
		
		if (input::get_current(Keyboard_Keys::Alt) && input::get_pressed(Keyboard_Keys::F4)) {
			platform_data->keep_alive = false;
		}

		if (platform_data->shutdown_request) {
			platform_data->keep_alive = false;
		}
	}

	inline r32 get_delta_seconds() {
		return (r32)delta_time / time_precision;
	}
}

//
// initialization
//

namespace game {
	bool needs_init();
	void init();
}

struct Game_Data;
Game_Data * get_game_data() {
	if (game::needs_init()) {
		reset_memory_chunk(globals::permanent_memory);
		game::init();
		
		globals::render_buffer_f.exposure = 0;
		clear_buffer(globals::render_buffer_f, {0, 0, 0, 0});
	}
	// the "header" of platform data is game data
	return (Game_Data *)globals::permanent_memory->data;
}
