
static bool CheckLua(lua_State * L, s32 result)
{
	if (result == LUA_OK) { return true; }
	cstring errorMessage = lua_tostring(L, -1);
	CUSTOM_ERROR("Lua error: '%s'", errorMessage);
	return false;
}

static s32 NativeCustomAdd(lua_State * L)
{
	LUA_NUMBER a = lua_tonumber(L, -1);
	LUA_NUMBER b = lua_tonumber(L, -2);
	lua_pushnumber(L, a + b);
	return 1;
}

static void call_lua_from_c(lua_State * L) {
	lua_getglobal(L, "CustomAdd");
	if (lua_isfunction(L, -1))
	{
		lua_pushnumber(L, 3.5f);
		lua_pushnumber(L, 7.125f);
		if (CheckLua(L, lua_pcall(L, 2, 1, 0)))
		{
			CUSTOM_TRACE("[C] 'CustomAdd' result: '%f'", lua_tonumber(L, -1));
			lua_pop(L, 1);
		}
	}
	lua_pop(L, 1);
}

static void call_c_from_lua(lua_State * L) {
	lua_getglobal(L, "CallNativeCustomAdd");
	if (lua_isfunction(L, -1))
	{
		lua_pushnumber(L, 2.5f);
		lua_pushnumber(L, 10.125f);
		CheckLua(L, lua_pcall(L, 2, 1, 0));
	}
	lua_pop(L, 1);
}

static void call_c_from_c_function(lua_State * L) {
	lua_getglobal(L, "NativeCustomAdd");
	if (lua_isfunction(L, -1))
	{
		lua_pushnumber(L, 3.5f);
		lua_pushnumber(L, 7.125f);
		if (CheckLua(L, lua_pcall(L, 2, 1, 0)))
		{
			CUSTOM_TRACE("[C] 'NativeCustomAdd' result: '%f'", lua_tonumber(L, -1));
			lua_pop(L, 1);
		}
	}
	lua_pop(L, 1);
}

static void process_script(lua_State * L, cstring file_path) {
	if (!CheckLua(L, luaL_dofile(L, "assets/scripts/test.lua"))) { return; }
	call_lua_from_c(L);
	call_c_from_lua(L);
	call_c_from_c_function(L);
}

static void test_lua() {
	lua_State *L = luaL_newstate();

	// luaL_openlibs(L);
	luaL_requiref(L, LUA_GNAME, luaopen_base, 1); lua_pop(L, 1);
	luaL_requiref(L, LUA_STRLIBNAME, luaopen_string, 1); lua_pop(L, 1);

	lua_register(L, "NativeCustomAdd", NativeCustomAdd);
	process_script(L, "src/test.lua");

	lua_close(L);
}
