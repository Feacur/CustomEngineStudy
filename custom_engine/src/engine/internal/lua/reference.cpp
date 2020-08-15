#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/api/lua.h"
#include "engine/api/internal/reference.h"

#include <lua.hpp>
// #include <lstate.h>

typedef custom::Ref Ref;

static int Ref_index(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_INDEX_RAWGET_IMPL(Ref);

	LUA_DECLARE_USERDATA_CONST_FAST(Ref, object, 1);
	cstring id = lua_tostring(L, 2);

	// @Optimize?
	if (strcmp(id, "id") == 0) { lua_pushinteger(L, object->id); return 1; }
	if (strcmp(id, "gen") == 0) { lua_pushinteger(L, object->gen); return 1; }

	LUA_REPORT_INDEX();
	lua_pushnil(L); return 1;
}

static int Ref_newindex(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 3, "expected 3 arguments");
	LUA_DECLARE_USERDATA_FAST(Ref, object, 1);
	cstring id = lua_tostring(L, 2);

	// @Optimize?
	LUA_ASSERT_TYPE(LUA_TNUMBER, 3);
	if (strcmp(id, "id") == 0) { object->id = (u32)lua_tointeger(L, 3); return 0; }
	if (strcmp(id, "gen") == 0) { object->gen = (u32)lua_tointeger(L, 3); return 0; }

	LUA_REPORT_INDEX();
	return 0;
}

static int Ref_eq(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_DECLARE_USERDATA_CONST_FAST(Ref, object1, 1);
	LUA_DECLARE_USERDATA_CONST_SAFE(Ref, object2, 2);

	lua_pushboolean(L, *object1 == *object2);

	return 1;
}

static int Ref_new(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_ASSERT_TYPE(LUA_TNUMBER, 1);
	LUA_ASSERT_TYPE(LUA_TNUMBER, 2);

	Ref * udata = (Ref *)lua_newuserdatauv(L, sizeof(Ref), 0);
	luaL_setmetatable(L, "Ref");
	*udata = {
		(u32)lua_tointeger(L, 1),
		(u32)lua_tointeger(L, 2),
	};

	return 1;
}

static luaL_Reg const Ref_meta[] = {
	{"__index", Ref_index},
	{"__newindex", Ref_newindex},
	{"__eq",  Ref_eq},
	// instance:###
	// Type.###
	{"new", Ref_new},
	{NULL, NULL},
};

namespace custom {
namespace lua {

void init_reference(lua_State * L) {
	LUA_META_IMPL(Ref)
}

}}
