#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/core/collection_types.h"
#include "engine/core/math_types.h"
#include "engine/debug/log.h"
#include "engine/api/platform/file.h"
#include "engine/api/platform/graphics_resource.h"
#include "engine/api/internal/types_names_lookup.h"
#include "engine/api/internal/asset_types.h"
#include "engine/impl/array_fixed.h"
#include "engine/impl/bytecode.h"
#include "engine/impl/asset_system.h"

#include <new>

#include <stb_image.h>
#include <lua.hpp>

#include "obj_parser.h"

//
//
//

namespace custom {
namespace loader {

static Bytecode * bc = NULL;

void init(Bytecode * bytecode) {
	bc = bytecode;
}

}}

//
// Lua_Asset
//

namespace custom {

template<> VOID_DREF_FUNC(asset_pool_load<Lua_Asset>) {
	RefT<Lua_Asset> & refT = (RefT<Lua_Asset> &)ref;
	if (!refT.exists()) { CUSTOM_ASSERT(false, "Lua asset doesn't exist"); return; }

	cstring path = Asset::get_path(refT);
	if (!file::exists(path)) { CUSTOM_ASSERT(false, "file doesn't exist '%s'", path); return; }

	Array<u8> file; file::read(path, file);
	if (!file.count) { return; }

	Lua_Asset * asset = refT.get_fast();
	new (asset) Lua_Asset;

	asset->source.data     = file.data;     file.data     = NULL;
	asset->source.count    = file.count;    file.count    = 0;
	asset->source.capacity = file.capacity; file.capacity = 0;
}

template<> VOID_DREF_FUNC(asset_pool_unload<Lua_Asset>) {
	RefT<Lua_Asset> & refT = (RefT<Lua_Asset> &)ref;
	if (!refT.exists()) { CUSTOM_ASSERT(false, "Lua asset doesn't exist"); return; }

	Lua_Asset * asset = refT.get_fast();
	asset->source.set_capacity(0);
}

}

namespace custom {
namespace loader {

// @Note: alternative to `luaL_dostring(L, (cstring)asset->source.data)`
#define CUSTOM_LOAD() (\
	luaL_loadbufferx(L, (cstring)asset->source.data, asset->source.count, path, NULL)\
	|| lua_pcall(L, 0, LUA_MULTRET, 0)\
)\

void script(lua_State * L, RefT<Lua_Asset> const & ref) {
	if (!ref.exists()) { CUSTOM_ASSERT(false, "asset doesn't exist"); return; }

	Lua_Asset const * asset = ref.get_fast();
	if (!asset->source.count) { return; }

	cstring path = Asset::get_path(ref);
	if (CUSTOM_LOAD() != LUA_OK) {
		CUSTOM_ERROR("lua: '%s'", lua_tostring(L, -1));
		lua_pop(L, 1);
	}
}
#undef CUSTOM_LOAD

}}

//
// Shader_Asset
//

namespace custom {

template<> VOID_DREF_FUNC(asset_pool_load<Shader_Asset>) {
	RefT<Shader_Asset> & refT = (RefT<Shader_Asset> &)ref;
	if (!refT.exists()) { CUSTOM_ASSERT(false, "shader asset doesn't exist"); return; }

	cstring path = Asset::get_path(refT);
	if (!file::exists(path)) { CUSTOM_ASSERT(false, "file doesn't exist '%s'", path); return; }

	Array<u8> file; file::read(path, file);
	if (!file.count) { return; }

	Shader_Asset * asset = refT.get_fast();
	new (asset) Shader_Asset;

	asset->source.data = file.data; file.data = NULL;
	asset->source.capacity = file.capacity; file.capacity = 0;
	asset->source.count = file.count; file.count = 0;

	// @Note: direct asset to the GVM
	if (graphics::mark_pending_shader(ref.id)) {
		CUSTOM_ASSERT(false, "shader resource already exists");
		custom::loader::bc->write(graphics::Instruction::Free_Shader);
		custom::loader::bc->write((Ref const &)ref);
	}

	custom::loader::bc->write(graphics::Instruction::Allocate_Shader);
	custom::loader::bc->write((Ref const &)ref);
	
	custom::loader::bc->write(graphics::Instruction::Load_Shader);
	custom::loader::bc->write((Ref const &)ref);
}

template<> VOID_DREF_FUNC(asset_pool_unload<Shader_Asset>) {
	RefT<Shader_Asset> & refT = (RefT<Shader_Asset> &)ref;
	if (!refT.exists()) { CUSTOM_ASSERT(false, "shader asset doesn't exist"); return; }

	Shader_Asset * asset = refT.get_fast();
	asset->~Shader_Asset();

	// @Note: remove asset from the GVM
	custom::loader::bc->write(graphics::Instruction::Free_Shader);
	custom::loader::bc->write((Ref const &)ref);
}

}

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

}}

//
// Texture_Asset
//

