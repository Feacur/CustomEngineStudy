enum struct Pointer_Keys : uint8 {
	None,
	//
	Key1,
	Key2,
	Key3,
	//
	Last
};
UNDERLYING_TYPE_META(Pointer_Keys, uint8)
IS_ENUM_META(Pointer_Keys)

static int32 const POINTER_KEYS_NUMBER = (int32)Pointer_Keys::Last;

struct Input_Pointer {
	Vector2i pixel_position;
	Vector2i raw_delta;
	Vector2 wheel;
	int8 was_pressed[POINTER_KEYS_NUMBER];
	int8 is_pressed[POINTER_KEYS_NUMBER];
};

static int32 const POINTER_KEYS_BYTES = sizeof(Input_Pointer::is_pressed);

inline bool pointer_get_previous_state(Input_Pointer *pointer, Pointer_Keys key) {
	CUSTOM_ASSERT(pointer, "Pointer is not initialized");
	CUSTOM_ASSERT((int32)key < POINTER_KEYS_NUMBER, "Pointer key is out of range");
	return pointer->was_pressed[(int32)key];
}

inline bool pointer_get_current_state(Input_Pointer *pointer, Pointer_Keys key) {
	CUSTOM_ASSERT(pointer, "Pointer is not initialized");
	CUSTOM_ASSERT((int32)key < POINTER_KEYS_NUMBER, "Pointer key is out of range");
	return pointer->is_pressed[(int32)key];
}

inline bool pointer_get_transition_pressed(Input_Pointer *pointer, Pointer_Keys key) {
	bool was_pressed = pointer_get_previous_state(pointer, key);
	bool is_pressed = pointer_get_current_state(pointer, key);
	return is_pressed && (is_pressed != was_pressed);
}

inline bool pointer_get_transition_released(Input_Pointer *pointer, Pointer_Keys key) {
	bool was_pressed = pointer_get_previous_state(pointer, key);
	bool is_pressed = pointer_get_current_state(pointer, key);
	return !is_pressed && (is_pressed != was_pressed);
}
