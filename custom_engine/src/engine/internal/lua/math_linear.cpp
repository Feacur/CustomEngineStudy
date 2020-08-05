#include "custom_pch.h"
#include "engine/impl/math_linear.h"

#include <lua.hpp>

static int vec3_tostring(lua_State * L) {
	vec3 * udata = (vec3 *)luaL_checkudata(L, 1, "vec3");
	lua_pushfstring(L, "(%f, %f, %f)", udata->x, udata->y, udata->z);
	return 1;
}

static luaL_Reg const vec3_mt[] = {
	{"__index", NULL}, /* place holder */
	{"__tostring", vec3_tostring},
	{NULL, NULL},
};

static luaL_Reg const math_lib[] = {
	{NULL, NULL},
};

int luaopen_custom_math(lua_State * L) {
	luaL_newlib(L, math_lib);

	if (luaL_newmetatable(L, "vec3")) {
		luaL_setfuncs(L, vec3_mt, 0);
		lua_setfield(L, -1, "__index");
	}
	else { lua_pop(L, 1); }

	return 0;
}
