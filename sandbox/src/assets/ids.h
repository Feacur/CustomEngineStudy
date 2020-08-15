#pragma once
#include "engine/core/types.h"
#include "engine/core/meta.h"

namespace sandbox {

enum struct Uniform : u32
{
	resolution,
	view_proj,
	transform,
	texture,
	color,
	z,
	count,
};
UNDERLYING_TYPE_META(Uniform, u32)
IS_ENUM_META(Uniform)

}
