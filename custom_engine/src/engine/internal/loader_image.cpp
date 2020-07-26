#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/core/collection_types.h"
#include "engine/core/math_types.h"
#include "engine/debug/log.h"
#include "engine/api/internal/bytecode.h"
#include "engine/api/platform/file.h"
#include "engine/api/client/asset_lookup.h"
#include "engine/api/graphics_params.h"
#include "engine/api/resource.h"

#include <stb_image.h>

namespace custom {
namespace loader {

static void describe_texture(
	Bytecode * bc,
	u32 asset_id, ivec2 size, u8 channels,
	graphics::Data_Type data_type, graphics::Texture_Type texture_type
);

static void describe_texture_load(
	Bytecode * bc,
	u32 asset_id, ivec2 offset, ivec2 size, u8 channels,
	graphics::Data_Type data_type, graphics::Texture_Type texture_type
);

void image(Bytecode * bc, u32 asset_id) {
	if (has_texture(asset_id)) { return; }
	if (asset_id >= asset::texture::count) { return; }
	cstring path = asset::texture::paths[asset_id];

	Array<u8> file; file::read(path, file);
	if (file.count != file.capacity) { return; }

	stbi_set_flip_vertically_on_load(1);

	ivec2 size;
	s32 channels;
	stbi_uc * data = stbi_load_from_memory(file.data, file.count, &size.x, &size.y, &channels, 0);
	CUSTOM_ASSERT(data, "failed to read image '%s'", path);

	constexpr graphics::Data_Type const data_type = graphics::Data_Type::u8;
	constexpr graphics::Texture_Type const texture_type = graphics::Texture_Type::Color;

	// @Note: allocate GPU memory, describe; might take it from some lightweight meta
	describe_texture(bc, asset_id, size, (u8)channels, data_type, texture_type);

	// @Note: upload actual texture data; might stream it later
	describe_texture_load(bc, asset_id, {0, 0}, size, (u8)channels, data_type, texture_type);
	bc->write(data, size.x * size.y * channels);

	stbi_image_free(data);
}

void imagef(Bytecode * bc, u32 asset_id) {
	if (has_texture(asset_id)) { return; }

	cstring path = asset::texture::paths[asset_id];
	Array<u8> file; file::read(path, file);
	if (file.count != file.capacity) { return; }

	stbi_set_flip_vertically_on_load(1);

	ivec2 size;
	s32 channels;
	float * data = stbi_loadf_from_memory(file.data, file.count, &size.x, &size.y, &channels, 0);
	CUSTOM_ASSERT(data, "failed to read image '%s'", path);

	constexpr graphics::Data_Type const data_type = graphics::Data_Type::r32;
	constexpr graphics::Texture_Type const texture_type = graphics::Texture_Type::Color;

	// @Note: allocate GPU memory, describe; might take it from some lightweight meta
	describe_texture(bc, asset_id, size, (u8)channels, data_type, texture_type);

	// @Note: upload actual texture data; might stream it later
	describe_texture_load(bc, asset_id, {0, 0}, size, (u8)channels, data_type, texture_type);
	bc->write(data, size.x * size.y * channels);

	stbi_image_free(data);
}

void image16(Bytecode * bc, u32 asset_id) {
	if (has_texture(asset_id)) { return; }

	cstring path = asset::texture::paths[asset_id];
	Array<u8> file; file::read(path, file);
	if (file.count != file.capacity) { return; }

	stbi_set_flip_vertically_on_load(1);

	ivec2 size;
	s32 channels;
	stbi_us * data = stbi_load_16_from_memory(file.data, file.count, &size.x, &size.y, &channels, 0);
	CUSTOM_ASSERT(data, "failed to read image '%s'", path);

	constexpr graphics::Data_Type const data_type = graphics::Data_Type::u16;
	constexpr graphics::Texture_Type const texture_type = graphics::Texture_Type::Color;

	// @Note: allocate GPU memory, describe; might take it from some lightweight meta
	describe_texture(bc, asset_id, size, (u8)channels, data_type, texture_type);

	// @Note: upload actual texture data; might stream it later
	describe_texture_load(bc, asset_id, {0, 0}, size, (u8)channels, data_type, texture_type);
	bc->write(data, size.x * size.y * channels);

	stbi_image_free(data);
}

}}

//
// utilities
//

namespace custom {
namespace loader {

static void describe_texture(
	Bytecode * bc,
	u32 asset_id, ivec2 size, u8 channels,
	graphics::Data_Type data_type, graphics::Texture_Type texture_type
) {	
	u8 meta_id = asset::texture::meta_ids[asset_id];
	asset::texture::Meta const & meta = asset::texture::meta_presets[meta_id];
	bc->write(graphics::Instruction::Allocate_Texture);
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
	Bytecode * bc,
	u32 asset_id, ivec2 offset, ivec2 size, u8 channels,
	graphics::Data_Type data_type, graphics::Texture_Type texture_type
) {
	bc->write(graphics::Instruction::Load_Texture);
	bc->write(asset_id);
	bc->write(offset);
	bc->write(size);
	bc->write(channels);
	bc->write(data_type);
	bc->write(texture_type);
}

}}
