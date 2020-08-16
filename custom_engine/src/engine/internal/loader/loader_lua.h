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
