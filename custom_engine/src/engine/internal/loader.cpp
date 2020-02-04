#include "custom_pch.h"
#include "engine/api/loader.h"
#include "engine/api/file.h"
#include "engine/core/code.h"
#include "engine/core/math_types.h"
#include "engine/api/asset_lookup.h"
#include "engine/api/graphics_params.h"
#include "engine/impl/array.h"
#include "engine/impl/bytecode.h"
#include "engine/debug/log.h"

#include <stb_image.h>

namespace custom {
namespace graphics {

template<typename T> static constexpr inline Data_Type get_data_type() { return Data_Type::None; }
#define GDT_IMPL(T) template<> constexpr inline Data_Type get_data_type<T>() { return Data_Type::T; }
	GDT_IMPL(sampler_unit);
	GDT_IMPL(s8); GDT_IMPL(s16); GDT_IMPL(s32);
	GDT_IMPL(u8); GDT_IMPL(u16); GDT_IMPL(u32);
	GDT_IMPL(r32); GDT_IMPL(r64);
	GDT_IMPL(vec2); GDT_IMPL(vec3); GDT_IMPL(vec4);
	GDT_IMPL(ivec2); GDT_IMPL(ivec3); GDT_IMPL(ivec4);
	GDT_IMPL(uvec2); GDT_IMPL(uvec3); GDT_IMPL(uvec4);
	GDT_IMPL(mat2); GDT_IMPL(mat3); GDT_IMPL(mat4);
#undef GDT_IMPL

}}

namespace custom {
namespace loader {

static Bytecode * bc = NULL;

static void init_uniforms();

template<typename T, u32 count>
static void write_data_array(T const (& data)[count]);

static void describe_texture(
	u32 asset_id, ivec2 size, u8 channels,
	graphics::Data_Type data_type, graphics::Texture_Type texture_type
);

static void describe_texture_load(
	u32 asset_id, ivec2 offset, ivec2 size, u8 channels,
	graphics::Data_Type data_type, graphics::Texture_Type texture_type
);

void init(Bytecode * bytecode) {
	bc = bytecode;
	init_uniforms();
}

void image(u32 asset_id) {
	static graphics::Data_Type const data_type = graphics::Data_Type::u8;
	static graphics::Texture_Type const texture_type = graphics::Texture_Type::Color;

	cstring path = asset::texture::paths[asset_id];
	Array<u8> file; file_read(path, file);
	if (file.count != file.capacity) { return; }

	stbi_set_flip_vertically_on_load(1);

	ivec2 size;
	s32 channels;
	stbi_uc * data = stbi_load_from_memory(file.data, file.count, &size.x, &size.y, &channels, 0);
	CUSTOM_ASSERT(data, "failed to read image '%s'", path);

	// @Note: allocate GPU memory, describe; might take it from some lightweight meta
	bc->write(graphics::Instruction::Allocate_Texture);
	describe_texture(asset_id, size, (u8)channels, data_type, texture_type);

	// @Note: upload actual texture data; might stream it later
	bc->write(graphics::Instruction::Load_Texture);
	describe_texture_load(asset_id, {0, 0}, size, (u8)channels, data_type, texture_type);
	bc->write(data, size.x * size.y * channels);

	stbi_image_free(data);
}

void imagef(u32 asset_id) {
	static graphics::Data_Type const data_type = graphics::Data_Type::r32;
	static graphics::Texture_Type const texture_type = graphics::Texture_Type::Color;

	cstring path = asset::texture::paths[asset_id];
	Array<u8> file; file_read(path, file);
	if (file.count != file.capacity) { return; }

	stbi_set_flip_vertically_on_load(1);

	ivec2 size;
	s32 channels;
	float * data = stbi_loadf_from_memory(file.data, file.count, &size.x, &size.y, &channels, 0);
	CUSTOM_ASSERT(data, "failed to read image '%s'", path);

	// @Note: allocate GPU memory, describe; might take it from some lightweight meta
	bc->write(graphics::Instruction::Allocate_Texture);
	describe_texture(asset_id, size, (u8)channels, data_type, texture_type);

	// @Note: upload actual texture data; might stream it later
	bc->write(graphics::Instruction::Load_Texture);
	describe_texture_load(asset_id, {0, 0}, size, (u8)channels, data_type, texture_type);
	bc->write(data, size.x * size.y * channels);

	stbi_image_free(data);
}

void image16(u32 asset_id) {
	static graphics::Data_Type const data_type = graphics::Data_Type::u16;
	static graphics::Texture_Type const texture_type = graphics::Texture_Type::Color;

	cstring path = asset::texture::paths[asset_id];
	Array<u8> file; file_read(path, file);
	if (file.count != file.capacity) { return; }

	stbi_set_flip_vertically_on_load(1);

	ivec2 size;
	s32 channels;
	stbi_us * data = stbi_load_16_from_memory(file.data, file.count, &size.x, &size.y, &channels, 0);
	CUSTOM_ASSERT(data, "failed to read image '%s'", path);

	// @Note: allocate GPU memory, describe; might take it from some lightweight meta
	bc->write(graphics::Instruction::Allocate_Texture);
	describe_texture(asset_id, size, (u8)channels, data_type, texture_type);

	// @Note: upload actual texture data; might stream it later
	bc->write(graphics::Instruction::Load_Texture);
	describe_texture_load(asset_id, {0, 0}, size, (u8)channels, data_type, texture_type);
	bc->write(data, size.x * size.y * channels);

	stbi_image_free(data);
}

void shader(u32 asset_id) {
	cstring path = asset::shader::paths[asset_id];
	Array<u8> file; file_read(path, file);
	if (file.count != file.capacity) { return; }

	u8 meta_id = asset::shader::meta_ids[asset_id];
	asset::shader::Meta meta = asset::shader::meta_presets[meta_id];

	bc->write(graphics::Instruction::Allocate_Shader);
	bc->write(asset_id);
	
	bc->write(graphics::Instruction::Load_Shader);
	bc->write(asset_id);
	bc->write(file.count + 1);
	bc->write(file.data, file.count); bc->write('\0');
	bc->write(meta.parts);
}

void quad(u32 asset_id) {
	// cstring path = asset::mesh::paths[asset_id];
	// Array<u8> file; file_read(path, file);
	// if (file.count != file.capacity) { return; }

	u8 meta_id = asset::mesh::meta_ids[asset_id];
	asset::mesh::Meta meta = asset::mesh::meta_presets[meta_id];

	r32 const vertex_data[] = {
		/*position*/ -0.5f, -0.5f, 0.0f, /*UV*/ 0.0f, 0.0f,
		/*position*/  0.5f, -0.5f, 0.0f, /*UV*/ 1.0f, 0.0f,
		/*position*/  0.5f,  0.5f, 0.0f, /*UV*/ 1.0f, 1.0f,
		/*position*/ -0.5f,  0.5f, 0.0f, /*UV*/ 0.0f, 1.0f,
	};
	u8 const vertex_attributes[] = { /*position*/ 3, /*UV*/ 2, };

	u8 const index_data[] = {
		0, 1, 2,
		2, 3, 0,
	};

	// might use graphics::get_data_type<decltype(+value)>(), but it's cryptic

	bc->write(graphics::Instruction::Allocate_Mesh);
	bc->write(asset_id);
	bc->write((u32)2);
	bc->write(meta.vfrequency); bc->write(meta.vaccess); bc->write(graphics::Data_Type::r32);
	bc->write((u32)C_ARRAY_LENGTH(vertex_data)); bc->write(vertex_attributes);
	bc->write(meta.ifrequency); bc->write(meta.iaccess); bc->write(graphics::Data_Type::u8);
	bc->write((u32)C_ARRAY_LENGTH(index_data)); bc->write((u32)0);
	bc->write((u8)1);

	bc->write(graphics::Instruction::Load_Mesh);
	bc->write(asset_id);
	bc->write((u32)2);
	bc->write((u32)0); write_data_array(vertex_data);
	bc->write((u32)0); write_data_array(index_data);

	bc->write(graphics::Instruction::Set_Mesh_Buffer_Count);
	bc->write(asset_id);
	bc->write((u32)2);
	bc->write((u32)C_ARRAY_LENGTH(vertex_data));
	bc->write((u32)C_ARRAY_LENGTH(index_data));
}

void particle_test(u32 asset_id) {
	// cstring path = asset::mesh::paths[asset_id];
	// Array<u8> file; file_read(path, file);
	// if (file.count != file.capacity) { return; }

	u8 meta_id = asset::mesh::meta_ids[asset_id];
	asset::mesh::Meta meta = asset::mesh::meta_presets[meta_id];

	u8 const vertex_attributes[] = { /*position*/ 3, /*color*/ 4, };

	bc->write(graphics::Instruction::Allocate_Mesh);
	bc->write(asset_id);
	bc->write((u32)2);
	bc->write(meta.vfrequency); bc->write(meta.vaccess); bc->write(graphics::Data_Type::r32);
	bc->write((u32)((3 + 4) * 4 * 128)); bc->write(vertex_attributes);
	bc->write(meta.ifrequency); bc->write(meta.iaccess); bc->write(graphics::Data_Type::u32);
	bc->write((u32)(3 * 2 * 128)); bc->write((u32)0);
	bc->write((u8)1);
}

}}

