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

	Texture_Asset const * asset = ref.get_fast();
	if (!asset->data) { return; }

	// @Note: allocate GPU memory, describe; might take it from some lightweight meta
	describe_texture(ref.id, *asset, false, asset->size, (u8)asset->channels, asset->data_type, asset->texture_type);

	// @Note: upload actual texture data; might stream it later
	describe_texture_load(ref.id, {0, 0}, asset->size, (u8)asset->channels, asset->data_type, asset->texture_type);
	bc->write(asset->data, asset->size.x * asset->size.y * asset->channels);
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
