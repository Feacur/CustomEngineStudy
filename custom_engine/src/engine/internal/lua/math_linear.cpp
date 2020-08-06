#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/impl/math_linear.h"

#include <lua.hpp>

static int vec3_tostring(lua_State * L) {
	CUSTOM_ASSERT(lua_gettop(L) == 1, "expected 1 argument");
	vec3 * udata = (vec3 *)luaL_checkudata(L, 1, "vec3");
	lua_pushfstring(L, "(%f, %f, %f)", udata->x, udata->y, udata->z);
	return 1;
}

static luaL_Reg const vec3_meta[] = {
	{"__index", NULL}, /* place holder */
	{"__tostring", vec3_tostring},
	{NULL, NULL},
};

static int vec3_magnitude_squared(lua_State * L) {
	CUSTOM_ASSERT(lua_gettop(L) == 1, "expected 1 argument");
	vec3 * udata = (vec3 *)luaL_checkudata(L, 1, "vec3");
	lua_pushnumber(L, magnitude_squared(*udata));
	return 1;
}

static luaL_Reg const vec3_methods[] = {
	{"magnitude_squared", vec3_magnitude_squared},
	{NULL, NULL},
};

static int vec3_dot(lua_State * L) {
	CUSTOM_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	vec3 * udata1 = (vec3 *)luaL_checkudata(L, 1, "vec3");
	vec3 * udata2 = (vec3 *)luaL_checkudata(L, 1, "vec3");
	lua_pushnumber(L, dot_product(*udata1, *udata2));
	return 1;
}

static luaL_Reg const vec3_lib[] = {
	{"dot", vec3_dot},
	{NULL, NULL},
};

namespace custom {
namespace lua {

void init_math_linear(lua_State * L) {
	if (luaL_newmetatable(L, "vec3")) {
		luaL_setfuncs(L, vec3_meta, 0);

		lua_pushvalue(L, -1);
		luaL_newlib(L, vec3_methods);
		lua_setfield(L, -1, "__index");

		luaL_setfuncs(L, vec3_lib, 0);
		lua_setglobal(L, "vec3");
	}
	else { lua_pop(L, 1); }
}

}}
