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

static int vec2_index(lua_State * L) {
	if (!lua_getmetatable(L, 1)) { lua_pushnil(L); return 1; }

	lua_pushvalue(L, 2);
	int type = lua_rawget(L, -2);
	lua_remove(L, -2);
	if (type != LUA_TNIL) { return 1; }

	vec2 * object = (vec2 *)lua_touserdata(L, 1);
	lua_pop(L, 1);

	cstring id = lua_tostring(L, 2);

	// @Optimize?
	if (strcmp(id, "x") == 0) { lua_pushnumber(L, object->x); return 1; }
	if (strcmp(id, "y") == 0) { lua_pushnumber(L, object->y); return 1; }

	lua_pushnil(L); return 1;
}

static int vec2_newindex(lua_State * L) {
	vec2 * object = (vec2 *)lua_touserdata(L, 1);
	cstring id = lua_tostring(L, 2);

	// @Optimize?
	if (strcmp(id, "x") == 0) { object->x = (r32)lua_tonumber(L, 3); return 0; }
	if (strcmp(id, "y") == 0) { object->y = (r32)lua_tonumber(L, 3); return 0; }

	return 0;
}

static int vec2_tostring(lua_State * L) {
	CUSTOM_ASSERT(lua_gettop(L) == 1, "expected 1 argument");
	vec2 * object = (vec2 *)luaL_checkudata(L, 1, "vec2");
	lua_pushfstring(L, "(%f, %f)", object->x, object->y);
	return 1;
}

static int vec2_magnitude_squared(lua_State * L) {
	CUSTOM_ASSERT(lua_gettop(L) == 1, "expected 1 argument");
	vec2 * object = (vec2 *)luaL_checkudata(L, 1, "vec2");
	lua_pushnumber(L, magnitude_squared(*object));
	return 1;
}

static int vec2_dot(lua_State * L) {
	CUSTOM_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	vec2 * object1 = (vec2 *)luaL_checkudata(L, 1, "vec2");
	vec2 * object2 = (vec2 *)luaL_checkudata(L, 2, "vec2");
	lua_pushnumber(L, dot_product(*object1, *object2));
	return 1;
}

static luaL_Reg const vec2_meta[] = {
	{"__index", vec2_index},
	{"__newindex", vec2_newindex},
	{"__tostring", vec2_tostring},
	// instance:###
	{"magnitude_squared", vec2_magnitude_squared},
	// Type.###
	{"dot", vec2_dot},
	{NULL, NULL},
};

//
// vec3
//

static int vec3_index(lua_State * L) {
	if (!lua_getmetatable(L, 1)) { lua_pushnil(L); return 1; }

	lua_pushvalue(L, 2);
	int type = lua_rawget(L, -2);
	lua_remove(L, -2);
	if (type != LUA_TNIL) { return 1; }

	vec3 * object = (vec3 *)lua_touserdata(L, 1);
	lua_pop(L, 1);

	cstring id = lua_tostring(L, 2);

	// @Optimize?
	if (strcmp(id, "x") == 0) { lua_pushnumber(L, object->x); return 1; }
	if (strcmp(id, "y") == 0) { lua_pushnumber(L, object->y); return 1; }
	if (strcmp(id, "z") == 0) { lua_pushnumber(L, object->z); return 1; }

	lua_pushnil(L); return 1;
}

static int vec3_newindex(lua_State * L) {
	vec3 * object = (vec3 *)lua_touserdata(L, 1);
	cstring id = lua_tostring(L, 2);

	// @Optimize?
	if (strcmp(id, "x") == 0) { object->x = (r32)lua_tonumber(L, 3); return 0; }
	if (strcmp(id, "y") == 0) { object->y = (r32)lua_tonumber(L, 3); return 0; }
	if (strcmp(id, "z") == 0) { object->z = (r32)lua_tonumber(L, 3); return 0; }

	return 0;
}

static int vec3_tostring(lua_State * L) {
	CUSTOM_ASSERT(lua_gettop(L) == 1, "expected 1 argument");
	vec3 * object = (vec3 *)luaL_checkudata(L, 1, "vec3");
	lua_pushfstring(L, "(%f, %f, %f)", object->x, object->y, object->z);
	return 1;
}

