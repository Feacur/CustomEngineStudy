namespace custom {
namespace loader {

void uniforms() {
	if (graphics::mark_pending_uniforms()) { return; }

	bc->write(graphics::Instruction::Init_Uniforms);
	bc->write((u32)custom::uniform_names.count);

	for (u32 i = 0; i < (u32)custom::uniform_names.count; ++i) {
		cstring name = custom::uniform_names[i];
		u32 length = (u32)strlen(name);
		bc->write_sized_array(name, length);
	}
}

void shader(RefT<Shader_Asset> const & ref) {
	if (graphics::mark_pending_shader(ref.id)) { return; }

	Shader_Asset const * asset = ref.get_fast();
	if (!asset->source.count) { return; }

	bc->write(graphics::Instruction::Allocate_Shader);
	bc->write(ref);
	
	bc->write(graphics::Instruction::Load_Shader);
	bc->write(ref);
}

/*
void offscreen(u32 asset_id) {
	if (graphics::mark_pending_shader(asset_id)) { return; }

	if (asset_id <= asset::texture::count) { return; }

	// @Todo:
	ivec2 size {0, 0};
	s32 channels = 4;

	constexpr graphics::Data_Type const data_type = graphics::Data_Type::u8;
	constexpr graphics::Texture_Type const texture_type = graphics::Texture_Type::Color;

	u8 meta_id = asset::texture::meta_ids[asset_id];
	asset::texture::Meta const & meta = asset::texture::meta_presets[meta_id];
	bc->write(graphics::Instruction::Allocate_Texture);
	bc->write(asset_id);
	bc->write((b8)true);
	bc->write(size);
	bc->write(channels);
	bc->write(data_type);
	bc->write(texture_type);
	bc->write(meta.min_tex);
	bc->write(meta.min_mip);
	bc->write(meta.mag_tex);
	bc->write(meta.wrap_x); bc->write(meta.wrap_y);
}

void target(u32 asset_id) {
	if (graphics::mark_pending_target(asset_id)) { return; }

	u8 meta_id = asset::target::meta_ids[asset_id];
	asset::target::Meta const & meta = asset::target::meta_presets[meta_id];

	// @Todo:
	loader::offscreen(meta.texture_id);

	bc->write(graphics::Instruction::Allocate_Target);
	bc->write((u16)1); bc->write(meta.texture_id);
	bc->write((u16)1); bc->write(meta.size);
	bc->write(meta.data_type); bc->write(meta.texture_type);
}
*/

}}
