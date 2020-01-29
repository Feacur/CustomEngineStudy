#pragma once
#include "engine/core/types.h"
#include "engine/api/graphics_params.h"

namespace custom {
namespace asset {

namespace shader {
	extern cstring paths[];
};

namespace texture {
	struct Meta
	{
		graphics::Filter_Mode texture_filter, mipmap_filter;
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