//
// utilities
//

namespace custom {
namespace loader {

static void init_uniforms() {
	cstring name;

	u32 count = 0;
	while ((name = asset::uniform::names[count]) != NULL) {
		++count;
	}

	bc->write(graphics::Instruction::Init_Uniforms);
	bc->write(count);

	u32 asset_id = 0;
	while ((name = asset::uniform::names[asset_id]) != NULL) {
		u32 length = (u32)strlen(name);
		bc->write(length + 1);
		bc->write(name, length); bc->write('\0');
		++asset_id;
	}
}

template<typename T, u32 count>
static void write_data_array(T const (& data)[count]) {
	bc->write(graphics::get_data_type<T>());
	bc->write(data);
}

static void describe_texture(
	u32 asset_id, ivec2 size, u8 channels,
	graphics::Data_Type data_type, graphics::Texture_Type texture_type
) {
	u8 meta_id = asset::texture::meta_ids[asset_id];
	asset::texture::Meta meta = asset::texture::meta_presets[meta_id];
	bc->write(asset_id);
	bc->write(size);
	bc->write(channels);
	bc->write(data_type);
	bc->write(texture_type);
	bc->write(meta.min_tex);
	bc->write(meta.min_mip);
	bc->write(meta.mag_tex);
	bc->write(meta.wrap_x); bc->write(meta.wrap_y);
}

static void describe_texture_load(
	u32 asset_id, ivec2 offset, ivec2 size, u8 channels,
	graphics::Data_Type data_type, graphics::Texture_Type texture_type
) {
	bc->write(asset_id);
	bc->write(offset);
	bc->write(size);
	bc->write(channels);
	bc->write(data_type);
	bc->write(texture_type);
}

}}
