#pragma once

// namespace custom {
enum struct Keyboard_Mode {
	Raw,
	Message,
};
UNDERLYING_TYPE_META(Keyboard_Mode, u8)
IS_ENUM_META(Keyboard_Mode)

static Keyboard_Mode keyboard_mode = Keyboard_Mode::Raw;

//
// state
//
struct Input_Keyboard {
	s8 was_pressed[(u8)custom::Key_Code::Last];
	s8 is_pressed[(u8)custom::Key_Code::Last];
};
static Input_Keyboard input_keyboard;
static bool input_keyboard_enable = true;

// static void keyboard_reset_state() {
// 	static u8 count = (u8)custom::Key_Code::Last;
// 	static size_t bytes = count * sizeof(custom::Key_Code);
// 	memset(input_keyboard.was_pressed, 0, bytes);
// 	memset(input_keyboard.is_pressed, 0, bytes);
// }

// static void keyboard_update_previous_state() {
// 	static u8 count = (u8)custom::Key_Code::Last;
// 	static size_t bytes = count * sizeof(custom::Key_Code);
// 	memcpy(input_keyboard.was_pressed, input_keyboard.is_pressed, bytes);
// }

//
// utility
//
static bool key_test_range(
	custom::Key_Code base,
	bool is_pressed,
	WPARAM virtual_key_code,
	WPARAM min_code,
	WPARAM max_code
) {
	if (virtual_key_code < min_code) { return false; }
	if (virtual_key_code > max_code) { return false; }

	s32 offset = (s32)(virtual_key_code - min_code);
	s32 index = (s32)base + offset;
	input_keyboard.is_pressed[index] = is_pressed;
	return true;
}

static bool key_test_value(
	custom::Key_Code key,
	bool is_pressed,
	WPARAM virtual_key_code,
	WPARAM expected_code
) {
	if (virtual_key_code != expected_code) { return false; }
	input_keyboard.is_pressed[(s32)key] = is_pressed;
	return true;
}

#define KEY_TEST_RANGE(VALUE_BASE, MIN, MAX)\
if (key_test_range(custom::Key_Code::VALUE_BASE, is_pressed, virtual_key_code, MIN, MAX)) { return; }

#define KEY_TEST_VALUE(VALUE, EXPECTED)\
if (key_test_value(custom::Key_Code::VALUE, is_pressed, virtual_key_code, EXPECTED)) { return; }

static void keyboard_set_key(WPARAM virtual_key_code, bool is_pressed) {
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
	// https://docs.microsoft.com/ru-ru/windows/win32/inputdev/about-keyboard-input
	if (keyboard_mode == Keyboard_Mode::Message) {
		// WORD repeats = LOWORD(lParam);
		WORD flag = HIWORD(lParam);
		// bool is_extended = BITS_ARE_SET(flag, KF_EXTENDED);
		// bool is_dialogue = BITS_ARE_SET(flag, KF_DLGMODE);
		// bool is_menu     = BITS_ARE_SET(flag, KF_MENUMODE);
		// bool is_alt      = BITS_ARE_SET(flag, KF_ALTDOWN);
		bool was_pressed = BITS_ARE_SET(flag, KF_REPEAT);
		bool is_released = BITS_ARE_SET(flag, KF_UP);


		bool is_transition      = (was_pressed == is_released);

		keyboard_set_key(wParam, !is_released);
	}
}

#if defined(CUSTOM_FEATURE_RAW_INPUT)
static void raw_input_callback(Window * window, RAWKEYBOARD const & data) {
	if (keyboard_mode == Keyboard_Mode::Raw) {
		// bool scan_e0_prefix = BITS_ARE_SET(data.Flags, RI_KEY_E0);
		// bool scan_e1_prefix = BITS_ARE_SET(data.Flags, RI_KEY_E1);
		bool was_pressed    = BITS_ARE_SET(data.Flags, RI_KEY_MAKE);
		bool is_released    = BITS_ARE_SET(data.Flags, RI_KEY_BREAK);
		
		bool is_transition  = (was_pressed == is_released);
		
		keyboard_set_key(data.VKey, !is_released);
	}
}
#endif // defined(CUSTOM_FEATURE_RAW_INPUT)

// }
