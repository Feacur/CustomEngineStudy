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
};

struct Texture_Asset {
	u8 * data;
	ivec2 size;
	s32 channels;

	graphics::Data_Type data_type = graphics::Data_Type::u8;
	graphics::Texture_Type texture_type = graphics::Texture_Type::Color;

	graphics::Filter_Mode
		min_tex = graphics::Filter_Mode::None,
		min_mip = graphics::Filter_Mode::None,
		mag_tex = graphics::Filter_Mode::None;
	graphics::Wrap_Mode
		wrap_x = graphics::Wrap_Mode::Repeat,
		wrap_y = graphics::Wrap_Mode::Repeat;
};

struct Mesh_Asset {
	Array<u8> attributes;
	Array<r32> vertices;
	Array<u32> indices;
	graphics::Mesh_Frequency vfrequency = graphics::Mesh_Frequency::Static;
	graphics::Mesh_Access vaccess       = graphics::Mesh_Access::Draw;
	graphics::Mesh_Frequency ifrequency = graphics::Mesh_Frequency::Static;
	graphics::Mesh_Access iaccess       = graphics::Mesh_Access::Draw;
};

}
