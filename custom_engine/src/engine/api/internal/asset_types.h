#pragma once
#include "engine/core/types.h"
#include "engine/core/math_types.h"
#include "engine/core/collection_types.h"
#include "engine/api/graphics_params.h"

namespace custom {

struct Shader_Asset {
	Array<u8> source;
	~Shader_Asset() = default;
};

struct Texture_Asset {
	Array<u8> data;
	ivec2 size;
	s32 channels;

	u8 is_dynamic;
	graphics::Data_Type data_type;
	graphics::Texture_Type texture_type;

	graphics::Filter_Mode min_tex, min_mip, mag_tex;
	graphics::Wrap_Mode wrap_x, wrap_y;

	~Texture_Asset() = default;
};

struct Mesh_Asset {
	struct Buffer {
		Array<u8> attributes;
		Array<u8> buffer;

		u8 is_index;
		graphics::Data_Type data_type;

		graphics::Mesh_Frequency frequency;
		graphics::Mesh_Access access;
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
