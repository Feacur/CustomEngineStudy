#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/core/collection_types.h"
#include "engine/core/math_types.h"
#include "engine/debug/log.h"
#include "engine/api/platform/file.h"
#include "engine/api/platform/graphics_resource.h"
#include "engine/api/internal/types_names_lookup.h"
#include "engine/api/internal/asset_types.h"
#include "engine/api/graphics_params.h"
#include "engine/api/internal/entity_system.h"
#include "engine/impl/array_fixed.h"
#include "engine/impl/bytecode.h"
#include "engine/impl/asset_system.h"

#include "obj_parser.h"
#include <stb_image.h>
#include <lua.hpp>

namespace custom {

template<> VOID_DREF_FUNC(asset_pool_load<Lua_Asset>) {
	RefT<Lua_Asset> & refT = (RefT<Lua_Asset> &)ref;
	if (!refT.exists()) { CUSTOM_ASSERT(false, "Lua asset doesn't exist"); return; }

	cstring path = Asset::get_path(refT);
	if (!file::exists(path)) { CUSTOM_ASSERT(false, "file doesn't exist '%s'", path); return; }

	Array<u8> file; file::read(path, file);
	if (!file.count) { return; }

	Lua_Asset * asset = refT.get_fast();
	asset->source.data = file.data; file.data = NULL;
	asset->source.count = file.count; file.count = 0;
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

template<> VOID_DREF_FUNC(asset_pool_load<Shader_Asset>) {
	RefT<Shader_Asset> & refT = (RefT<Shader_Asset> &)ref;
	if (!refT.exists()) { CUSTOM_ASSERT(false, "shader asset doesn't exist"); return; }

	cstring path = Asset::get_path(refT);
	if (!file::exists(path)) { CUSTOM_ASSERT(false, "file doesn't exist '%s'", path); return; }

	Array<u8> file; file::read(path, file);
	if (!file.count) { return; }

	Shader_Asset * asset = refT.get_fast();
	asset->source.data = file.data; file.data = NULL;
	asset->source.count = file.count; file.count = 0;
	asset->source.capacity = file.capacity; file.capacity = 0;
}

template<> VOID_DREF_FUNC(asset_pool_unload<Shader_Asset>) {
	RefT<Shader_Asset> & refT = (RefT<Shader_Asset> &)ref;
	if (!refT.exists()) { CUSTOM_ASSERT(false, "shader asset doesn't exist"); return; }

	Shader_Asset * asset = refT.get_fast();
	asset->source.set_capacity(0);
}

}

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

template<> VOID_DREF_FUNC(asset_pool_load<Mesh_Asset>) {
	RefT<Mesh_Asset> & refT = (RefT<Mesh_Asset> &)ref;
	if (!refT.exists()) { CUSTOM_ASSERT(false, "mesh asset doesn't exist"); return; }

	cstring path = Asset::get_path(refT);
	if (!file::exists(path)) { CUSTOM_ASSERT(false, "file doesn't exist '%s'", path); return; }

	Array<u8> file; file::read(path, file);
	if (!file.count) { return; }

	Mesh_Asset * asset = refT.get_fast();
	obj::parse(file, asset->attributes, asset->vertices, asset->indices);
}

template<> VOID_DREF_FUNC(asset_pool_unload<Mesh_Asset>) {
	RefT<Mesh_Asset> & refT = (RefT<Mesh_Asset> &)ref;
	if (!refT.exists()) { CUSTOM_ASSERT(false, "mesh asset doesn't exist"); return; }

	Mesh_Asset * asset = refT.get_fast();
	asset->attributes.set_capacity(0);
	asset->vertices.set_capacity(0);
	asset->indices.set_capacity(0);
}

}

namespace custom {
namespace loader {

static Bytecode * bc = NULL;

void init(Bytecode * bytecode) {
	bc = bytecode;
}

}}

#include "loader_lua.h"
#include "loader_shader.h"
#include "loader_image.h"
#include "loader_obj.h"
