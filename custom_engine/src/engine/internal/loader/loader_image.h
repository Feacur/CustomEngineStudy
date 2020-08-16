#include <stb_image.h>

namespace custom {
namespace loader {

static void describe_texture(
	u32 asset_id, Texture_Asset const & asset, bool is_dynamic, ivec2 size, u8 channels,
	graphics::Data_Type data_type, graphics::Texture_Type texture_type
);

static void describe_texture_load(
	u32 asset_id, ivec2 offset, ivec2 size, u8 channels,
	graphics::Data_Type data_type, graphics::Texture_Type texture_type
);

void image(RefT<Texture_Asset> const & ref) {
	if (graphics::mark_pending_texture(ref.id)) { return; }

	if (!ref.exists()) { CUSTOM_ASSERT(false, "asset doesn't exist"); return; }
	Texture_Asset const * asset = ref.get_fast();
	cstring path = Asset::get_path(ref);

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
	describe_texture(ref.id, *asset, false, size, (u8)channels, data_type, texture_type);

	// @Note: upload actual texture data; might stream it later
	describe_texture_load(ref.id, {0, 0}, size, (u8)channels, data_type, texture_type);
	bc->write(data, size.x * size.y * channels);

	stbi_image_free(data);
}

void imagef(RefT<Texture_Asset> const & ref) {
	if (graphics::mark_pending_texture(ref.id)) { return; }

	if (!ref.exists()) { CUSTOM_ASSERT(false, "asset doesn't exist"); return; }
	Texture_Asset const * asset = ref.get_fast();
	cstring path = Asset::get_path(ref);

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
	describe_texture(ref.id, *asset, false, size, (u8)channels, data_type, texture_type);

	// @Note: upload actual texture data; might stream it later
	describe_texture_load(ref.id, {0, 0}, size, (u8)channels, data_type, texture_type);
	bc->write(data, size.x * size.y * channels);

	stbi_image_free(data);
}

void image16(RefT<Texture_Asset> const & ref) {
	if (graphics::mark_pending_texture(ref.id)) { return; }

	if (!ref.exists()) { CUSTOM_ASSERT(false, "asset doesn't exist"); return; }
	Texture_Asset const * asset = ref.get_fast();
	cstring path = Asset::get_path(ref);

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
	describe_texture(ref.id, *asset, false, size, (u8)channels, data_type, texture_type);

	// @Note: upload actual texture data; might stream it later
	describe_texture_load(ref.id, {0, 0}, size, (u8)channels, data_type, texture_type);
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
	u32 asset_id, Texture_Asset const & asset, bool is_dynamic, ivec2 size, u8 channels,
	graphics::Data_Type data_type, graphics::Texture_Type texture_type
) {
	bc->write(graphics::Instruction::Allocate_Texture);
	bc->write(asset_id);
	bc->write((b8)is_dynamic);
	bc->write(size);
	bc->write(channels);
	bc->write(data_type);
	bc->write(texture_type);
	bc->write(asset.min_tex);
	bc->write(asset.min_mip);
	bc->write(asset.mag_tex);
	bc->write(asset.wrap_x); bc->write(asset.wrap_y);
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
