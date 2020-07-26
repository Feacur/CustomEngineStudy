#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/core/collection_types.h"
#include "engine/debug/log.h"
#include "engine/api/platform/file.h"
#include "engine/api/client/asset_lookup.h"

#include <lua.hpp>

namespace custom {
namespace loader {

// @Note: alternative to `luaL_dostring(L, (cstring)file.data)`
#define CUSTOM_LOAD()\
	luaL_loadbufferx(L, (cstring)file.data, file.count, path, NULL)\
	|| lua_pcall(L, 0, LUA_MULTRET, 0)\

void script(lua_State * L, u32 asset_id) {
	// if (has_script(asset_id)) { return; }
	if (asset_id >= asset::script::count) { return; }
	cstring path = asset::script::paths[asset_id];

	Array<u8> file; file::read(path, file);
	if (file.count != file.capacity) { return; }

	if (CUSTOM_LOAD() != LUA_OK) {
		CUSTOM_ERROR("lua: '%s'", lua_tostring(L, -1));
		lua_pop(L, 1);
	}
}
#undef CUSTOM_LOAD

}}
