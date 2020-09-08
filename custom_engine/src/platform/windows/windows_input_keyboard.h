#pragma once

// namespace custom {

//
// utility
//
static inline void keyboard_set(Window * window, custom::Key_Code key, bool is_pressed) {
	using U = meta::underlying_type<custom::Key_Code>::type;
	window->keyboard.keys[(U)key] = is_pressed;
}

static inline void keyboard_reset(Window * window) {
	ZeroMemory(window->keyboard.prev, sizeof(window->keyboard.keys));
	ZeroMemory(window->keyboard.keys, sizeof(window->keyboard.keys));
}

static inline void keyboard_update(Window * window) {
	CopyMemory(window->keyboard.prev, window->keyboard.keys, sizeof(window->keyboard.keys));
}

static bool key_test_range(
	Window * window,
	custom::Key_Code base,
	bool is_pressed,
	WPARAM virtual_key_code,
	WPARAM min_code,
	WPARAM max_code
) {
	using U = meta::underlying_type<custom::Key_Code>::type;
	if (virtual_key_code < min_code) { return false; }
	if (virtual_key_code > max_code) { return false; }
	U offset = (U)(virtual_key_code - min_code);
	U index = (U)base + offset;
	keyboard_set(window, (custom::Key_Code)index, is_pressed);
	return true;
}

static bool key_test_value(
	Window * window,
	custom::Key_Code key,
	bool is_pressed,
	WPARAM virtual_key_code,
	WPARAM expected_code
) {
	if (virtual_key_code != expected_code) { return false; }
	keyboard_set(window, key, is_pressed);
	return true;
}

#define KEY_TEST_RANGE(VALUE_BASE, MIN, MAX) if (key_test_range(window, custom::Key_Code::VALUE_BASE, is_pressed, virtual_key_code, MIN, MAX)) { return; }
#define KEY_TEST_VALUE(VALUE, EXPECTED) if (key_test_value(window, custom::Key_Code::VALUE, is_pressed, virtual_key_code, EXPECTED)) { return; }

static void keyboard_set_key(Window * window, WPARAM virtual_key_code, bool is_pressed) {
	KEY_TEST_RANGE(D0,      '0', '9')
	KEY_TEST_RANGE(F1,      VK_F1, VK_F12)
	KEY_TEST_RANGE(A,       'A', 'Z')

	KEY_TEST_VALUE(Left,    VK_LEFT)
	KEY_TEST_VALUE(Right,   VK_RIGHT)
	KEY_TEST_VALUE(Down,    VK_DOWN)
	KEY_TEST_VALUE(Up,      VK_UP)
	
	KEY_TEST_VALUE(Escape,  VK_ESCAPE)
	KEY_TEST_VALUE(Enter,   VK_RETURN)
	KEY_TEST_VALUE(Control, VK_CONTROL)
	KEY_TEST_VALUE(Shift,   VK_SHIFT)
	KEY_TEST_VALUE(Alt,     VK_MENU)
	KEY_TEST_VALUE(Space,   VK_SPACE)
	KEY_TEST_VALUE(Tab,     VK_TAB)
}

#undef KEY_TEST_RANGE
#undef KEY_TEST_VALUE

//
// processing
//
static void process_message_keyboard(Window * window, WPARAM wParam, LPARAM lParam) {
	if (keyboard_mode != Input_Mode::Message) { return; }

	// https://docs.microsoft.com/ru-ru/windows/win32/inputdev/about-keyboard-input
	// WORD repeats = LOWORD(lParam);
	WORD flag = HIWORD(lParam);
	// bool is_extended = BITS_ARE_SET(flag, KF_EXTENDED);
	// bool is_dialogue = BITS_ARE_SET(flag, KF_DLGMODE);
	// bool is_menu     = BITS_ARE_SET(flag, KF_MENUMODE);
	// bool is_alt      = BITS_ARE_SET(flag, KF_ALTDOWN);
	// bool was_pressed = BITS_ARE_SET(flag, KF_REPEAT);
	bool is_released = BITS_ARE_SET(flag, KF_UP);

	// bool is_transition      = (was_pressed == is_released);

	keyboard_set_key(window, wParam, !is_released);
}

#if defined(CUSTOM_FEATURE_RAW_INPUT)
static void raw_input_callback(Window * window, RAWKEYBOARD const & data) {
	if (keyboard_mode != Input_Mode::Raw) { return; }

	// bool scan_e0_prefix = BITS_ARE_SET(data.Flags, RI_KEY_E0);
	// bool scan_e1_prefix = BITS_ARE_SET(data.Flags, RI_KEY_E1);
	// bool was_pressed    = BITS_ARE_SET(data.Flags, RI_KEY_MAKE);
	bool is_released    = BITS_ARE_SET(data.Flags, RI_KEY_BREAK);
	
	// bool is_transition  = (was_pressed == is_released);
	
	keyboard_set_key(window, data.VKey, !is_released);
}
#endif // defined(CUSTOM_FEATURE_RAW_INPUT)

// }
