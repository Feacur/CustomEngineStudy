#include "custom_pch.h"
#include "engine/api/loader.h"
#include "engine/core/code.h"
#include "engine/core/math_types.h"
#include "engine/api/graphics_vm.h"
#include "engine/impl/command_buffer.h"
#include "engine/debug/log.h"

#include <stb_image.h>

namespace custom {

static void describe_texture(Command_Buffer & command_buffer, u32 asset_id, ivec2 size, u8 channels, Data_Type data_type, Texture_Type texture_type) {
	command_buffer.write(asset_id);
	command_buffer.write(size);
	command_buffer.write(channels);
	command_buffer.write(data_type);
	command_buffer.write(texture_type);
}

void load_image(Command_Buffer & command_buffer, u32 asset_id, cstring path) {
	static Data_Type const data_type = Data_Type::U8;
	static Texture_Type const texture_type = Texture_Type::Color;

	stbi_set_flip_vertically_on_load(1);

	ivec2 size;
	s32 channels;
	stbi_uc * data = stbi_load(path, &size.x, &size.y, &channels, 0);
	CUSTOM_ASSERT(data, "failed to read image '%s'", path);

	// @Note: allocate GPU memory, describe; might take it from some lightweight meta
	command_buffer.write(Graphics_Instruction::Allocate_Texture);
	describe_texture(command_buffer, asset_id, size, (u8)channels, data_type, texture_type);
	command_buffer.write(Filter_Mode::Linear); command_buffer.write(Filter_Mode::None);
	command_buffer.write(Wrap_Mode::Repeat); command_buffer.write(Wrap_Mode::Repeat);

	// @Note: upload actual texture data; might stream it later
	command_buffer.write(Graphics_Instruction::Load_Texture);
	describe_texture(command_buffer, asset_id, size, (u8)channels, data_type, texture_type);
	command_buffer.write(data, size.x * size.y * channels);

	stbi_image_free(data);
}

void load_imagef(Command_Buffer & command_buffer, u32 asset_id, cstring path) {
	static Data_Type const data_type = Data_Type::R32;
	static Texture_Type const texture_type = Texture_Type::Color;

	stbi_set_flip_vertically_on_load(1);

	ivec2 size;
	s32 channels;
	float * data = stbi_loadf(path, &size.x, &size.y, &channels, 0);
	CUSTOM_ASSERT(data, "failed to read image '%s'", path);

	// @Note: allocate GPU memory, describe; might take it from some lightweight meta
	command_buffer.write(Graphics_Instruction::Allocate_Texture);
	describe_texture(command_buffer, asset_id, size, (u8)channels, data_type, texture_type);
	command_buffer.write(Filter_Mode::Linear); command_buffer.write(Filter_Mode::None);
	command_buffer.write(Wrap_Mode::Repeat); command_buffer.write(Wrap_Mode::Repeat);

	// @Note: upload actual texture data; might stream it later
	command_buffer.write(Graphics_Instruction::Load_Texture);
	describe_texture(command_buffer, asset_id, size, (u8)channels, data_type, texture_type);
	command_buffer.write(data, size.x * size.y * channels);

	stbi_image_free(data);
}

void load_image16(Command_Buffer & command_buffer, u32 asset_id, cstring path) {
	static Data_Type const data_type = Data_Type::U16;
	static Texture_Type const texture_type = Texture_Type::Color;

	stbi_set_flip_vertically_on_load(1);

	ivec2 size;
	s32 channels;
	stbi_us * data = stbi_load_16(path, &size.x, &size.y, &channels, 0);
	CUSTOM_ASSERT(data, "failed to read image '%s'", path);

	// @Note: allocate GPU memory, describe; might take it from some lightweight meta
	command_buffer.write(Graphics_Instruction::Allocate_Texture);
	describe_texture(command_buffer, asset_id, size, (u8)channels, data_type, texture_type);
	command_buffer.write(Filter_Mode::Linear); command_buffer.write(Filter_Mode::None);
	command_buffer.write(Wrap_Mode::Repeat); command_buffer.write(Wrap_Mode::Repeat);

	// @Note: upload actual texture data; might stream it later
	command_buffer.write(Graphics_Instruction::Load_Texture);
	describe_texture(command_buffer, asset_id, size, (u8)channels, data_type, texture_type);
	command_buffer.write(data, size.x * size.y * channels);

	stbi_image_free(data);
}

}
