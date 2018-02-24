enum struct Keyboard_Keys : int32 {
	None,
	// Numbers (10)
	N0,
	N1,
	N2,
	N3,
	N4,
	N5,
	N6,
	N7,
	N8,
	N9,
	// Functional (12)
	F1,
	F2,
	F3,
	F4,
	F5,
	F6,
	F7,
	F8,
	F9,
	F10,
	F11,
	F12,
	// Letters (26)
	A,
	B,
	C,
	D,
	E,
	F,
	G,
	H,
	I,
	J,
	K,
	L,
	M,
	N,
	O,
	P,
	Q,
	R,
	S,
	T,
	U,
	V,
	W,
	X,
	Y,
	Z,
	// Arrows (4)
	Left,
	Right,
	Down,
	Up,
	// Commands (7)
	Escape,
	Enter,
	Control,
	Shift,
	Alt,
	Space,
	Tab,
	//
	Last,
};

GLOBAL_CONST int32 KEYBOARD_KEYS_NUMBER = (int32)Keyboard_Keys::Last;

struct Input_Keyboard {
	bool was_pressed[KEYBOARD_KEYS_NUMBER];
	bool is_pressed[KEYBOARD_KEYS_NUMBER];
};

GLOBAL_CONST int32 KEYBOARD_KEYS_BYTES = sizeof(Input_Keyboard::is_pressed);

inline bool keyboard_get_previous_state(Input_Keyboard *keyboard, Keyboard_Keys key) {
	ASSERT_TRUE(keyboard, "Keyboard is not initialized");
	ASSERT_TRUE((int32)key < KEYBOARD_KEYS_NUMBER, "Keyboard key is out of range");
	return keyboard->was_pressed[(int32)key];
}

inline bool keyboard_get_current_state(Input_Keyboard *keyboard, Keyboard_Keys key) {
	ASSERT_TRUE(keyboard, "Keyboard is not initialized");
	ASSERT_TRUE((int32)key < KEYBOARD_KEYS_NUMBER, "Keyboard key is out of range");
	return keyboard->is_pressed[(int32)key];
}

inline bool keyboard_get_transition_pressed(Input_Keyboard *keyboard, Keyboard_Keys key) {
	bool was_pressed = keyboard_get_previous_state(keyboard, key);
	bool is_pressed = keyboard_get_current_state(keyboard, key);
	return is_pressed && (is_pressed != was_pressed);
}

inline bool keyboard_get_transition_released(Input_Keyboard *keyboard, Keyboard_Keys key) {
	bool was_pressed = keyboard_get_previous_state(keyboard, key);
	bool is_pressed = keyboard_get_current_state(keyboard, key);
	return !is_pressed && (is_pressed != was_pressed);
}
