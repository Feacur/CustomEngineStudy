#pragma once
#include "engine/core/math_types.h"
#include "engine/api/graphics_params.h"

namespace custom {
namespace asset {

namespace script {
};

namespace uniform {
};

namespace texture {
	struct Meta
	{
		graphics::Filter_Mode min_tex, min_mip, mag_tex;
		graphics::Wrap_Mode wrap_x, wrap_y;
	};
};

namespace mesh {
	struct Meta
	{
		graphics::Mesh_Frequency vfrequency; graphics::Mesh_Access vaccess;
		graphics::Mesh_Frequency ifrequency; graphics::Mesh_Access iaccess;
	};
};

namespace target {
	struct Meta
	{
		u32 texture_id;
		ivec2 size;
		graphics::Data_Type data_type;
		graphics::Texture_Type texture_type;
	};
};

}}
