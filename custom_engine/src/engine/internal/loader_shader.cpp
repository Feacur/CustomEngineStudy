#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/core/collection_types.h"
#include "engine/core/math_types.h"
#include "engine/debug/log.h"
#include "engine/api/platform/file.h"
#include "engine/api/platform/graphics_resource.h"
#include "engine/api/client/asset_lookup.h"
#include "engine/api/graphics_params.h"
#include "engine/impl/bytecode.h"

namespace custom {
namespace loader {

void uniforms(Bytecode * bc) {
	u32 const count = asset::uniform::count;

	bc->write(graphics::Instruction::Init_Uniforms);
	bc->write(count);

	for (u32 i = 0; i < count; ++i) {
		cstring name = asset::uniform::names[i];
		u32 length = (u32)strlen(name);
		bc->write_sized_array(name, length);
	}
}

void shader(Bytecode * bc, u32 asset_id) {
	if (asset_id >= asset::shader::count) { return; }
	cstring path = asset::shader::paths[asset_id];

	Array<u8> file; file::read(path, file);
	if (file.count != file.capacity) { return; }

	u8 meta_id = asset::shader::meta_ids[asset_id];
	asset::shader::Meta const & meta = asset::shader::meta_presets[meta_id];

	if (!graphics::is_allocated_shader(asset_id)) {
		bc->write(graphics::Instruction::Allocate_Shader);
		bc->write(asset_id);
	}
	
	if (!graphics::is_uploaded_shader(asset_id)) {
		bc->write(graphics::Instruction::Load_Shader);
		bc->write(asset_id);
		bc->write_sized_array(file.data, file.count);
		bc->write(meta.parts);
	}
}

}}
