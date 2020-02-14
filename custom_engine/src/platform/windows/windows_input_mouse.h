#pragma once

// namespace custom {

//
// utility
//
static inline void mouse_set(Window * window, custom::Mouse_Code key, bool is_pressed) {
	if (is_pressed) {
		if (window->mouse.keys[(u8)key] != custom::Key_State::Pressed) {
			window->mouse.keys[(u8)key] = custom::Key_State::Pressed;
		}
		else {
			window->mouse.keys[(u8)key] = custom::Key_State::Repeated;
		}
	}
	else {
		window->mouse.keys[(u8)key] = custom::Key_State::Released;
	}
}

static void update_current_mouse(Window * window, POINT const & screen_position, POINT const & client_position, ivec2 const & window_size) {
	window->mouse.screen = screen_position;
	window->mouse.client = client_position;
	window->mouse.position = {
		window->mouse.client.x,
		window->size.y - (window->mouse.client.y + 1)
	};
}

static constexpr inline ivec2 mouse_input_get_delta(Window * window, LONG x, LONG y) {
	return {
		x - window->mouse.screen.x,
		window->mouse.screen.y - y
	};
}

//
// processing
//
#define MOUSE_SET_MESSAGE_VALUE(VALUE, EXPECTED) mouse_set(window, custom::Mouse_Code::VALUE, BITS_ARE_SET(virtual_key_code, EXPECTED))
static void process_message_mouse(Window * window, WPARAM wParam, LPARAM lParam) {
	static Input_Mode const mode = Input_Mode::Message;
	
	if (mouse_pos_mode == mode) {
		POINTS points = MAKEPOINTS(lParam);
		POINT client_position;
		client_position.x = points.x;
		client_position.y = points.y;
		
		POINT screen_position = client_position;
		ClientToScreen(window->hwnd, &screen_position);

		window->mouse.delta = mouse_input_get_delta(window, screen_position.x, screen_position.y);
		update_current_mouse(window, screen_position, client_position, window->size);
	}

	if (mouse_keys_mode == mode) {
		WPARAM virtual_key_code = wParam;
		// GET_XBUTTON_WPARAM(wParam)
		MOUSE_SET_MESSAGE_VALUE(Key1, MK_LBUTTON);
		MOUSE_SET_MESSAGE_VALUE(Key2, MK_MBUTTON);
		MOUSE_SET_MESSAGE_VALUE(Key3, MK_RBUTTON);
		MOUSE_SET_MESSAGE_VALUE(Key4, MK_XBUTTON1);
		MOUSE_SET_MESSAGE_VALUE(Key5, MK_XBUTTON2);
	}
}
#undef MOUSE_SET_MESSAGE_VALUE

#if defined(CUSTOM_FEATURE_RAW_INPUT)
static bool mouse_is_inside(Window * window) {
	return window->mouse.client.x >= 0
		&& window->mouse.client.y >= 0
		&& window->mouse.client.x <  window->size.x
		&& window->mouse.client.y <  window->size.y;
}

#define MOUSE_TEST_RAW_VALUE(VALUE, EXPECTED, is_pressed)\
if (BITS_ARE_SET(flags, EXPECTED)) {\
	mouse_set(window, custom::Mouse_Code::VALUE, is_pressed);\
}

static void raw_input_callback(Window * window, RAWMOUSE const & data) {
	static Input_Mode const mode = Input_Mode::Raw;
	
	if (mouse_pos_mode == mode) {
		POINT screen_position;
		GetCursorPos(&screen_position);

		POINT client_position = screen_position;
		ScreenToClient(window->hwnd, &client_position);
		
		if (BITS_ARE_SET(data.usFlags, MOUSE_MOVE_ABSOLUTE)) {
			// @Bug: probably better using screen coords?
			// input_mouse.raw_delta = mouse_input_get_delta(screen_position.x, screen_position.y);
			window->mouse.delta = mouse_input_get_delta(window, data.lLastX, data.lLastY);
		}
		else {
			window->mouse.delta = {data.lLastX, -data.lLastY};
		}

		update_current_mouse(window, screen_position, client_position, window->size);
	}

	if (mouse_keys_mode == mode) {
		USHORT flags = data.usButtonFlags;
		if (mouse_is_inside(window)) {
			MOUSE_TEST_RAW_VALUE(Key1, RI_MOUSE_BUTTON_1_DOWN, true);
			MOUSE_TEST_RAW_VALUE(Key2, RI_MOUSE_BUTTON_2_DOWN, true);
			MOUSE_TEST_RAW_VALUE(Key3, RI_MOUSE_BUTTON_3_DOWN, true);
			MOUSE_TEST_RAW_VALUE(Key4, RI_MOUSE_BUTTON_4_DOWN, true);
			MOUSE_TEST_RAW_VALUE(Key5, RI_MOUSE_BUTTON_5_DOWN, true);
		}
		MOUSE_TEST_RAW_VALUE(Key1, RI_MOUSE_BUTTON_1_UP, false);
		MOUSE_TEST_RAW_VALUE(Key2, RI_MOUSE_BUTTON_2_UP, false);
		MOUSE_TEST_RAW_VALUE(Key3, RI_MOUSE_BUTTON_3_UP, false);
		MOUSE_TEST_RAW_VALUE(Key4, RI_MOUSE_BUTTON_4_UP, false);
		MOUSE_TEST_RAW_VALUE(Key5, RI_MOUSE_BUTTON_5_UP, false);
	}
}

#undef MOUSE_TEST_RAW_VALUE
#endif // defined(CUSTOM_FEATURE_RAW_INPUT)

// }
