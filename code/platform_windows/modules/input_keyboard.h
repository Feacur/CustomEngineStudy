enum struct Keyboard_Mode {
	Message,
	Raw,
};
UNDERLYING_TYPE_META(Keyboard_Mode, uint8)
IS_ENUM_META(Keyboard_Mode)

static Input_Keyboard input_keyboard;
static bool input_keyboard_enable = true;

static Keyboard_Mode keyboard_mode = Keyboard_Mode::Raw;

bool keyboard_test_range(
	Keyboard_Keys base,
	bool is_pressed,
	WPARAM virtual_key_code,
	WPARAM min_code,
	WPARAM max_code
) {
	if (virtual_key_code < min_code) { return false; }
	if (virtual_key_code > max_code) { return false; }

	auto offset = (int32)(virtual_key_code - min_code);
	int32 index = (int32)base + offset;
	input_keyboard.is_pressed[index] = is_pressed;
	return true;
}
#define KEYBOARD_TEST_RANGE(VALUE_BASE, MIN, MAX)\
if (keyboard_test_range(Keyboard_Keys::VALUE_BASE, is_pressed, virtual_key_code, MIN, MAX)) { return; }

bool keyboard_test_value(
	Keyboard_Keys key,
	bool is_pressed,
	WPARAM virtual_key_code,
	WPARAM expected_code
) {
	if (virtual_key_code != expected_code) { return false; }
	input_keyboard.is_pressed[(int32)key] = is_pressed;
	return true;
}

#define KEYBOARD_TEST_VALUE(VALUE, EXPECTED)\
if (keyboard_test_value(Keyboard_Keys::VALUE, is_pressed, virtual_key_code, EXPECTED)) { return; }

void keyboard_set_key(WPARAM virtual_key_code, bool is_pressed) {
	KEYBOARD_TEST_RANGE(N0,      '0', '9')
	KEYBOARD_TEST_RANGE(F1,      VK_F1, VK_F12)
	KEYBOARD_TEST_RANGE(A,       'A', 'Z')

	KEYBOARD_TEST_VALUE(Left,    VK_LEFT)
	KEYBOARD_TEST_VALUE(Right,   VK_RIGHT)
	KEYBOARD_TEST_VALUE(Down,    VK_DOWN)
	KEYBOARD_TEST_VALUE(Up,      VK_UP)
	
	KEYBOARD_TEST_VALUE(Escape,  VK_ESCAPE)
	KEYBOARD_TEST_VALUE(Enter,   VK_RETURN)
	KEYBOARD_TEST_VALUE(Control, VK_CONTROL)
	KEYBOARD_TEST_VALUE(Shift,   VK_SHIFT)
	KEYBOARD_TEST_VALUE(Alt,     VK_MENU)
	KEYBOARD_TEST_VALUE(Space,   VK_SPACE)
	KEYBOARD_TEST_VALUE(Tab,     VK_TAB)
}

inline void keyboard_reset_state() {
	memset(input_keyboard.was_pressed, 0, KEYBOARD_KEYS_BYTES);
	memset(input_keyboard.is_pressed, 0, KEYBOARD_KEYS_BYTES);
}

inline void keyboard_update_previous_state() {
	memcpy(input_keyboard.was_pressed, input_keyboard.is_pressed, KEYBOARD_KEYS_BYTES);
}

void process_message_keyboard(HWND window, WPARAM wParam, LPARAM lParam) {
	if (keyboard_mode == Keyboard_Mode::Message) {
		bool is_an_extended_key = get_bit_at_index(lParam, 24);
		bool alt_key_is_pressed = get_bit_at_index(lParam, 29);
		bool was_pressed        = get_bit_at_index(lParam, 30);
		bool is_released        = get_bit_at_index(lParam, 31);
		
		bool is_transition      = (was_pressed == is_released);

		keyboard_set_key(wParam, !is_released);
	}
}

void process_raw_input_callback(HWND window, RAWKEYBOARD const & data) {
	if (keyboard_mode == Keyboard_Mode::Raw) {
		bool scan_e0_prefix = BITS_ARE_SET(data.Flags, RI_KEY_E0);
		bool scan_e1_prefix = BITS_ARE_SET(data.Flags, RI_KEY_E1);
		bool was_pressed    = BITS_ARE_SET(data.Flags, RI_KEY_MAKE);
		bool is_released    = BITS_ARE_SET(data.Flags, RI_KEY_BREAK);
		
		bool is_transition  = (was_pressed == is_released);
		
		keyboard_set_key(data.VKey, !is_released);
	}
}
