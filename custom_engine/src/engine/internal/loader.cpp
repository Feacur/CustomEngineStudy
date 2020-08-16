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
	asset->source.data = file.data; file.data = NULL;
	asset->source.capacity = file.capacity; file.capacity = 0;
	asset->source.count = file.count; file.count = 0;
}

template<> VOID_DREF_FUNC(asset_pool_unload<Shader_Asset>) {
	RefT<Shader_Asset> & refT = (RefT<Shader_Asset> &)ref;
	if (!refT.exists()) { CUSTOM_ASSERT(false, "shader asset doesn't exist"); return; }

	Shader_Asset * asset = refT.get_fast();
	asset->source.set_capacity(0);
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

void shader(RefT<Shader_Asset> const & ref) {
	if (graphics::mark_pending_shader(ref.id)) { return; }

	Shader_Asset const * asset = ref.get_fast();
	if (!asset->source.count) { return; }

	bc->write(graphics::Instruction::Allocate_Shader);
	bc->write((Ref const &)ref);
	
	bc->write(graphics::Instruction::Load_Shader);
	bc->write((Ref const &)ref);
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

	stbi_set_flip_vertically_on_load(1);
	asset->data = stbi_load_from_memory(file.data, file.count, &asset->size.x, &asset->size.y, &asset->channels, 0);
}

template<> VOID_DREF_FUNC(asset_pool_unload<Texture_Asset>) {
	RefT<Texture_Asset> & refT = (RefT<Texture_Asset> &)ref;
	if (!refT.exists()) { CUSTOM_ASSERT(false, "texture asset doesn't exist"); return; }

	Texture_Asset * asset = refT.get_fast();
	stbi_image_free(asset->data);
}

}

namespace custom {
namespace loader {

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
}

template<> VOID_DREF_FUNC(asset_pool_unload<Mesh_Asset>) {
	RefT<Mesh_Asset> & refT = (RefT<Mesh_Asset> &)ref;
	if (!refT.exists()) { CUSTOM_ASSERT(false, "mesh asset doesn't exist"); return; }

	Mesh_Asset * asset = refT.get_fast();
	for (u32 i = 0; i < asset->buffers.count; ++i) {
		asset->buffers[i].attributes.set_capacity(0);
		asset->buffers[i].buffer.set_capacity(0);
	}
	asset->buffers.set_capacity(0);
}

}

namespace custom {
namespace loader {

template<typename T>
static void write_data_array(custom::Array<T> const & data);

void mesh(RefT<Mesh_Asset> const & ref) {
	if (graphics::mark_pending_mesh(ref.id)) { return; }

	Mesh_Asset const * asset = ref.get_fast();
	if (!asset->buffers.count) { return; }

	bc->write(graphics::Instruction::Allocate_Mesh);
	bc->write((Ref const &)ref);

	bc->write(graphics::Instruction::Load_Mesh);
	bc->write((Ref const &)ref);
}

}}
