#include "engine/core/code.h"
#include "engine/core/collection_types.h"
#include "engine/debug/log.h"
#include "engine/api/platform/file.h"
#include "engine/impl/array.h"
#include "engine/impl/asset_system.h"

// #include <new>
#include <lua.hpp>

#include "asset_types.h"
#include "../entity_system/component_types.h"

namespace custom {

// @Todo: revisit
static void read_file_safely(cstring path, Array<u8> & buffer) {
	constexpr u32 count = 4;
	if (!file::get_time(path)) { CUSTOM_ASSERT(false, "file doesn't exist '%s'", path); return; }
	for (u32 i = 0; i < count; ++i) {
		if (file::read(path, buffer)) { return; }
	}
	CUSTOM_ASSERT(false, "failed to read file safely: '%s'", path);
}

}

// @Note: direct asset to the Lua
//        alternative to `luaL_dostring(L, (cstring)asset->source.data)`,
//        assuming NULL-termination is there
#define CUSTOM_LOAD() (                                                               \
    luaL_loadbufferx(L, (cstring)asset->source.data, asset->source.count, path, NULL) \
    || lua_pcall(L, 0, LUA_MULTRET, 0)                                                \
)                                                                                     \

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
	if (!asset_ref.exists()) { CUSTOM_ASSERT(false, "Lua asset doesn't exist"); return; }

	RefT<Lua_Asset> & refT = (RefT<Lua_Asset> &)asset_ref;
	if (!refT.exists()) { CUSTOM_ASSERT(false, "asset doesn exist"); }

	//
	cstring path = asset_ref.get_path();
	Array<u8> file; read_file_safely(path, file);
	if (!file.count) { return; }

	Lua_Asset * asset = refT.get_fast();
	asset->update(file);

	// @Note: direct the asset to Lua
	if (CUSTOM_LOAD() != LUA_OK) {
		CUSTOM_ERROR("lua: '%s'", lua_tostring(L, -1));
		lua_pop(L, 1);
	}

	// @Todo: implement load/unload
	// @Note: memory might have been relocated
	if (!refT.exists()) { CUSTOM_ASSERT(false, "asset doesn exist"); }
	asset = refT.get_fast();
	asset->~Lua_Asset();
}

template<> LOADING_FUNC(asset_pool_unload<Lua_Asset>) {
	if (!asset_ref.exists()) { CUSTOM_ASSERT(false, "Lua asset doesn't exist"); return; }

	RefT<Lua_Asset> & refT = (RefT<Lua_Asset> &)asset_ref;
	if (!refT.exists()) { CUSTOM_ASSERT(false, "asset doesn exist"); }

	//
	Lua_Asset * asset = refT.get_fast();
	asset->~Lua_Asset();

	// @Todo: unload Lua's chunk if possible?
}

template<> LOADING_FUNC(asset_pool_update<Lua_Asset>) {
	if (!asset_ref.exists()) { CUSTOM_ASSERT(false, "Lua asset doesn't exist"); return; }

	RefT<Lua_Asset> & refT = (RefT<Lua_Asset> &)asset_ref;
	if (!refT.exists()) { CUSTOM_ASSERT(false, "asset doesn exist"); }

	//
	cstring path = asset_ref.get_path();
	Array<u8> file; read_file_safely(path, file);
	if (!file.count) { return; }

	Lua_Asset * asset = refT.get_fast();
	asset->update(file);

	// @Note: direct the asset to Lua
	if (CUSTOM_LOAD() != LUA_OK) {
		CUSTOM_ERROR("lua: '%s'", lua_tostring(L, -1));
		lua_pop(L, 1);
	}

	// @Todo: implement load/unload
	// @Note: memory might have been relocated
	if (!refT.exists()) { CUSTOM_ASSERT(false, "asset doesn exist"); }
	asset = refT.get_fast();
	asset->~Lua_Asset();
}

}}
