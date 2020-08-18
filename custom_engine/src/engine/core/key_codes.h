#pragma once
#include "engine/core/types.h"
#include "engine/core/meta.h"

namespace custom {

enum struct Key_Code : u8 {
	None,
	#define KEY_CODE_IMPL(T) T,
	#include "engine/registry_impl/key_codes.h"
	Last,
};
UNDERLYING_TYPE_META(Key_Code, u8)
IS_ENUM_META(Key_Code)

enum struct Mouse_Code : u8 {
	None,
	#define MOUSE_CODE_IMPL(T) T,
	#include "engine/registry_impl/mouse_codes.h"
	Last
};
UNDERLYING_TYPE_META(Mouse_Code, u8)
IS_ENUM_META(Mouse_Code)

}
