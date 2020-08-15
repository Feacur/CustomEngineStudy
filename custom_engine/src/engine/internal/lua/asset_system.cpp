#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/api/lua.h"
#include "engine/api/internal/asset_system.h"
#include "engine/api/client/asset_types_lookup.h"

#include <lua.hpp>
// #include <lstate.h>

typedef custom::Assets Assets;
typedef custom::Ref Ref;

static int Assets_index(lua_State * L) {
	LUA_INDEX_RAWGET_IMPL(Assets);

	Assets const * object = (Assets const *)lua_touserdata(L, 1);

	cstring id = lua_tostring(L, 2);

	LUA_REPORT_INDEX();
	lua_pushnil(L); return 1;
}

static int Assets_newindex(lua_State * L) {
	Assets * object = (Assets *)lua_touserdata(L, 1);

	cstring id = lua_tostring(L, 2);

	// @Optimize?
	// if (strcmp(id, "") == 0) { return 0; }

	LUA_REPORT_INDEX();
	return 0;
}

static int Assets_add(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_ASSERT_TYPE(LUA_TNUMBER, 1);
	LUA_ASSERT_TYPE(LUA_TSTRING, 2);

	u32 type = (u32)lua_tointeger(L, 1);
	cstring id = lua_tostring(L, 2);
	Ref const & component_ref = Assets::add(type, id);
	
	Ref * udata = (Ref *)lua_newuserdatauv(L, sizeof(Ref), 0);
	luaL_setmetatable(L, custom::asset::names[type]);
	*udata = component_ref;

	return 1;
}

static int Assets_rem(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_ASSERT_TYPE(LUA_TNUMBER, 1);
	LUA_ASSERT_TYPE(LUA_TSTRING, 2);

	u32 type = (u32)lua_tointeger(L, 1);
	cstring id = lua_tostring(L, 2);
	Assets::rem(type, id);

	return 0;
}

static int Assets_has(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_ASSERT_TYPE(LUA_TNUMBER, 1);
	LUA_ASSERT_TYPE(LUA_TSTRING, 2);

	u32 type = (u32)lua_tointeger(L, 1);
	cstring id = lua_tostring(L, 2);
	lua_pushboolean(L, Assets::has(type, id));

	return 1;
}

static int Assets_get(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_ASSERT_TYPE(LUA_TNUMBER, 1);
	LUA_ASSERT_TYPE(LUA_TSTRING, 2);

	u32 type = (u32)lua_tointeger(L, 1);
	cstring id = lua_tostring(L, 2);
	Ref component_ref = Assets::get(type, id);

	bool has_asset = (*Assets::asset_containers[type])(component_ref);
	if (!has_asset) { lua_pushnil(L); return 1; }

	Ref * udata = (Ref *)lua_newuserdatauv(L, sizeof(Ref), 0);
	luaL_setmetatable(L, custom::asset::names[type]);
	*udata = component_ref;

	return 1;
}

static int Assets_get_path(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_ASSERT_TYPE(LUA_TNUMBER, 1);

	u32 type = (u32)lua_tointeger(L, 1);
	LUA_ASSERT_USERDATA(custom::asset::names[type], 2);

	Ref const * ref = (Ref const *)lua_touserdata(L, 2);
	cstring path = Assets::get_path(type, *ref);

	if (!path) { lua_pushnil(L); return 1; }

	lua_pushstring(L, path);

	return 1;
}

static luaL_Reg const Assets_meta[] = {
	{"__index", Assets_index},
	{"__newindex", Assets_newindex},
	// instance:###
	// Type.###
	{"add", Assets_add},
	{"rem", Assets_rem},
	{"has", Assets_has},
	{"get", Assets_get},
	{"get_path", Assets_get_path},
	//
	{NULL, NULL},
};

namespace custom {
namespace lua {

void init_assets(lua_State * L);
void init_asset_system(lua_State * L) {
	LUA_META_IMPL(Assets)
	custom::lua::init_assets(L);
	for (u32 i = 0; i < custom::asset::count; ++i) {
		lua_getglobal(L, custom::asset::names[i]);
		lua_pushinteger(L, i);
		lua_setfield(L, -2, "type");
		lua_pop(L, 1);
	}
}

}}
