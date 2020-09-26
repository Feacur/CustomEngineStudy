#pragma once

// namespace custom {

//
// utility
//
static inline void mouse_set(Window * window, custom::Mouse_Code key, bool is_pressed) {
	window->mouse.keys[(u8)key] = is_pressed;
}

static inline void mouse_reset(Window * window) {
	ZeroMemory(window->mouse.prev, sizeof(window->mouse.keys));
	ZeroMemory(window->mouse.keys, sizeof(window->mouse.keys));
}

static inline void mouse_update(Window * window) {
	CopyMemory(window->mouse.prev, window->mouse.keys, sizeof(window->mouse.keys));
	window->mouse.delta = {};
	window->mouse.wheel = {};
}

static void update_current_mouse(Window * window, POINT const & screen, POINT const & client, ivec2 const & window_size) {
	window->mouse.screen = screen;
	window->mouse.client = client;
	window->mouse.position = {
		window->mouse.client.x,
		window->size.y - (window->mouse.client.y + 1)
	};
}

constexpr static inline ivec2 mouse_input_get_delta(Window * window, LONG x, LONG y) {
	return {
		x - window->mouse.screen.x,
		window->mouse.screen.y - y
	};
}

//
// processing
//
#define MOUSE_SET_MESSAGE_VALUE(VALUE, EXPECTED) mouse_set(window, custom::Mouse_Code::VALUE, BITS_ARE_SET(virtual_key_code, EXPECTED))
static void process_message_mouse(Window * window, WPARAM wParam, LPARAM lParam, bool client_space, vec2 const & wheel_mask) {
	// @Note: always process wheel messages input, as I didn't figure out raw input
	window->mouse.wheel.x += wheel_mask.x * GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;
	window->mouse.wheel.y += wheel_mask.y * GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA;

	if (mouse_mode != Input_Mode::Message) { return; }

	POINTS points = MAKEPOINTS(lParam);
	POINT client, screen;
	if (client_space) {
		client.x = points.x; client.y = points.y;
		screen = client;
		ClientToScreen(window->hwnd, &screen);
	}
	else {
		screen.x = points.x; screen.y = points.y;
		client = screen;
		ScreenToClient(window->hwnd, &client);
	}

	window->mouse.delta += mouse_input_get_delta(window, screen.x, screen.y);
	update_current_mouse(window, screen, client, window->size);

	WPARAM virtual_key_code = wParam;
	// GET_XBUTTON_WPARAM(wParam)
	MOUSE_SET_MESSAGE_VALUE(Key1, MK_LBUTTON);
	MOUSE_SET_MESSAGE_VALUE(Key2, MK_MBUTTON);
	MOUSE_SET_MESSAGE_VALUE(Key3, MK_RBUTTON);
	MOUSE_SET_MESSAGE_VALUE(Key4, MK_XBUTTON1);
	MOUSE_SET_MESSAGE_VALUE(Key5, MK_XBUTTON2);
}
#undef MOUSE_SET_MESSAGE_VALUE

#if defined(CUSTOM_FEATURE_RAW_INPUT)
// static bool mouse_is_inside(Window * window) {
// 	return window->mouse.client.x >= 0
// 		&& window->mouse.client.y >= 0
// 		&& window->mouse.client.x <  window->size.x
// 		&& window->mouse.client.y <  window->size.y;
// }

#define MOUSE_TEST_RAW_VALUE(VALUE, EXPECTED, is_pressed)     \
if (BITS_ARE_SET(flags, EXPECTED)) {                          \
    mouse_set(window, custom::Mouse_Code::VALUE, is_pressed); \
}                                                             \

static void raw_input_callback(Window * window, RAWMOUSE const & data) {
	if (mouse_mode != Input_Mode::Raw) { return; }

	POINT screen;
	GetCursorPos(&screen);

	POINT client = screen;
	ScreenToClient(window->hwnd, &client);
	
	if (BITS_ARE_SET(data.usFlags, MOUSE_MOVE_ABSOLUTE)) {
		// @Bug: probably better using screen coords?
		// input_mouse.raw_delta = mouse_input_get_delta(screen.x, screen.y);
		window->mouse.delta += mouse_input_get_delta(window, data.lLastX, data.lLastY);
	}
	else {
		window->mouse.delta += {data.lLastX, -data.lLastY};
	}

	update_current_mouse(window, screen, client, window->size);

	USHORT flags = data.usButtonFlags;
	MOUSE_TEST_RAW_VALUE(Key1, RI_MOUSE_BUTTON_1_DOWN, true);
	MOUSE_TEST_RAW_VALUE(Key2, RI_MOUSE_BUTTON_2_DOWN, true);
	MOUSE_TEST_RAW_VALUE(Key3, RI_MOUSE_BUTTON_3_DOWN, true);
	MOUSE_TEST_RAW_VALUE(Key4, RI_MOUSE_BUTTON_4_DOWN, true);
	MOUSE_TEST_RAW_VALUE(Key5, RI_MOUSE_BUTTON_5_DOWN, true);

	MOUSE_TEST_RAW_VALUE(Key1, RI_MOUSE_BUTTON_1_UP, false);
	MOUSE_TEST_RAW_VALUE(Key2, RI_MOUSE_BUTTON_2_UP, false);
	MOUSE_TEST_RAW_VALUE(Key3, RI_MOUSE_BUTTON_3_UP, false);
	MOUSE_TEST_RAW_VALUE(Key4, RI_MOUSE_BUTTON_4_UP, false);
	MOUSE_TEST_RAW_VALUE(Key5, RI_MOUSE_BUTTON_5_UP, false);

	// @Note: process wheel input as messages, as I didn't figure out raw input
	// if (BITS_ARE_SET(flags, RI_MOUSE_WHEEL)) {
	// 	// @Note: value is signed; alike GET_WHEEL_DELTA_WPARAM
	// 	window->mouse.wheel.x += (r32)(short)HIWORD(data.usButtonData) / WHEEL_DELTA; // @Bug: wrong?
	// 	window->mouse.wheel.y += (r32)(short)LOWORD(data.usButtonData) / WHEEL_DELTA;
	// }
}

#undef MOUSE_TEST_RAW_VALUE
#endif // defined(CUSTOM_FEATURE_RAW_INPUT)

// }