namespace custom {

template<> VOID_DREF_FUNC(asset_pool_load<Texture_Asset>) {
	RefT<Texture_Asset> & refT = (RefT<Texture_Asset> &)ref;
	if (!refT.exists()) { CUSTOM_ASSERT(false, "texture asset doesn't exist"); return; }

	cstring path = Asset::get_path(refT);
	if (!file::exists(path)) { CUSTOM_ASSERT(false, "file doesn't exist '%s'", path); return; }

	Array<u8> file; file::read(path, file);
	if (!file.count) { return; }

	Texture_Asset * asset = refT.get_fast();
	new (asset) Texture_Asset;

	stbi_set_flip_vertically_on_load(1);
	stbi_uc * data = stbi_load_from_memory(file.data, file.count, &asset->size.x, &asset->size.y, &asset->channels, 0);

	u8 data_type_size = 0;
	switch (asset->data_type)
	{
		case custom::graphics::Data_Type::u8: data_type_size = sizeof(u8); break;
		case custom::graphics::Data_Type::u16: data_type_size = sizeof(u16); break;
		case custom::graphics::Data_Type::r32: data_type_size = sizeof(r32); break;
	}

	asset->data.data = data;
	asset->data.capacity = asset->size.x * asset->size.y * asset->channels * data_type_size;
	asset->data.capacity = asset->data.capacity;

	// @Note: direct asset to the GVM
	if (graphics::mark_pending_texture(ref.id)) {
		CUSTOM_ASSERT(false, "texture resource already exists");
		custom::loader::bc->write(graphics::Instruction::Free_Texture);
		custom::loader::bc->write((Ref const &)ref);
	}

	custom::loader::bc->write(graphics::Instruction::Allocate_Texture);
	custom::loader::bc->write((Ref const &)ref);

	custom::loader::bc->write(graphics::Instruction::Load_Texture);
	custom::loader::bc->write((Ref const &)ref);
}

template<> VOID_DREF_FUNC(asset_pool_unload<Texture_Asset>) {
	RefT<Texture_Asset> & refT = (RefT<Texture_Asset> &)ref;
	if (!refT.exists()) { CUSTOM_ASSERT(false, "texture asset doesn't exist"); return; }

	Texture_Asset * asset = refT.get_fast();
	asset->~Texture_Asset();

	// @Note: remove asset from the GVM
	custom::loader::bc->write(graphics::Instruction::Free_Texture);
	custom::loader::bc->write((Ref const &)ref);
}

}

//
// Mesh_Asset
//

namespace custom {

template<> VOID_DREF_FUNC(asset_pool_load<Mesh_Asset>) {
	RefT<Mesh_Asset> & refT = (RefT<Mesh_Asset> &)ref;
	if (!refT.exists()) { CUSTOM_ASSERT(false, "mesh asset doesn't exist"); return; }

	cstring path = Asset::get_path(refT);
	if (!file::exists(path)) { CUSTOM_ASSERT(false, "file doesn't exist '%s'", path); return; }

	Array<u8> file; file::read(path, file);
	if (!file.count) { return; }

	Array<u8> attributes;
	Array<r32> vertices;
	Array<u32> indices;
	obj::parse(file, attributes, vertices, indices);

	Mesh_Asset * asset = refT.get_fast();
	new (asset) Mesh_Asset;

	asset->buffers.set_capacity(2);

	{
		asset->buffers.push();
		Mesh_Asset::Buffer & buffer = asset->buffers[0];
		new (&buffer) Mesh_Asset::Buffer;
	
		buffer.attributes.data     = attributes.data;     attributes.data     = NULL;
		buffer.attributes.capacity = attributes.capacity; attributes.capacity = 0;
		buffer.attributes.count    = attributes.count;    attributes.count    = 0;

		buffer.buffer.data     = (u8 *)vertices.data;             vertices.data     = NULL;
		buffer.buffer.capacity = vertices.capacity * sizeof(r32); vertices.capacity = 0;
		buffer.buffer.count    = vertices.count * sizeof(r32);    vertices.count    = 0;

		buffer.data_type = graphics::Data_Type::r32;
		buffer.is_index = false;
	}

	{
		asset->buffers.push();
		Mesh_Asset::Buffer & buffer = asset->buffers[1];
		new (&buffer) Mesh_Asset::Buffer;

		buffer.attributes.data     = NULL;
		buffer.attributes.capacity = 0;
		buffer.attributes.count    = 0;

		buffer.buffer.data     = (u8 *)indices.data;             indices.data     = NULL;
		buffer.buffer.capacity = indices.capacity * sizeof(u32); indices.capacity = 0;
		buffer.buffer.count    = indices.count * sizeof(u32);    indices.count    = 0;

		buffer.data_type = graphics::Data_Type::u32;
		buffer.is_index = true;
	}

	// @Note: direct asset to the GVM
	if (graphics::mark_pending_mesh(ref.id)) {
		CUSTOM_ASSERT(false, "mesh resource already exists");
		custom::loader::bc->write(graphics::Instruction::Free_Mesh);
		custom::loader::bc->write((Ref const &)ref);
	}

	custom::loader::bc->write(graphics::Instruction::Allocate_Mesh);
	custom::loader::bc->write((Ref const &)ref);

	custom::loader::bc->write(graphics::Instruction::Load_Mesh);
	custom::loader::bc->write((Ref const &)ref);
}

template<> VOID_DREF_FUNC(asset_pool_unload<Mesh_Asset>) {
	RefT<Mesh_Asset> & refT = (RefT<Mesh_Asset> &)ref;
	if (!refT.exists()) { CUSTOM_ASSERT(false, "mesh asset doesn't exist"); return; }

	Mesh_Asset * asset = refT.get_fast();
	asset->~Mesh_Asset();

	// @Note: remove asset from the GVM
	custom::loader::bc->write(graphics::Instruction::Free_Mesh);
	custom::loader::bc->write((Ref const &)ref);
}

}
