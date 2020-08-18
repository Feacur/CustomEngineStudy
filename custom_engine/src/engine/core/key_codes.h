#pragma once
#include "engine/core/types.h"
#include "engine/core/meta.h"

namespace custom {

enum struct Key_Code : u8 {
	None,
	#define KEY_CODE_IMPL(T) T,
	#include "key_codes_registry_impl.h"
	Last,
};
UNDERLYING_TYPE_META(Key_Code, u8)
IS_ENUM_META(Key_Code)

enum struct Mouse_Code : u8 {
	None,
	#define MOUSE_CODE_IMPL(T) T,
	#include "mouse_codes_registry_impl.h"
	Last
};
UNDERLYING_TYPE_META(Mouse_Code, u8)
IS_ENUM_META(Mouse_Code)

}
