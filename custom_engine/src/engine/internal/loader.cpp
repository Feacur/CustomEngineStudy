#include "custom_pch.h"
#include "engine/api/loader.h"
#include "engine/core/code.h"
#include "engine/core/math_types.h"
#include "engine/api/asset_lookup.h"
#include "engine/api/graphics_params.h"
#include "engine/impl/array.h"
#include "engine/impl/bytecode.h"
#include "engine/debug/log.h"

#include <stdio.h>
#include <stb_image.h>

namespace custom {

void load_file(cstring path, Array<u8> & buffer) {
	FILE * file = fopen(path, "rb");
	CUSTOM_ASSERT(file, "failed to open file: %s", path);
	if (!file) { return; }

	fseek (file, 0, SEEK_END);
	long file_size = ftell(file); // @Note: not a size_t here
	rewind(file); // fseek (file, 0, SEEK_SET);

	buffer.set_capacity((u32)file_size);
	if (buffer.data) {
		buffer.count = (u32)fread(buffer.data, sizeof(u8), file_size, file);
		CUSTOM_ASSERT(buffer.count == buffer.capacity, "failed to read file %s;\n\tread %d out of %d bytes", path, buffer.count, buffer.capacity);
	}

	fclose(file);
}

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

void load_image(Bytecode & bc, u32 asset_id) {
	bc.write(graphics::Instruction::Message_Inline);
	bc.write("load image u8");

	static graphics::Data_Type const data_type = graphics::Data_Type::u8;
	static graphics::Texture_Type const texture_type = graphics::Texture_Type::Color;

	cstring path = asset::texture::paths[asset_id];

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

void load_imagef(Bytecode & bc, u32 asset_id) {
	bc.write(graphics::Instruction::Message_Inline);
	bc.write("load image r32");

	static graphics::Data_Type const data_type = graphics::Data_Type::r32;
	static graphics::Texture_Type const texture_type = graphics::Texture_Type::Color;

	cstring path = asset::texture::paths[asset_id];

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

void load_image16(Bytecode & bc, u32 asset_id) {
	bc.write(graphics::Instruction::Message_Inline);
	bc.write("load image u16");

	static graphics::Data_Type const data_type = graphics::Data_Type::u16;
	static graphics::Texture_Type const texture_type = graphics::Texture_Type::Color;

	cstring path = asset::texture::paths[asset_id];

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

void load_shader(Bytecode & bc, u32 asset_id) {
	bc.write(graphics::Instruction::Message_Inline);
	bc.write("load shader");

	cstring path = asset::shader::paths[asset_id];

	Array<u8> file; load_file(path, file);
	if (file.count != file.capacity) { return; }

	bc.write(graphics::Instruction::Allocate_Shader);
	bc.write(asset_id);
	bc.write(file.count + 1);
	bc.write(file.data, file.count);
	bc.write('\0');
}

void load_quad(Bytecode & bc, u32 asset_id) {
	bc.write(graphics::Instruction::Message_Inline);
	bc.write("load quad");

	// cstring path = asset::mesh::paths[asset_id];
	// Loader_File file = load_file(path);

	bc.write(graphics::Instruction::Allocate_Mesh);
	bc.write(asset_id);

	r32 const vertices[] = {
		/*position*/ -0.5f, -0.5f, 0.0f, /*UV*/ 0.0f, 0.0f,
		/*position*/  0.5f, -0.5f, 0.0f, /*UV*/ 1.0f, 0.0f,
		/*position*/  0.5f,  0.5f, 0.0f, /*UV*/ 1.0f, 1.0f,
		/*position*/ -0.5f,  0.5f, 0.0f, /*UV*/ 0.0f, 1.0f,
	};

	u32 const indices[] = {
		0, 1, 2,
		2, 3, 0,
	};

	bc.write(vertices);
	bc.write(indices);
}

}
