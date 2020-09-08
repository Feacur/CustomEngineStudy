#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/core/types.h"
#include "engine/debug/log.h"
#include "engine/api/lua.h"

#include <lua.hpp>

static int Debug_custom_assert(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_ASSERT_TYPE(LUA_TBOOLEAN, 1);
	LUA_ASSERT_TYPE(LUA_TSTRING,  2);

	CUSTOM_ASSERT(lua_toboolean(L, 1), "%s", lua_tostring(L, 2));

	return 0;
}

static luaL_Reg const Debug_aux[] = {
	// Lib.###
	{"custom_assert", Debug_custom_assert},
	{NULL, NULL},
};

//
//
//

namespace custom {
namespace lua {

void init_debug(lua_State * L) {
	LUA_AUX_IMPL(Debug)

	// @Todo: figure out interactive debugging
	// luaL_requiref(L, LUA_DBLIBNAME, luaopen_debug, 1); lua_pop(L, 1);
}

}}