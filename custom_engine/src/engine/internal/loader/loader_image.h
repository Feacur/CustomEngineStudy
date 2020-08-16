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

	u8 data_type_size = 0;
	switch (asset->data_type)
	{
		case custom::graphics::Data_Type::u8: data_type_size = sizeof(u8); break;
		case custom::graphics::Data_Type::u16: data_type_size = sizeof(u16); break;
		case custom::graphics::Data_Type::r32: data_type_size = sizeof(r32); break;
	}

	// @Note: allocate GPU memory, describe; might take it from some lightweight meta
	bc->write(graphics::Instruction::Allocate_Texture);
	bc->write((Ref const &)ref);

	// @Note: upload actual texture data; might stream it later
	bc->write(graphics::Instruction::Load_Texture);
	bc->write((Ref const &)ref);
}

}}
