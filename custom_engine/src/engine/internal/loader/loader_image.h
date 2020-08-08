#include <stb_image.h>

namespace custom {
namespace loader {

static void describe_texture(
	u32 asset_id, bool is_dynamic, ivec2 size, u8 channels,
	graphics::Data_Type data_type, graphics::Texture_Type texture_type
);

static void describe_texture_load(
	u32 asset_id, ivec2 offset, ivec2 size, u8 channels,
	graphics::Data_Type data_type, graphics::Texture_Type texture_type
);

void image(u32 asset_id) {
	if (graphics::mark_pending_texture(asset_id)) { return; }

	if (asset_id >= asset::texture::count) { return; }
	cstring path = asset::texture::paths[asset_id];

	Array<u8> file; file::read(path, file);
	if (file.count != file.capacity) { return; }

	ivec2 size;
	s32 channels;
	stbi_set_flip_vertically_on_load(1);
	stbi_uc * data = stbi_load_from_memory(file.data, file.count, &size.x, &size.y, &channels, 0);
	CUSTOM_ASSERT(data, "failed to read image '%s'", path);

	constexpr graphics::Data_Type const data_type = graphics::Data_Type::u8;
	constexpr graphics::Texture_Type const texture_type = graphics::Texture_Type::Color;

	// @Note: allocate GPU memory, describe; might take it from some lightweight meta
	describe_texture(asset_id, false, size, (u8)channels, data_type, texture_type);

	// @Note: upload actual texture data; might stream it later
	describe_texture_load(asset_id, {0, 0}, size, (u8)channels, data_type, texture_type);
	bc->write(data, size.x * size.y * channels);

	stbi_image_free(data);
}

void imagef(u32 asset_id) {
	if (graphics::mark_pending_texture(asset_id)) { return; }

	cstring path = asset::texture::paths[asset_id];
	Array<u8> file; file::read(path, file);
	if (file.count != file.capacity) { return; }

	ivec2 size;
	s32 channels;
	stbi_set_flip_vertically_on_load(1);
	float * data = stbi_loadf_from_memory(file.data, file.count, &size.x, &size.y, &channels, 0);
	CUSTOM_ASSERT(data, "failed to read image '%s'", path);

	constexpr graphics::Data_Type const data_type = graphics::Data_Type::r32;
	constexpr graphics::Texture_Type const texture_type = graphics::Texture_Type::Color;

	// @Note: allocate GPU memory, describe; might take it from some lightweight meta
	describe_texture(asset_id, false, size, (u8)channels, data_type, texture_type);

	// @Note: upload actual texture data; might stream it later
	describe_texture_load(asset_id, {0, 0}, size, (u8)channels, data_type, texture_type);
	bc->write(data, size.x * size.y * channels);

	stbi_image_free(data);
}

void image16(u32 asset_id) {
	if (graphics::mark_pending_texture(asset_id)) { return; }

	cstring path = asset::texture::paths[asset_id];
	Array<u8> file; file::read(path, file);
	if (file.count != file.capacity) { return; }

	ivec2 size;
	s32 channels;
	stbi_set_flip_vertically_on_load(1);
	stbi_us * data = stbi_load_16_from_memory(file.data, file.count, &size.x, &size.y, &channels, 0);
	CUSTOM_ASSERT(data, "failed to read image '%s'", path);

	constexpr graphics::Data_Type const data_type = graphics::Data_Type::u16;
	constexpr graphics::Texture_Type const texture_type = graphics::Texture_Type::Color;

	// @Note: allocate GPU memory, describe; might take it from some lightweight meta
	describe_texture(asset_id, false, size, (u8)channels, data_type, texture_type);

	// @Note: upload actual texture data; might stream it later
	describe_texture_load(asset_id, {0, 0}, size, (u8)channels, data_type, texture_type);
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
	u32 asset_id, bool is_dynamic, ivec2 size, u8 channels,
	graphics::Data_Type data_type, graphics::Texture_Type texture_type
) {
	u8 meta_id = asset::texture::meta_ids[asset_id];
	asset::texture::Meta const & meta = asset::texture::meta_presets[meta_id];
	bc->write(graphics::Instruction::Allocate_Texture);
	bc->write(asset_id);
	bc->write((b8)is_dynamic);
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
	bc->write(graphics::Instruction::Load_Texture);
	bc->write(asset_id);
	bc->write(offset);
	bc->write(size);
	bc->write(channels);
	bc->write(data_type);
	bc->write(texture_type);
}

}}
