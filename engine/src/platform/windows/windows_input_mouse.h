#pragma once

static POINT os_input_mouse_screen;
static POINT os_input_mouse_client;

static ivec2 get_mouse_window(POINT point) {
	return {point.x, window_size.y - (point.y + 1)};
}

static bool mouse_is_inside() {
	return os_input_mouse_client.x >= 0
		&& os_input_mouse_client.y >= 0
		&& os_input_mouse_client.x <  window_size.x
		&& os_input_mouse_client.y <  window_size.y;
}

// namespace custom
// {
	enum struct Mouse_Mode {
		Raw,
		Message,
	};
	UNDERLYING_TYPE_META(Mouse_Mode, u8)
	IS_ENUM_META(Mouse_Mode)

	static Mouse_Mode mouse_position_mode = Mouse_Mode::Raw;
	static Mouse_Mode mouse_keys_mode     = Mouse_Mode::Raw;

	//
	// state
	//
	struct Input_Mouse {
		ivec2 pixel_position;
		ivec2 raw_delta;
		vec2 wheel;
		s8 was_pressed[(u8)custom::Mouse_Code::Last];
		s8 is_pressed[(u8)custom::Mouse_Code::Last];
	};
	static Input_Mouse input_mouse;
	static bool input_mouse_enable = true;

	// static void mouse_reset_state() {
	// 	static u8 count = (u8)custom::Mouse_Code::Last;
	// 	static size_t bytes = count * sizeof(custom::Mouse_Code);
	// 	memset(input_mouse.was_pressed, 0, bytes);
	// 	memset(input_mouse.is_pressed, 0, bytes);
	// }

	// static void mouse_update_previous_state() {
	// 	input_mouse.raw_delta = {};
	// 	input_mouse.wheel = {};
	// 	static u8 count = (u8)custom::Mouse_Code::Last;
	// 	static size_t bytes = count * sizeof(custom::Mouse_Code);
	// 	memcpy(input_mouse.was_pressed, input_mouse.is_pressed, bytes);
	// }

	//
	// utility
	//
	static inline void mouse_set_key(custom::Mouse_Code key, bool is_pressed) {
		input_mouse.is_pressed[(s32)key] = is_pressed;
	}

	static void update_current_mouse(POINT const & screen_position, POINT const & client_position) {
		os_input_mouse_screen = screen_position;
		os_input_mouse_client = client_position;
		input_mouse.pixel_position = // transform_window_to_render(
			get_mouse_window(os_input_mouse_client)
		;//);
	}

	static constexpr inline ivec2 mouse_input_get_delta(LONG x, LONG y) {
		return {
			x - os_input_mouse_screen.x,
			os_input_mouse_screen.y - y
		};
	}

	//
	// processing
	//
	#define MOUSE_SET_MESSAGE_VALUE(VALUE, EXPECTED) mouse_set_key(custom::Mouse_Code::VALUE, BITS_ARE_SET(virtual_key_code, EXPECTED))
	static void process_message_mouse(HWND window, WPARAM wParam, LPARAM lParam) {
		static Mouse_Mode const mode = Mouse_Mode::Message;
		
		if (mouse_position_mode == mode) {
			auto points = MAKEPOINTS(lParam);
			POINT client_position;
			client_position.x = points.x;
			client_position.y = points.y;
			
			POINT screen_position = client_position;
			ClientToScreen(window, &screen_position);

			input_mouse.raw_delta = mouse_input_get_delta(screen_position.x, screen_position.y);
			update_current_mouse(screen_position, client_position);
		}

		if (mouse_keys_mode == mode) {
			WPARAM virtual_key_code = wParam;
			MOUSE_SET_MESSAGE_VALUE(Key1, MK_LBUTTON);
			MOUSE_SET_MESSAGE_VALUE(Key2, MK_MBUTTON);
			MOUSE_SET_MESSAGE_VALUE(Key3, MK_RBUTTON);
		}
	}
	#undef MOUSE_SET_MESSAGE_VALUE

	#if defined(CUSTOM_FEATURE_RAW_INPUT)
	#define MOUSE_TEST_RAW_VALUE(VALUE, EXPECTED, is_pressed)\
	if (BITS_ARE_SET(flags, EXPECTED)) {\
		mouse_set_key(custom::Mouse_Code::VALUE, is_pressed);\
	}

	static void raw_input_callback(HWND window, RAWMOUSE const & data) {
		static Mouse_Mode const mode = Mouse_Mode::Raw;
		
		if (mouse_position_mode == mode) {
			POINT screen_position;
			GetCursorPos(&screen_position);

			POINT client_position = screen_position;
			ScreenToClient(window, &client_position);
			
			if (BITS_ARE_SET(data.usFlags, MOUSE_MOVE_ABSOLUTE)) {
				// @Bug: probably better using screen coords
				// input_mouse.raw_delta = mouse_input_get_delta(screen_position.x, screen_position.y);
				input_mouse.raw_delta = mouse_input_get_delta(data.lLastX, data.lLastY);
			}
			else {
				input_mouse.raw_delta = {data.lLastX, -data.lLastY};
			}

			update_current_mouse(screen_position, client_position);
		}

		if (mouse_keys_mode == mode) {
			USHORT flags = data.usButtonFlags;
			if (mouse_is_inside()) {
				MOUSE_TEST_RAW_VALUE(Key1, RI_MOUSE_BUTTON_1_DOWN, true);
				MOUSE_TEST_RAW_VALUE(Key2, RI_MOUSE_BUTTON_2_DOWN, true);
				MOUSE_TEST_RAW_VALUE(Key3, RI_MOUSE_BUTTON_3_DOWN, true);
			}
			MOUSE_TEST_RAW_VALUE(Key1, RI_MOUSE_BUTTON_1_UP, false);
			MOUSE_TEST_RAW_VALUE(Key2, RI_MOUSE_BUTTON_2_UP, false);
			MOUSE_TEST_RAW_VALUE(Key3, RI_MOUSE_BUTTON_3_UP, false);
		}
	}

	#undef MOUSE_TEST_RAW_VALUE
	#endif // defined(CUSTOM_FEATURE_RAW_INPUT)
// }
