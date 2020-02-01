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

namespace uniform {
	extern cstring names[];
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
	struct Meta
	{
		graphics::Mesh_Frequency vfrequency; graphics::Mesh_Access vaccess;
		graphics::Mesh_Frequency ifrequency; graphics::Mesh_Access iaccess;
	};
	extern Meta meta_presets[];

	extern cstring paths[];
	extern u8 meta_ids[];
};

}}
