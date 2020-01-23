#pragma once
#include "engine/core/types.h"
#include "engine/core/meta.h"

namespace custom {

enum struct Mouse_Code : u8 {
	None,
	Key1, Key2, Key3, Key4, Key5,
	Last
};
UNDERLYING_TYPE_META(Mouse_Code, u8)
IS_ENUM_META(Mouse_Code)

}
