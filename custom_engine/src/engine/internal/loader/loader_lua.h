#include <lua.hpp>

namespace custom {
namespace loader {

// @Note: alternative to `luaL_dostring(L, (cstring)file.data)`
#define CUSTOM_LOAD() (\
	luaL_loadbufferx(L, (cstring)file.data, file.count, path, NULL)\
	|| lua_pcall(L, 0, LUA_MULTRET, 0)\
)\

void script(lua_State * L, RefT<Lua_Asset> const & ref) {
	if (!ref.exists()) { CUSTOM_ASSERT(false, "asset doesn't exist"); return; }

	cstring path = Asset::get_path(ref);
	if (!file::exists(path)) { CUSTOM_ASSERT(false, "file doesn't exist"); return; }

	Array<u8> file; file::read(path, file);
	if (file.count != file.capacity) { return; }

	Lua_Asset const * asset = ref.get_fast();

	if (CUSTOM_LOAD() != LUA_OK) {
		CUSTOM_ERROR("lua: '%s'", lua_tostring(L, -1));
		lua_pop(L, 1);
	}
}
#undef CUSTOM_LOAD

}}
