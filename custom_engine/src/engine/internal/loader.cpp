#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/core/math_types.h"
#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/api/platform/file.h"
#include "engine/api/internal/loader.h"
#include "engine/api/client/asset_lookup.h"
#include "engine/api/graphics_params.h"
#include "engine/api/resource.h"
#include "engine/impl/array.h"
#include "engine/impl/array_fixed.h"
#include "engine/impl/bytecode.h"

#include <stb_image.h>

#include "obj_parser.h"

namespace custom {
namespace graphics {

template<typename T> static constexpr inline Data_Type get_data_type() { return Data_Type::None; }
#define GDT_IMPL(T) template<> constexpr inline Data_Type get_data_type<T>() { return Data_Type::T; }
	GDT_IMPL(texture_unit); GDT_IMPL(sampler_unit);
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

template<typename T>
static void write_data_array_custom(custom::Array<T> const & data);

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
	stbi_set_flip_vertically_on_load(1);
	init_uniforms();
}

void image(u32 asset_id) {
	if (has_texture(asset_id)) { return; }

	static graphics::Data_Type const data_type = graphics::Data_Type::u8;
	static graphics::Texture_Type const texture_type = graphics::Texture_Type::Color;

	cstring path = asset::texture::paths[asset_id];
	Array<u8> file; file::read(path, file);
	if (file.count != file.capacity) { return; }

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
	if (has_texture(asset_id)) { return; }

	static graphics::Data_Type const data_type = graphics::Data_Type::r32;
	static graphics::Texture_Type const texture_type = graphics::Texture_Type::Color;

	cstring path = asset::texture::paths[asset_id];
	Array<u8> file; file::read(path, file);
	if (file.count != file.capacity) { return; }

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
	if (has_texture(asset_id)) { return; }

	static graphics::Data_Type const data_type = graphics::Data_Type::u16;
	static graphics::Texture_Type const texture_type = graphics::Texture_Type::Color;

	cstring path = asset::texture::paths[asset_id];
	Array<u8> file; file::read(path, file);
	if (file.count != file.capacity) { return; }

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
	if (has_shader(asset_id)) { return; }

	cstring path = asset::shader::paths[asset_id];
	Array<u8> file; file::read(path, file);
	if (file.count != file.capacity) { return; }

	u8 meta_id = asset::shader::meta_ids[asset_id];
	asset::shader::Meta const & meta = asset::shader::meta_presets[meta_id];

	bc->write(graphics::Instruction::Allocate_Shader);
	bc->write(asset_id);
	
	bc->write(graphics::Instruction::Load_Shader);
	bc->write(asset_id);
	bc->write(file.count + 1);
	bc->write(file.data, file.count); bc->write('\0');
	bc->write(meta.parts);
}

void mesh_obj(u32 asset_id) {
	if (has_mesh(asset_id)) { return; }

	cstring path = asset::mesh::paths[asset_id];
	Array<u8> file; file::read(path, file);
	if (file.count != file.capacity) { return; }

	Array<u8> vertex_attributes;
	Array<r32> vertices;
	Array<u32> indices;
	obj::parse(file, vertex_attributes, vertices, indices);

	u8 meta_id = asset::mesh::meta_ids[asset_id];
	asset::mesh::Meta const & meta = asset::mesh::meta_presets[meta_id];

	bc->write(graphics::Instruction::Allocate_Mesh);
	bc->write(asset_id);
	bc->write((u8)2);
	bc->write((b8)false); bc->write(graphics::Mesh_Frequency::Static); bc->write(graphics::Mesh_Access::Draw);
	bc->write(graphics::Data_Type::r32); bc->write(vertices.count); bc->write(vertices.count);
	bc->write(vertex_attributes);
	bc->write((b8)true); bc->write(graphics::Mesh_Frequency::Static); bc->write(graphics::Mesh_Access::Draw);
	bc->write(graphics::Data_Type::u32); bc->write(indices.count); bc->write(indices.count);
	bc->write((u32)0);

	bc->write(graphics::Instruction::Load_Mesh);
	bc->write(asset_id);
	bc->write((u8)2);
	bc->write((u32)0); write_data_array_custom(vertices);
	bc->write((u32)0); write_data_array_custom(indices);
}

}}

#include "engine/api/runtime_data.h"

