#include "custom_pch.h"
#include "engine/api/loader.h"
#include "engine/core/code.h"
#include "engine/core/math_types.h"
#include "engine/api/graphics_vm.h"
#include "engine/impl/bytecode.h"
#include "engine/debug/log.h"

#include <stb_image.h>

namespace custom {

static void describe_texture(
	Bytecode & bc,
	u32 asset_id, ivec2 size, u8 channels,
	graphics::Data_Type data_type, graphics::Texture_Type texture_type
) {
	bc.write(asset_id);
	bc.write(size);
	bc.write(channels);
	bc.write(data_type);
	bc.write(texture_type);
}

void load_image(Bytecode & bc, u32 asset_id, cstring path) {
	static graphics::Data_Type const data_type = graphics::Data_Type::U8;
	static graphics::Texture_Type const texture_type = graphics::Texture_Type::Color;

	stbi_set_flip_vertically_on_load(1);

	ivec2 size;
	s32 channels;
	stbi_uc * data = stbi_load(path, &size.x, &size.y, &channels, 0);
	CUSTOM_ASSERT(data, "failed to read image '%s'", path);

	// @Note: allocate GPU memory, describe; might take it from some lightweight meta
	bc.write(graphics::Instruction::Allocate_Texture);
	describe_texture(bc, asset_id, size, (u8)channels, data_type, texture_type);
	bc.write(graphics::Filter_Mode::Linear); bc.write(graphics::Filter_Mode::None);
	bc.write(graphics::Wrap_Mode::Repeat); bc.write(graphics::Wrap_Mode::Repeat);

	// @Note: upload actual texture data; might stream it later
	bc.write(graphics::Instruction::Load_Texture);
	describe_texture(bc, asset_id, size, (u8)channels, data_type, texture_type);
	bc.write(data, size.x * size.y * channels);

	stbi_image_free(data);
}

void load_imagef(Bytecode & bc, u32 asset_id, cstring path) {
	static graphics::Data_Type const data_type = graphics::Data_Type::R32;
	static graphics::Texture_Type const texture_type = graphics::Texture_Type::Color;

	stbi_set_flip_vertically_on_load(1);

	ivec2 size;
	s32 channels;
	float * data = stbi_loadf(path, &size.x, &size.y, &channels, 0);
	CUSTOM_ASSERT(data, "failed to read image '%s'", path);

	// @Note: allocate GPU memory, describe; might take it from some lightweight meta
	bc.write(graphics::Instruction::Allocate_Texture);
	describe_texture(bc, asset_id, size, (u8)channels, data_type, texture_type);
	bc.write(graphics::Filter_Mode::Linear); bc.write(graphics::Filter_Mode::None);
	bc.write(graphics::Wrap_Mode::Repeat); bc.write(graphics::Wrap_Mode::Repeat);

	// @Note: upload actual texture data; might stream it later
	bc.write(graphics::Instruction::Load_Texture);
	describe_texture(bc, asset_id, size, (u8)channels, data_type, texture_type);
	bc.write(data, size.x * size.y * channels);

	stbi_image_free(data);
}

void load_image16(Bytecode & bc, u32 asset_id, cstring path) {
	static graphics::Data_Type const data_type = graphics::Data_Type::U16;
	static graphics::Texture_Type const texture_type = graphics::Texture_Type::Color;

	stbi_set_flip_vertically_on_load(1);

	ivec2 size;
	s32 channels;
	stbi_us * data = stbi_load_16(path, &size.x, &size.y, &channels, 0);
	CUSTOM_ASSERT(data, "failed to read image '%s'", path);

	// @Note: allocate GPU memory, describe; might take it from some lightweight meta
	bc.write(graphics::Instruction::Allocate_Texture);
	describe_texture(bc, asset_id, size, (u8)channels, data_type, texture_type);
	bc.write(graphics::Filter_Mode::Linear); bc.write(graphics::Filter_Mode::None);
	bc.write(graphics::Wrap_Mode::Repeat); bc.write(graphics::Wrap_Mode::Repeat);

	// @Note: upload actual texture data; might stream it later
	bc.write(graphics::Instruction::Load_Texture);
	describe_texture(bc, asset_id, size, (u8)channels, data_type, texture_type);
	bc.write(data, size.x * size.y * channels);

	stbi_image_free(data);
}

}
