#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/core/collection_types.h"
#include "engine/debug/log.h"
#include "engine/api/platform/file.h"
#include "engine/api/client/asset_lookup.h"

#include <lua.hpp>

namespace custom {
namespace loader {

void script(lua_State * L, u32 asset_id) {
	// if (has_script(asset_id)) { return; }
	if (asset_id >= asset::script::count) { return; }
	cstring path = asset::script::paths[asset_id];

	Array<u8> file; file::read(path, file);
	if (file.count != file.capacity) { return; }
	file.push('\0');

	if (luaL_dostring(L, (cstring)file.data) != LUA_OK) {
		CUSTOM_ERROR("lua: '%s'", lua_tostring(L, -1));
		lua_pop(L, 1);
	}
}

}}