static int vec3_magnitude_squared(lua_State * L) {
	CUSTOM_ASSERT(lua_gettop(L) == 1, "expected 1 argument");
	vec3 * object = (vec3 *)luaL_checkudata(L, 1, "vec3");
	lua_pushnumber(L, magnitude_squared(*object));
	return 1;
}

static int vec3_dot(lua_State * L) {
	CUSTOM_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	vec3 * object1 = (vec3 *)luaL_checkudata(L, 1, "vec3");
	vec3 * object2 = (vec3 *)luaL_checkudata(L, 2, "vec3");
	lua_pushnumber(L, dot_product(*object1, *object2));
	return 1;
}

static luaL_Reg const vec3_meta[] = {
	{"__index", vec3_index},
	{"__newindex", vec3_newindex},
	{"__tostring", vec3_tostring},
	// instance:###
	{"magnitude_squared", vec3_magnitude_squared},
	// Type.###
	{"dot", vec3_dot},
	{NULL, NULL},
};

//
// vec4
//

static int vec4_index(lua_State * L) {
	if (!lua_getmetatable(L, 1)) { lua_pushnil(L); return 1; }

	lua_pushvalue(L, 2);
	int type = lua_rawget(L, -2);
	lua_remove(L, -2);
	if (type != LUA_TNIL) { return 1; }

	vec4 * object = (vec4 *)lua_touserdata(L, 1);
	lua_pop(L, 1);

	cstring id = lua_tostring(L, 2);

	// @Optimize?
	if (strcmp(id, "x") == 0) { lua_pushnumber(L, object->x); return 1; }
	if (strcmp(id, "y") == 0) { lua_pushnumber(L, object->y); return 1; }
	if (strcmp(id, "z") == 0) { lua_pushnumber(L, object->z); return 1; }
	if (strcmp(id, "w") == 0) { lua_pushnumber(L, object->w); return 1; }

	lua_pushnil(L); return 1;
}

static int vec4_newindex(lua_State * L) {
	vec4 * object = (vec4 *)lua_touserdata(L, 1);
	cstring id = lua_tostring(L, 2);

	// @Optimize?
	if (strcmp(id, "x") == 0) { object->x = (r32)lua_tonumber(L, 3); return 0; }
	if (strcmp(id, "y") == 0) { object->y = (r32)lua_tonumber(L, 3); return 0; }
	if (strcmp(id, "z") == 0) { object->z = (r32)lua_tonumber(L, 3); return 0; }
	if (strcmp(id, "w") == 0) { object->w = (r32)lua_tonumber(L, 3); return 0; }

	return 0;
}

static int vec4_tostring(lua_State * L) {
	CUSTOM_ASSERT(lua_gettop(L) == 1, "expected 1 argument");
	vec4 * object = (vec4 *)luaL_checkudata(L, 1, "vec4");
	lua_pushfstring(L, "(%f, %f, %f, %f)", object->x, object->y, object->z, object->w);
	return 1;
}

static int vec4_magnitude_squared(lua_State * L) {
	CUSTOM_ASSERT(lua_gettop(L) == 1, "expected 1 argument");
	vec4 * object = (vec4 *)luaL_checkudata(L, 1, "vec4");
	lua_pushnumber(L, magnitude_squared(*object));
	return 1;
}

static int vec4_dot(lua_State * L) {
	CUSTOM_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	vec4 * object1 = (vec4 *)luaL_checkudata(L, 1, "vec4");
	vec4 * object2 = (vec4 *)luaL_checkudata(L, 2, "vec4");
	lua_pushnumber(L, dot_product(*object1, *object2));
	return 1;
}

static luaL_Reg const vec4_meta[] = {
	{"__index", vec4_index},
	{"__newindex", vec4_newindex},
	{"__tostring", vec4_tostring},
	// instance:###
	{"magnitude_squared", vec4_magnitude_squared},
	// Type.###
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
