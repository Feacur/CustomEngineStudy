#pragma once
#include "engine/core/types.h"
#include "engine/core/meta.h"

namespace sandbox {

enum struct Uniform : u32
{
	#define UNIFORM_IMPL(T) T,
	#include "uniform_registry_impl.h"
	count,
};
UNDERLYING_TYPE_META(Uniform, u32)
IS_ENUM_META(Uniform)

}
