#pragma once
#include "engine/core/types.h"

namespace custom {

enum struct Key_Code : u8 {
	None,
	#define KEY_CODE_IMPL(T) T,
	#include "engine/registry_impl/key_codes.h"
	Last,
};

enum struct Mouse_Code : u8 {
	None,
	#define MOUSE_CODE_IMPL(T) T,
	#include "engine/registry_impl/mouse_codes.h"
	Last
};

}
