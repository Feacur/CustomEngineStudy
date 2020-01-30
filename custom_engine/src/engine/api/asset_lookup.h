#pragma once
#include "engine/core/types.h"
#include "engine/api/graphics_params.h"

namespace custom {
namespace asset {

namespace shader {
	struct Meta
	{
		graphics::Shader_Part parts;
	};
	extern Meta meta_presets[];

	extern cstring paths[];
	extern u8 meta_ids[];
};

namespace texture {
	struct Meta
	{
		graphics::Filter_Mode min_tex, min_mip, mag_tex;
		graphics::Wrap_Mode wrap_x, wrap_y;
	};
	extern Meta meta_presets[];

	extern cstring paths[];
	extern u8 meta_ids[];
};

namespace mesh {
	extern cstring paths[];
};

}}
