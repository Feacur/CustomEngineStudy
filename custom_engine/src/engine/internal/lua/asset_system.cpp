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

#define LUA_ASSERT_ASSET_TYPE(index) do {\
	LUA_ASSERT_TYPE(LUA_TUSERDATA, index);\
	if (lua_getmetatable(L, index)) {\
		bool is_an_asset = false;\
		for (u32 type = 0; !is_an_asset && type < custom::asset_names.get_count(); ++type) {\
			if (luaL_getmetatable(L, custom::asset_names.get_string(type)) != LUA_TTABLE) {\
				CUSTOM_LUA_ASSERT(false, "metatable '%s' doesn't exist", custom::asset_names.get_string(type));\
			}\
			is_an_asset = lua_rawequal(L, -1, -2);\
			lua_pop(L, 1);\
		}\
		lua_pop(L, 1);\
		CUSTOM_LUA_ASSERT(is_an_asset, "expected an asset at index %d", index);\
	}\
	else {\
		CUSTOM_LUA_ASSERT(false, "expected a userdata with a metatable at index %d", index);\
	}\
} while (0)\

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

	u32 type = (u32)lua_tointeger(L, 1);
	cstring id_string = lua_tostring(L, 2);
	u32 id = Asset::store_string(id_string, custom::empty_index);
	Asset asset = Asset::add(type, id);
	
	Asset * udata = (Asset *)lua_newuserdatauv(L, sizeof(Asset), 0);
	luaL_setmetatable(L, custom::asset_names.get_string(type));
	*udata = asset;

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
	Asset asset = Asset::get(type, id);

	bool has_asset = (*Asset::asset_containers[type])(asset);
	if (!has_asset) { lua_pushnil(L); return 1; }

	Asset * udata = (Asset *)lua_newuserdatauv(L, sizeof(Asset), 0);
	luaL_setmetatable(L, custom::asset_names.get_string(type));
	*udata = asset;

	return 1;
}

static int Asset_get_path(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 1, "expected 1 argument");
	LUA_ASSERT_ASSET_TYPE(1);

	Asset const * object = (Asset const *)lua_touserdata(L, 1);

	lua_pushstring(L, object->get_path());

	return 1;
}

static int Asset_exists(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 1, "expected 1 argument");
	LUA_ASSERT_ASSET_TYPE(1);

	Asset const * object = (Asset const *)lua_touserdata(L, 1);

	lua_pushboolean(L, object->exists());

	return 1;
}

static int Asset_destroy(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 1, "expected 1 argument");
	LUA_ASSERT_ASSET_TYPE(1);

	Asset * object = (Asset *)lua_touserdata(L, 1);
	object->destroy();

	return 0;
}

static luaL_Reg const Asset_meta[] = {
	{"__index", Asset_index},
	{"__newindex", Asset_newindex},
	// instance:###
	{"get_path", Asset_get_path},
	{"exists", Asset_exists},
	{"destroy", Asset_destroy},
	// Type.###
	{"add", Asset_add},
	{"rem", Asset_rem},
	{"has", Asset_has},
	{"get", Asset_get},
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
	for (u32 type = 0; type < custom::asset_names.get_count(); ++type) {
		lua_getglobal(L, custom::asset_names.get_string(type));
		lua_pushinteger(L, type);
		lua_setfield(L, -2, "type");
		lua_pop(L, 1);
	}
}

}}
