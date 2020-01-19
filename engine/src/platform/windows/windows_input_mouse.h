#pragma once

static POINT os_input_pointer_screen;
static POINT os_input_pointer_client;

inline ivec2 get_pointer_window(POINT point) {
	return {point.x, window_size.y - (point.y + 1)};
}

inline bool pointer_is_inside() {
	return os_input_pointer_client.x >= 0
		&& os_input_pointer_client.y >= 0
		&& os_input_pointer_client.x <  window_size.x
		&& os_input_pointer_client.y <  window_size.y;
}

// namespace custom
// {
	enum struct Pointer_Mode {
		Message,
		Direct,
		Raw,
	};
	UNDERLYING_TYPE_META(Pointer_Mode, u8)
	IS_ENUM_META(Pointer_Mode)

	struct Input_Pointer {
		ivec2 pixel_position;
		ivec2 raw_delta;
		vec2 wheel;
		s8 was_pressed[(u8)custom::Mouse_Code::Last];
		s8 is_pressed[(u8)custom::Mouse_Code::Last];
	};

	static Input_Pointer input_pointer;
	static bool input_pointer_enable = true;

	static Pointer_Mode pointer_position_mode = Pointer_Mode::Raw;
	static Pointer_Mode pointer_keys_mode     = Pointer_Mode::Raw;

	inline void pointer_set_key(custom::Mouse_Code key, bool is_pressed) {
		input_pointer.is_pressed[(s32)key] = is_pressed;
	}

	inline void pointer_reset_state() {
		static u8 count = (u8)custom::Mouse_Code::Last;
		static size_t bytes = count * sizeof(custom::Mouse_Code);
		memset(input_pointer.was_pressed, 0, bytes);
		memset(input_pointer.is_pressed, 0, bytes);
	}

	inline void pointer_update_previous_state() {
		input_pointer.raw_delta = {};
		input_pointer.wheel = {};
		static u8 count = (u8)custom::Mouse_Code::Last;
		static size_t bytes = count * sizeof(custom::Mouse_Code);
		memcpy(input_pointer.was_pressed, input_pointer.is_pressed, bytes);
	}

	#define POINTER_SET_MESSAGE_VALUE(VALUE, EXPECTED)\
	pointer_set_key(custom::Mouse_Code::VALUE, BITS_ARE_SET(virtual_key_code, EXPECTED))

	#define POINTER_TEST_RAW_VALUE(VALUE, EXPECTED, is_pressed)\
	if (BITS_ARE_SET(flags, EXPECTED)) {\
		pointer_set_key(custom::Mouse_Code::VALUE, is_pressed);\
	}

	#define POINTER_SET_DIRECT_VALUE(VALUE, EXPECTED)\
	pointer_set_key(custom::Mouse_Code::VALUE, BITS_ARE_SET(GetKeyState(EXPECTED), 0x4000))

	void pointer_set_message_key(WPARAM virtual_key_code) {
		POINTER_SET_MESSAGE_VALUE(Key1, MK_LBUTTON);
		POINTER_SET_MESSAGE_VALUE(Key2, MK_MBUTTON);
		POINTER_SET_MESSAGE_VALUE(Key3, MK_RBUTTON);
	}

	void pointer_set_raw_key(USHORT flags) {
		if (pointer_is_inside()) {
			POINTER_TEST_RAW_VALUE(Key1, RI_MOUSE_BUTTON_1_DOWN, true);
			POINTER_TEST_RAW_VALUE(Key2, RI_MOUSE_BUTTON_2_DOWN, true);
			POINTER_TEST_RAW_VALUE(Key3, RI_MOUSE_BUTTON_3_DOWN, true);
		}
		POINTER_TEST_RAW_VALUE(Key1, RI_MOUSE_BUTTON_1_UP, false);
		POINTER_TEST_RAW_VALUE(Key2, RI_MOUSE_BUTTON_2_UP, false);
		POINTER_TEST_RAW_VALUE(Key3, RI_MOUSE_BUTTON_3_UP, false);
	}

	void pointer_set_direct_key() {
		POINTER_SET_DIRECT_VALUE(Key1, VK_LBUTTON);
		POINTER_SET_DIRECT_VALUE(Key2, VK_MBUTTON);
		POINTER_SET_DIRECT_VALUE(Key3, VK_RBUTTON);
	}

	void update_current_pointer(POINT const & screen_position, POINT const & client_position) {
		os_input_pointer_screen = screen_position;
		os_input_pointer_client = client_position;
		input_pointer.pixel_position = // transform_window_to_render(
			get_pointer_window(os_input_pointer_client)
		;//);
	}

	void update_raw_delta_pointer(LONG x, LONG y) {
		input_pointer.raw_delta = {
			x - os_input_pointer_screen.x,
			os_input_pointer_screen.y - y
		};
	}

	void process_message_mouse(HWND window, WPARAM wParam, LPARAM lParam) {
		static Pointer_Mode const mode = Pointer_Mode::Message;
		
		if (pointer_position_mode == mode) {
			auto points = MAKEPOINTS(lParam);

			POINT client_position;
			client_position.x = points.x;
			client_position.y = points.y;
			
			POINT screen_position = client_position;
			ClientToScreen(window, &screen_position);

			update_raw_delta_pointer(screen_position.x, screen_position.y);
			update_current_pointer(screen_position, client_position);
		}

		if (pointer_keys_mode == mode) {
			pointer_set_message_key(wParam);
		}
	}

	void pointer_input_direct(HWND window) {
		static Pointer_Mode const mode = Pointer_Mode::Direct;
		
		if (pointer_position_mode == mode) {
			POINT screen_position;
			GetCursorPos(&screen_position);
			POINT client_position = screen_position;
			ScreenToClient(window, &client_position);

			update_raw_delta_pointer(screen_position.x, screen_position.y);
			update_current_pointer(screen_position, client_position);
		}

		if (pointer_keys_mode == mode) {
			pointer_set_direct_key();
		}
	}
	
	static void raw_input_callback(HWND window, RAWMOUSE const & data) {
		static Pointer_Mode const mode = Pointer_Mode::Raw;
		
		if (pointer_position_mode == mode) {
			POINT screen_position;
			GetCursorPos(&screen_position);
			POINT client_position = screen_position;
			ScreenToClient(window, &client_position);
			
			if (BITS_ARE_SET(data.usFlags, MOUSE_MOVE_ABSOLUTE)) {
				update_raw_delta_pointer(data.lLastX, data.lLastY);
			}
			else {
				input_pointer.raw_delta = {data.lLastX, -data.lLastY};
			}

			update_current_pointer(screen_position, client_position);
		}

		if (pointer_keys_mode == mode) {
			pointer_set_raw_key(data.usButtonFlags);
		}
	}
// }
