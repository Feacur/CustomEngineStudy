#include "engine/api/asset_lookup.h"

namespace custom {
namespace asset {

namespace shader {
	Meta meta_presets[] = {
		{}, // NULL
		{ // 1
			graphics::Shader_Part::Vertex | graphics::Shader_Part::Pixel,
		},
	};

	cstring paths[] = {
		"assets/shaders/renderer2d.glsl",
		"assets/shaders/device.glsl",
		NULL
	};
	
	u8 meta_ids[] = {
		1,
		1,
		NULL
	};
};

namespace texture {
	Meta meta_presets[] = {
		{}, // NULL
		{ // 1
			graphics::Filter_Mode::Linear,
			graphics::Filter_Mode::None,
			graphics::Filter_Mode::None,
			graphics::Wrap_Mode::Repeat, graphics::Wrap_Mode::Repeat,
		},
	};

	cstring paths[] = {
		"assets/textures/checkerboard.png",
		NULL
	};
	
	u8 meta_ids[] = {
		1,
		NULL
	};
};

namespace mesh {
	cstring paths[] = {
		"",
		NULL
	};
};

}}
