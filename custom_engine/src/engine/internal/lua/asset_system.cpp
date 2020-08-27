#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/api/lua.h"
#include "engine/api/internal/asset_system.h"
#include "engine/api/internal/types_names_lookup.h"

#include <lua.hpp>

// @Todo: reuse userdata?

typedef custom::Asset Asset;
typedef custom::Ref Ref;

static int Asset_index(lua_State * L) {
	LUA_INDEX_RAWGET_IMPL(Asset);

	Asset const * object = (Asset const *)lua_touserdata(L, 1);

	cstring id = lua_tostring(L, 2);

	LUA_REPORT_INDEX();
	lua_pushnil(L); return 1;
}

static int Asset_newindex(lua_State * L) {
	Asset * object = (Asset *)lua_touserdata(L, 1);

	cstring id = lua_tostring(L, 2);

	// @Optimize?
	// if (strcmp(id, "") == 0) { return 0; }

	LUA_REPORT_INDEX();
	return 0;
}

static int Asset_add(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_ASSERT_TYPE(LUA_TNUMBER, 1);
	LUA_ASSERT_TYPE(LUA_TSTRING, 2);

	// @Todo: protect strings so that they wouldn't be garbage-collected
	//        - either by storing them at the engine side
	//        - or by marking them as such at the Lua side
	u32 type = (u32)lua_tointeger(L, 1);
	cstring id_string = lua_tostring(L, 2);
	u32 id = Asset::store_string(id_string, custom::empty_index);
	Asset asset_ref = Asset::add(type, id);
	
	Asset * udata = (Asset *)lua_newuserdatauv(L, sizeof(Asset), 0);
	luaL_setmetatable(L, custom::asset_names[type]);
	*udata = asset_ref;

	return 1;
}

static int Asset_rem(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_ASSERT_TYPE(LUA_TNUMBER, 1);
	LUA_ASSERT_TYPE(LUA_TSTRING, 2);

	u32 type = (u32)lua_tointeger(L, 1);
	cstring id_string = lua_tostring(L, 2);
	u32 id = Asset::store_string(id_string, custom::empty_index);
	Asset::rem(type, id);

	return 0;
}

static int Asset_has(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_ASSERT_TYPE(LUA_TNUMBER, 1);
	LUA_ASSERT_TYPE(LUA_TSTRING, 2);

	u32 type = (u32)lua_tointeger(L, 1);
	cstring id_string = lua_tostring(L, 2);
	u32 id = Asset::store_string(id_string, custom::empty_index);
	lua_pushboolean(L, Asset::has(type, id));

	return 1;
}

static int Asset_get(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_ASSERT_TYPE(LUA_TNUMBER, 1);
	LUA_ASSERT_TYPE(LUA_TSTRING, 2);

	u32 type = (u32)lua_tointeger(L, 1);
	cstring id_string = lua_tostring(L, 2);
	u32 id = Asset::store_string(id_string, custom::empty_index);
	Asset asset_ref = Asset::get(type, id);

	bool has_asset = (*Asset::asset_containers[type])(asset_ref);
	if (!has_asset) { lua_pushnil(L); return 1; }

	Asset * udata = (Asset *)lua_newuserdatauv(L, sizeof(Asset), 0);
	luaL_setmetatable(L, custom::asset_names[type]);
	*udata = asset_ref;

	return 1;
}

static int Asset_get_path(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_ASSERT_TYPE(LUA_TNUMBER, 1);

	u32 type = (u32)lua_tointeger(L, 1);
	LUA_ASSERT_USERDATA(custom::asset_names[type], 2);

	Asset const * ref = (Asset const *)lua_touserdata(L, 2);
	cstring path = Asset::get_path(type, *ref);

	if (!path) { lua_pushnil(L); return 1; }

	lua_pushstring(L, path);

	return 1;
}

static luaL_Reg const Asset_meta[] = {
	{"__index", Asset_index},
	{"__newindex", Asset_newindex},
	// instance:###
	// Type.###
	{"add", Asset_add},
	{"rem", Asset_rem},
	{"has", Asset_has},
	{"get", Asset_get},
	{"get_path", Asset_get_path},
	//
	{NULL, NULL},
};

//
//
//

namespace custom {
namespace lua {

// @Change: let client call `init` itself?
//          although why potentially discarding engine-side types?
void init_asset_types(lua_State * L);
extern void init_client_asset_types(lua_State * L);

void init_asset_system(lua_State * L) {
	LUA_META_IMPL(Asset)
	custom::lua::init_asset_types(L);
	custom::lua::init_client_asset_types(L);
	for (u32 i = 0; i < custom::asset_names.count; ++i) {
		lua_getglobal(L, custom::asset_names[i]);
		lua_pushinteger(L, i);
		lua_setfield(L, -2, "type");
		lua_pop(L, 1);
	}
}

}}
