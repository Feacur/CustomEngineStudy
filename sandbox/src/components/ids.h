#pragma once
#include "engine/core/types.h"
#include "engine/core/meta.h"

namespace sandbox {

#define COMPONENT_IMPL(T) T,
enum struct Component_Type : u32
{
	#include "registry_impl.h"
	count,
};
UNDERLYING_TYPE_META(Component_Type, u32)
IS_ENUM_META(Component_Type)

}
