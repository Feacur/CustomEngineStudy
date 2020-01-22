#pragma once
#include "engine/core/types.h"
#include "engine/core/meta.h"

namespace custom {

enum struct Key_Code : u8 {
	None,
	D0, D1, D2, D3, D4, D5, D6, D7, D8, D9,
	F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
	A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
	Left, Right, Down, Up,
	Space, Enter, Control, Shift, Alt, Tab, Escape,
	Last,
};
UNDERLYING_TYPE_META(Key_Code, u8)
IS_ENUM_META(Key_Code)

}
