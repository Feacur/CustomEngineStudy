namespace custom {
namespace loader {

void uniforms() {
	if (graphics::is_inited_uniforms()) { return; }

	bc->write(graphics::Instruction::Init_Uniforms);
	bc->write((u32)asset::uniform::count);

	for (u32 i = 0; i < (u32)asset::uniform::count; ++i) {
		cstring name = asset::uniform::names[i];
		u32 length = (u32)strlen(name);
		bc->write_sized_array(name, length);
	}
}

void shader(u32 asset_id) {
	if (graphics::is_allocated_shader(asset_id) && graphics::is_uploaded_shader(asset_id)) { return; }

	if (asset_id >= asset::shader::count) { return; }
	cstring path = asset::shader::paths[asset_id];

	Array<u8> file; file::read(path, file);
	if (file.count != file.capacity) { return; }

	u8 meta_id = asset::shader::meta_ids[asset_id];
	asset::shader::Meta const & meta = asset::shader::meta_presets[meta_id];

	bc->write(graphics::Instruction::Allocate_Shader);
	bc->write(asset_id);
	
	bc->write(graphics::Instruction::Load_Shader);
	bc->write(asset_id);
	bc->write_sized_array(file.data, file.count);
	bc->write(meta.parts);
}

}}
