#include "custom_pch.h"
#include "engine/api/loader.h"
#include "engine/core/code.h"
#include "engine/core/math_types.h"
#include "engine/api/asset_lookup.h"
#include "engine/api/graphics_params.h"
#include "engine/impl/bytecode.h"
#include "engine/debug/log.h"

// @Todo: move to a separate translation unit?
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace custom {

static cstring const shader_test = R"(
// name: renderer2d
// version: 330 core

// section: VERTEX_SECTION
#if defined(VERTEX_SECTION)
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

out vec2 v_TexCoord;

void main()
{
	v_TexCoord = a_TexCoord;
	gl_Position = vec4(a_Position, 1.0);
}
#endif // defined(VERTEX_SECTION)

// section: FRAGMENT_SECTION
#if defined(FRAGMENT_SECTION)
in vec2 v_TexCoord;

uniform sampler2D u_Texture;

layout(location = 0) out vec4 color;

void main()
{
	color = texture(u_Texture, v_TexCoord);
}
#endif // defined(FRAGMENT_SECTION)
)";

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

	u32 len = (u32)strlen(shader_test) + 1;

	bc.write(graphics::Instruction::Allocate_Shader);
	bc.write(asset_id);
	bc.write(len);
	bc.write(shader_test, len);
}

void load_quad(Bytecode & bc, u32 asset_id) {
	bc.write(graphics::Instruction::Message_Inline);
	bc.write("load quad");

	// cstring path = asset::mesh::paths[asset_id];

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
