#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/api/lua.h"
#include "engine/impl/asset_system.h"

#include "../asset_system/asset_types.h"

#include <lua.hpp>

// @Todo: reuse userdata?

//
// Lua_Asset
//

static int Lua_Asset_index(lua_State * L) {
	typedef custom::RefT<Lua_Asset> Ref;

	LUA_INDEX_RAWGET_IMPL(Lua_Asset);

	Ref const * object = (Ref const *)lua_touserdata(L, 1);
	CUSTOM_LUA_ASSERT(object->exists(), "object doesn't exist");

	cstring id = lua_tostring(L, 2);

	// @Optimize?
	// if (strcmp(id, "") == 0) { return 0; }

	LUA_REPORT_INDEX();
	lua_pushnil(L); return 1;
}

static int Lua_Asset_newindex(lua_State * L) {
	typedef custom::RefT<Lua_Asset> Ref;

	Ref * object = (Ref *)lua_touserdata(L, 1);
	CUSTOM_LUA_ASSERT(object->exists(), "object doesn't exist");

	cstring id = lua_tostring(L, 2);

	// @Optimize?
	// if (strcmp(id, "") == 0) { return 0; }

	LUA_REPORT_INDEX();
	return 0;
}

static int Lua_Asset_eq(lua_State * L) {
	typedef custom::RefT<Lua_Asset> Ref;

	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_ASSERT_USERDATA("Lua_Asset", 2);

	Ref const * object1 = (Ref const *)lua_touserdata(L, 1);
	Ref const * object2 = (Ref const *)lua_touserdata(L, 2);

	lua_pushboolean(L, *object1 == *object2);

	return 1;
}

static luaL_Reg const Lua_Asset_meta[] = {
	{"__index", Lua_Asset_index},
	{"__newindex", Lua_Asset_newindex},
	{"__eq", Lua_Asset_eq},
	// instance:###
	// Type.###
	//
	{NULL, NULL},
};

//
//
//

namespace custom {
namespace lua {

void init_client_asset_types(lua_State * L) {
	#define ASSET_IMPL(T) LUA_META_IMPL(T)
	#include "../registry_impl/asset_types.h"
}

}}
