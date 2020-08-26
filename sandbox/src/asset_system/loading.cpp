#include "engine/core/code.h"
#include "engine/core/collection_types.h"
#include "engine/core/math_types.h"
#include "engine/debug/log.h"
#include "engine/api/platform/file.h"
#include "engine/api/internal/component_types.h"
#include "engine/impl/array.h"
#include "engine/impl/asset_system.h"

#include <new>
#include <lua.hpp>

#include "asset_types.h"
#include "prefab_parser.h"
#include "../entity_system/component_types.h"

//
//
//

static lua_State * L = NULL;

void init_client_loader(lua_State * lua) {
	L = lua;
}

//
// Lua_Asset
//

namespace custom {
namespace loading {

template<> LOADING_FUNC(asset_pool_load<Lua_Asset>) {
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

	// @Note: direct asset to the Lua
	#define CUSTOM_LOAD() (\
		luaL_loadbufferx(L, (cstring)asset->source.data, asset->source.count, path, NULL)\
		|| lua_pcall(L, 0, LUA_MULTRET, 0)\
	)\

	// @Note: alternative to `luaL_dostring(L, (cstring)asset->source.data)`
	if (CUSTOM_LOAD() != LUA_OK) {
		CUSTOM_ERROR("lua: '%s'", lua_tostring(L, -1));
		lua_pop(L, 1);
	}

	// @Todo: implement load/unload
	asset->~Lua_Asset();
}

template<> LOADING_FUNC(asset_pool_unload<Lua_Asset>) {
	RefT<Lua_Asset> & refT = (RefT<Lua_Asset> &)ref;
	if (!refT.exists()) { CUSTOM_ASSERT(false, "Lua asset doesn't exist"); return; }

	Lua_Asset * asset = refT.get_fast();
	asset->~Lua_Asset();
}

}}

//
// Prefab
//

namespace custom {
namespace loading {

template<> LOADING_FUNC(asset_pool_load<Prefab>) {
	RefT<Prefab> & refT = (RefT<Prefab> &)ref;
	if (!refT.exists()) { CUSTOM_ASSERT(false, "Lua asset doesn't exist"); return; }

	cstring path = Asset::get_path(refT);
	if (!file::exists(path)) { CUSTOM_ASSERT(false, "file doesn't exist '%s'", path); return; }

	Array<u8> file; file::read(path, file);
	if (!file.count) { return; }

	Prefab * asset = refT.get_fast();
	new (asset) Prefab;

	// @Note: parse asset
	sandbox::prefab::parse(*asset, file);

	*asset = {custom::Entity::create(false)};

	RefT<Transform> transform = asset->add_component<Transform>();
	(*transform.get_fast()) = {
		{0,1,0}, {0,0,0,1}, {1,1,1}
	};
	RefT<Visual> visual = asset->add_component<Visual>();
	(*visual.get_fast()) = {
		Asset::add<Shader_Asset>("assets/shaders/v3_texture_tint.glsl", true),
		Asset::add<Texture_Asset>("assets/textures/checkerboard.png", true),
		Asset::add<Mesh_Asset>("assets/meshes/suzanne.obj", true),
	};
	RefT<Lua_Script> lua_script = asset->add_component<Lua_Script>();
	lua_script.get_fast()->update = "script_rotate";
}

template<> LOADING_FUNC(asset_pool_unload<Prefab>) {
	RefT<Prefab> & refT = (RefT<Prefab> &)ref;
	if (!refT.exists()) { CUSTOM_ASSERT(false, "Lua asset doesn't exist"); return; }

	Prefab * asset = refT.get_fast();
	asset->destroy();
	asset->~Prefab();
}

}}
