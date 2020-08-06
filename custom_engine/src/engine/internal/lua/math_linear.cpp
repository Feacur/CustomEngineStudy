#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/api/lua.h"
#include "engine/impl/math_linear.h"

#include <lua.hpp>
// #include <lstate.h>

//
// vec2
//

static int vec2_tostring(lua_State * L) {
	CUSTOM_ASSERT(lua_gettop(L) == 1, "expected 1 argument");
	vec2 * udata = (vec2 *)luaL_checkudata(L, 1, "vec2");
	lua_pushfstring(L, "(%f, %f)", udata->x, udata->y);
	return 1;
}

static luaL_Reg const vec2_meta[] = {
	{"__index", NULL}, /* place holder */
	{"__tostring", vec2_tostring},
	{NULL, NULL},
};

static int vec2_magnitude_squared(lua_State * L) {
	CUSTOM_ASSERT(lua_gettop(L) == 1, "expected 1 argument");
	vec2 * udata = (vec2 *)luaL_checkudata(L, 1, "vec2");
	lua_pushnumber(L, magnitude_squared(*udata));
	return 1;
}

static luaL_Reg const vec2_methods[] = {
	{"magnitude_squared", vec2_magnitude_squared},
	{NULL, NULL},
};

static int vec2_dot(lua_State * L) {
	CUSTOM_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	vec2 * udata1 = (vec2 *)luaL_checkudata(L, 1, "vec2");
	vec2 * udata2 = (vec2 *)luaL_checkudata(L, 2, "vec2");
	lua_pushnumber(L, dot_product(*udata1, *udata2));
	return 1;
}

static luaL_Reg const vec2_lib[] = {
	{"dot", vec2_dot},
	{NULL, NULL},
};

//
// vec3
//

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
	vec3 * udata2 = (vec3 *)luaL_checkudata(L, 2, "vec3");
	lua_pushnumber(L, dot_product(*udata1, *udata2));
	return 1;
}

static luaL_Reg const vec3_lib[] = {
	{"dot", vec3_dot},
	{NULL, NULL},
};

//
// vec4
//

static int vec4_tostring(lua_State * L) {
	CUSTOM_ASSERT(lua_gettop(L) == 1, "expected 1 argument");
	vec4 * udata = (vec4 *)luaL_checkudata(L, 1, "vec4");
	lua_pushfstring(L, "(%f, %f, %f, %f)", udata->x, udata->y, udata->z, udata->w);
	return 1;
}

static luaL_Reg const vec4_meta[] = {
	{"__index", NULL}, /* place holder */
	{"__tostring", vec4_tostring},
	{NULL, NULL},
};

static int vec4_magnitude_squared(lua_State * L) {
	CUSTOM_ASSERT(lua_gettop(L) == 1, "expected 1 argument");
	vec4 * udata = (vec4 *)luaL_checkudata(L, 1, "vec4");
	lua_pushnumber(L, magnitude_squared(*udata));
	return 1;
}

static luaL_Reg const vec4_methods[] = {
	{"magnitude_squared", vec4_magnitude_squared},
	{NULL, NULL},
};

static int vec4_dot(lua_State * L) {
	CUSTOM_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	vec4 * udata1 = (vec4 *)luaL_checkudata(L, 1, "vec4");
	vec4 * udata2 = (vec4 *)luaL_checkudata(L, 2, "vec4");
	lua_pushnumber(L, dot_product(*udata1, *udata2));
	return 1;
}

static luaL_Reg const vec4_lib[] = {
	{"dot", vec4_dot},
	{NULL, NULL},
};

namespace custom {
namespace lua {

void init_math_linear(lua_State * L) {
	LUA_META_IMPL(vec2)
	LUA_META_IMPL(vec3)
	LUA_META_IMPL(vec4)
}

}}
