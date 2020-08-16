#pragma once
#include "engine/core/types.h"
#include "engine/core/math_types.h"
#include "engine/core/collection_types.h"
#include "engine/api/graphics_params.h"

namespace custom {

struct Lua_Asset {
	Array<u8> source;
};

struct Shader_Asset {
	Array<u8> source;

	~Shader_Asset() {
		source.~Array();
	}
};

struct Texture_Asset {
	Array<u8> data;
	ivec2 size;
	s32 channels;

	u8 is_dynamic = true;
	graphics::Data_Type data_type = graphics::Data_Type::u8;
	graphics::Texture_Type texture_type = graphics::Texture_Type::Color;

	graphics::Filter_Mode
		min_tex = graphics::Filter_Mode::None,
		min_mip = graphics::Filter_Mode::None,
		mag_tex = graphics::Filter_Mode::None;
	graphics::Wrap_Mode
		wrap_x = graphics::Wrap_Mode::Repeat,
		wrap_y = graphics::Wrap_Mode::Repeat;

	~Texture_Asset() {
		data.~Array();
	}
};

struct Mesh_Asset {
	struct Buffer {
		Array<u8> attributes;
		Array<u8> buffer;

		u8 is_index;
		graphics::Data_Type data_type;

		graphics::Mesh_Frequency frequency = graphics::Mesh_Frequency::Static;
		graphics::Mesh_Access    access    = graphics::Mesh_Access::Draw;
	};
	Array<Buffer> buffers;

	~Mesh_Asset() {
		for (u32 i = 0; i < buffers.count; ++i) {
			buffers[i].attributes.~Array();
			buffers[i].buffer.~Array();
		}
		buffers.~Array();
	}
};

}