namespace custom {
namespace loader {

u32 create_mesh(u32 local_id, runtime::Buffer const * buffers, u8 count) {
	u32 asset_id = asset::mesh::count + local_id;
	if (has_mesh(asset_id)) { return asset_id; }

	bc->write(graphics::Instruction::Allocate_Mesh);
	bc->write(asset_id);
	bc->write(count);
	for (u8 i = 0; i < count; ++i) {
		runtime::Buffer const * buffer = buffers + i;
		bc->write(buffer->is_index);
		bc->write(buffer->frequency); bc->write(buffer->access);
		bc->write(buffer->data_type); bc->write(buffer->capacity); bc->write(buffer->count);
		bc->write((u32)buffer->attributes_count);
		bc->write(buffer->attributes, buffer->attributes_count);
	}
	return asset_id;
}

u32 create_quad_xy(u32 local_id) {
	u32 asset_id = asset::mesh::count + local_id;
	if (has_mesh(asset_id)) { return asset_id; }

	r32 const vertex_data[] = {
		/*position*/ -0.5f, -0.5f, 0.0f, /*UV*/ 0.0f, 0.0f,
		/*position*/  0.5f, -0.5f, 0.0f, /*UV*/ 1.0f, 0.0f,
		/*position*/  0.5f,  0.5f, 0.0f, /*UV*/ 1.0f, 1.0f,
		/*position*/ -0.5f,  0.5f, 0.0f, /*UV*/ 0.0f, 1.0f,
	};
	u32 vertex_data_count = (u32)C_ARRAY_LENGTH(vertex_data);
	u8 const vertex_attributes[] = { /*position*/ 3, /*UV*/ 2, };

	u8 const index_data[] = {
		0, 1, 2,
		2, 3, 0,
	};
	u32 index_data_count = (u32)C_ARRAY_LENGTH(index_data);

	// might use graphics::get_data_type<decltype(+value)>(), but it's cryptic

	bc->write(graphics::Instruction::Allocate_Mesh);
	bc->write(asset_id);
	bc->write((u8)2);
	bc->write((b8)false); bc->write(graphics::Mesh_Frequency::Static); bc->write(graphics::Mesh_Access::Draw);
	bc->write(graphics::Data_Type::r32); bc->write(vertex_data_count); bc->write(vertex_data_count);
	bc->write(vertex_attributes);
	bc->write((b8)true); bc->write(graphics::Mesh_Frequency::Static); bc->write(graphics::Mesh_Access::Draw);
	bc->write(graphics::Data_Type::u8); bc->write(index_data_count); bc->write(index_data_count);
	bc->write((u32)0);

	bc->write(graphics::Instruction::Load_Mesh);
	bc->write(asset_id);
	bc->write((u8)2);
	bc->write((u32)0); write_data_array(vertex_data);
	bc->write((u32)0); write_data_array(index_data);

	return asset_id;
}

u32 create_quad_xz(u32 local_id) {
	u32 asset_id = asset::mesh::count + local_id;
	if (has_mesh(asset_id)) { return asset_id; }

	r32 const vertex_data[] = {
		/*position*/ -0.5f, 0.0f, -0.5f, /*UV*/ 0.0f, 0.0f,
		/*position*/  0.5f, 0.0f, -0.5f, /*UV*/ 1.0f, 0.0f,
		/*position*/  0.5f, 0.0f,  0.5f, /*UV*/ 1.0f, 1.0f,
		/*position*/ -0.5f, 0.0f,  0.5f, /*UV*/ 0.0f, 1.0f,
	};
	u32 vertex_data_count = (u32)C_ARRAY_LENGTH(vertex_data);
	u8 const vertex_attributes[] = { /*position*/ 3, /*UV*/ 2, };

	u8 const index_data[] = {
		0, 1, 2,
		2, 3, 0,
	};
	u32 index_data_count = (u32)C_ARRAY_LENGTH(index_data);

	// might use graphics::get_data_type<decltype(+value)>(), but it's cryptic

	bc->write(graphics::Instruction::Allocate_Mesh);
	bc->write(asset_id);
	bc->write((u8)2);
	bc->write((b8)false); bc->write(graphics::Mesh_Frequency::Static); bc->write(graphics::Mesh_Access::Draw);
	bc->write(graphics::Data_Type::r32); bc->write(vertex_data_count); bc->write(vertex_data_count);
	bc->write(vertex_attributes);
	bc->write((b8)true); bc->write(graphics::Mesh_Frequency::Static); bc->write(graphics::Mesh_Access::Draw);
	bc->write(graphics::Data_Type::u8); bc->write(index_data_count); bc->write(index_data_count);
	bc->write((u32)0);

	bc->write(graphics::Instruction::Load_Mesh);
	bc->write(asset_id);
	bc->write((u8)2);
	bc->write((u32)0); write_data_array(vertex_data);
	bc->write((u32)0); write_data_array(index_data);

	return asset_id;
}

u32 create_quad2d(u32 local_id) {
	u32 asset_id = asset::mesh::count + local_id;
	if (has_mesh(asset_id)) { return asset_id; }

	r32 const vertex_data[] = {
		/*position*/ -0.5f, -0.5f, /*UV*/ 0.0f, 0.0f,
		/*position*/  0.5f, -0.5f, /*UV*/ 1.0f, 0.0f,
		/*position*/  0.5f,  0.5f, /*UV*/ 1.0f, 1.0f,
		/*position*/ -0.5f,  0.5f, /*UV*/ 0.0f, 1.0f,
	};
	u32 vertex_data_count = (u32)C_ARRAY_LENGTH(vertex_data);
	u8 const vertex_attributes[] = { /*position*/ 2, /*UV*/ 2, };

	u8 const index_data[] = {
		0, 1, 2,
		2, 3, 0,
	};
	u32 index_data_count = (u32)C_ARRAY_LENGTH(index_data);

	// might use graphics::get_data_type<decltype(+value)>(), but it's cryptic

	bc->write(graphics::Instruction::Allocate_Mesh);
	bc->write(asset_id);
	bc->write((u8)2);
	bc->write((b8)false); bc->write(graphics::Mesh_Frequency::Static); bc->write(graphics::Mesh_Access::Draw);
	bc->write(graphics::Data_Type::r32); bc->write(vertex_data_count); bc->write(vertex_data_count);
	bc->write(vertex_attributes);
	bc->write((b8)true); bc->write(graphics::Mesh_Frequency::Static); bc->write(graphics::Mesh_Access::Draw);
	bc->write(graphics::Data_Type::u8); bc->write(index_data_count); bc->write(index_data_count);
	bc->write((u32)0);

	bc->write(graphics::Instruction::Load_Mesh);
	bc->write(asset_id);
	bc->write((u8)2);
	bc->write((u32)0); write_data_array(vertex_data);
	bc->write((u32)0); write_data_array(index_data);

	return asset_id;
}

u32 create_cube(u32 local_id) {
	u32 asset_id = asset::mesh::count + local_id;
	if (has_mesh(asset_id)) { return asset_id; }

	u8 positions_source[3 * 8] = {
		0, 0, 0,
		1, 0, 0,
		1, 1, 0,
		0, 1, 0,
		1, 0, 1,
		0, 0, 1,
		0, 1, 1,
		1, 1, 1,
	};
	u8 uvs_source[2 * 4] = {
		0, 0,
		1, 0,
		1, 1,
		0, 1,
	};
	u8 quads_source[4 * 6] = {
		0, 1, 2, 3,
		4, 5, 6, 7,
		3, 2, 7, 6,
		5, 4, 1, 0,
		1, 4, 7, 2,
		5, 0, 3, 6,
	};

	Array<r32> vertices; vertices.set_capacity((3 + 2) * 4 * 6);
	Array<u8> indices; indices.set_capacity(3 * 2 * 6);

	u8 gi = 0;
	for (u8 fi = 0; fi < 6; ++fi) {
		u8 const * quad = quads_source + fi * 4;
		for (u8 qi = 0; qi < 4; ++qi) {
			u8 const * pos = positions_source + quad[qi] * 3;
			for (u8 vi = 0; vi < 3; ++vi) {
				vertices.push(pos[vi] - 0.5f);
			}
			u8 const * uv = uvs_source + qi * 2;
			for (u8 vi = 0; vi < 2; ++vi) {
				vertices.push(uv[vi]);
			}
		}
		indices.push(gi + 0); indices.push(gi + 1); indices.push(gi + 2);
		indices.push(gi + 2); indices.push(gi + 3); indices.push(gi + 0);
		gi += 4;
	}

	u8 const vertex_attributes[] = { /*position*/ 3, /*UV*/ 2, };

	bc->write(graphics::Instruction::Allocate_Mesh);
	bc->write(asset_id);
	bc->write((u8)2);
	bc->write((b8)false); bc->write(graphics::Mesh_Frequency::Static); bc->write(graphics::Mesh_Access::Draw);
	bc->write(graphics::Data_Type::r32); bc->write(vertices.count); bc->write(vertices.count);
	bc->write(vertex_attributes);
	bc->write((b8)true); bc->write(graphics::Mesh_Frequency::Static); bc->write(graphics::Mesh_Access::Draw);
	bc->write(graphics::Data_Type::u8); bc->write(indices.count); bc->write(indices.count);
	bc->write((u32)0);

	bc->write(graphics::Instruction::Load_Mesh);
	bc->write(asset_id);
	bc->write((u8)2);
	bc->write((u32)0); write_data_array_custom(vertices);
	bc->write((u32)0); write_data_array_custom(indices);

	return asset_id;
}

}}

//
// utilities
//

namespace custom {
namespace loader {

static void init_uniforms(void) {
	u32 const count = asset::uniform::count;

	bc->write(graphics::Instruction::Init_Uniforms);
	bc->write(count);

	for (u32 i = 0; i < count; ++i) {
		cstring name = asset::uniform::names[i];
		u32 length = (u32)strlen(name);
		bc->write(length + 1);
		bc->write(name, length); bc->write('\0');
	}
}

template<typename T, u32 count>
static void write_data_array(T const (& data)[count]) {
	bc->write(graphics::get_data_type<T>());
	bc->write(data);
}

template<typename T>
static void write_data_array_custom(custom::Array<T> const & data) {
	bc->write(graphics::get_data_type<T>());
	bc->write(data);
}

static void describe_texture(
	u32 asset_id, ivec2 size, u8 channels,
	graphics::Data_Type data_type, graphics::Texture_Type texture_type
) {
	u8 meta_id = asset::texture::meta_ids[asset_id];
	asset::texture::Meta const & meta = asset::texture::meta_presets[meta_id];
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
