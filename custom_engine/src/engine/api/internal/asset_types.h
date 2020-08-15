#pragma once
#include "engine/core/types.h"
#include "engine/api/graphics_params.h"

namespace custom {

struct Shader_Asset {
	u8 dummy;
};

struct Texture_Asset {
	graphics::Filter_Mode
		min_tex = graphics::Filter_Mode::None,
		min_mip = graphics::Filter_Mode::None,
		mag_tex = graphics::Filter_Mode::None;
	graphics::Wrap_Mode
		wrap_x = graphics::Wrap_Mode::Repeat,
		wrap_y = graphics::Wrap_Mode::Repeat;
};

struct Mesh_Asset {
	u8 dummy;
};

}
