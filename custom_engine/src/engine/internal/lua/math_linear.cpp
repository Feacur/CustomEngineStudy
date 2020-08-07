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
	LUA_INDEX_RAWGET_IMPL(vec2)

	vec2 * object = (vec2 *)lua_touserdata(L, 1);

	cstring id = lua_tostring(L, 2);

	// @Optimize?
	if (strcmp(id, "x") == 0) { lua_pushnumber(L, object->x); return 1; }
	if (strcmp(id, "y") == 0) { lua_pushnumber(L, object->y); return 1; }

	LUA_REPORT_INDEX();
	lua_pushnil(L); return 1;
}

static int vec2_newindex(lua_State * L) {
	LUA_ASSERT_USERDATA(1, "vec2");
	vec2 * object = (vec2 *)lua_touserdata(L, 1);
	cstring id = lua_tostring(L, 2);

	// @Optimize?
	CUSTOM_LUA_ASSERT(lua_type(L, 3) == LUA_TNUMBER, "expected a number");
	if (strcmp(id, "x") == 0) { object->x = (r32)lua_tonumber(L, 3); return 0; }
	if (strcmp(id, "y") == 0) { object->y = (r32)lua_tonumber(L, 3); return 0; }

	LUA_REPORT_INDEX();
	return 0;
}

static int vec2_magnitude_squared(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 1, "expected 1 argument");

	LUA_ASSERT_USERDATA(1, "vec2");
	vec2 * object = (vec2 *)lua_touserdata(L, 1);
	lua_pushnumber(L, magnitude_squared(*object));

	return 1;
}

static int vec2_new(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	CUSTOM_LUA_ASSERT(lua_type(L, 1) == LUA_TNUMBER, "expected a number");
	CUSTOM_LUA_ASSERT(lua_type(L, 2) == LUA_TNUMBER, "expected a number");

	vec2 * udata = (vec2 *)lua_newuserdatauv(L, sizeof(vec2), 0);
	luaL_setmetatable(L, "vec2");
	*udata = {
		(r32)lua_tonumber(L, 1),
		(r32)lua_tonumber(L, 2),
	};

	return 1;
}

static int vec2_dot(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_ASSERT_USERDATA(1, "vec2");
	LUA_ASSERT_USERDATA(2, "vec2");

	vec2 * object1 = (vec2 *)lua_touserdata(L, 1);
	vec2 * object2 = (vec2 *)lua_touserdata(L, 2);
	lua_pushnumber(L, dot_product(*object1, *object2));

	return 1;
}

static luaL_Reg const vec2_meta[] = {
	{"__index", vec2_index},
	{"__newindex", vec2_newindex},
	// instance:###
	{"magnitude_squared", vec2_magnitude_squared},
	// Type.###
	{"new", vec2_new},
	{"dot", vec2_dot},
	{NULL, NULL},
};

//
// vec3
//

static int vec3_index(lua_State * L) {
	LUA_INDEX_RAWGET_IMPL(vec3)

	vec3 * object = (vec3 *)lua_touserdata(L, 1);

	cstring id = lua_tostring(L, 2);

	// @Optimize?
	if (strcmp(id, "x") == 0) { lua_pushnumber(L, object->x); return 1; }
	if (strcmp(id, "y") == 0) { lua_pushnumber(L, object->y); return 1; }
	if (strcmp(id, "z") == 0) { lua_pushnumber(L, object->z); return 1; }

	LUA_REPORT_INDEX();
	lua_pushnil(L); return 1;
}

static int vec3_newindex(lua_State * L) {
	LUA_ASSERT_USERDATA(1, "vec3");
	vec3 * object = (vec3 *)lua_touserdata(L, 1);
	cstring id = lua_tostring(L, 2);

	// @Optimize?
	CUSTOM_LUA_ASSERT(lua_type(L, 3) == LUA_TNUMBER, "expected a number");
	if (strcmp(id, "x") == 0) { object->x = (r32)lua_tonumber(L, 3); return 0; }
	if (strcmp(id, "y") == 0) { object->y = (r32)lua_tonumber(L, 3); return 0; }
	if (strcmp(id, "z") == 0) { object->z = (r32)lua_tonumber(L, 3); return 0; }

	LUA_REPORT_INDEX();
	return 0;
}

static int vec3_magnitude_squared(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 1, "expected 1 argument");

	LUA_ASSERT_USERDATA(1, "vec3");
	vec3 * object = (vec3 *)lua_touserdata(L, 1);
	lua_pushnumber(L, magnitude_squared(*object));

	return 1;
}

static int vec3_new(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 3, "expected 3 arguments");
	CUSTOM_LUA_ASSERT(lua_type(L, 1) == LUA_TNUMBER, "expected a number");
	CUSTOM_LUA_ASSERT(lua_type(L, 2) == LUA_TNUMBER, "expected a number");
	CUSTOM_LUA_ASSERT(lua_type(L, 3) == LUA_TNUMBER, "expected a number");

	vec3 * udata = (vec3 *)lua_newuserdatauv(L, sizeof(vec3), 0);
	luaL_setmetatable(L, "vec3");
	*udata = {
		(r32)lua_tonumber(L, 1),
		(r32)lua_tonumber(L, 2),
		(r32)lua_tonumber(L, 3),
	};

	return 1;
}

static int vec3_dot(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_ASSERT_USERDATA(1, "vec3");
	LUA_ASSERT_USERDATA(2, "vec3");

	vec3 * object1 = (vec3 *)lua_touserdata(L, 1);
	vec3 * object2 = (vec3 *)lua_touserdata(L, 2);
	lua_pushnumber(L, dot_product(*object1, *object2));

	return 1;
}

static luaL_Reg const vec3_meta[] = {
	{"__index", vec3_index},
	{"__newindex", vec3_newindex},
	// instance:###
	{"magnitude_squared", vec3_magnitude_squared},
	// Type.###
	{"new", vec3_new},
	{"dot", vec3_dot},
	{NULL, NULL},
};

//
// vec4
//

static int vec4_index(lua_State * L) {
	LUA_INDEX_RAWGET_IMPL(vec4)

	vec4 * object = (vec4 *)lua_touserdata(L, 1);

	cstring id = lua_tostring(L, 2);

	// @Optimize?
	if (strcmp(id, "x") == 0) { lua_pushnumber(L, object->x); return 1; }
	if (strcmp(id, "y") == 0) { lua_pushnumber(L, object->y); return 1; }
	if (strcmp(id, "z") == 0) { lua_pushnumber(L, object->z); return 1; }
	if (strcmp(id, "w") == 0) { lua_pushnumber(L, object->w); return 1; }

	LUA_REPORT_INDEX();
	lua_pushnil(L); return 1;
}

static int vec4_newindex(lua_State * L) {
	LUA_ASSERT_USERDATA(1, "vec4");
	vec4 * object = (vec4 *)lua_touserdata(L, 1);
	cstring id = lua_tostring(L, 2);

	// @Optimize?
	CUSTOM_LUA_ASSERT(lua_type(L, 3) == LUA_TNUMBER, "expected a number");
	if (strcmp(id, "x") == 0) { object->x = (r32)lua_tonumber(L, 3); return 0; }
	if (strcmp(id, "y") == 0) { object->y = (r32)lua_tonumber(L, 3); return 0; }
	if (strcmp(id, "z") == 0) { object->z = (r32)lua_tonumber(L, 3); return 0; }
	if (strcmp(id, "w") == 0) { object->w = (r32)lua_tonumber(L, 3); return 0; }

	LUA_REPORT_INDEX();
	return 0;
}

static int vec4_magnitude_squared(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 1, "expected 1 argument");

	LUA_ASSERT_USERDATA(1, "vec4");
	vec4 * object = (vec4 *)lua_touserdata(L, 1);
	lua_pushnumber(L, magnitude_squared(*object));

	return 1;
}

static int vec4_new(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 4, "expected 4 arguments");
	CUSTOM_LUA_ASSERT(lua_type(L, 1) == LUA_TNUMBER, "expected a number");
	CUSTOM_LUA_ASSERT(lua_type(L, 2) == LUA_TNUMBER, "expected a number");
	CUSTOM_LUA_ASSERT(lua_type(L, 3) == LUA_TNUMBER, "expected a number");
	CUSTOM_LUA_ASSERT(lua_type(L, 4) == LUA_TNUMBER, "expected a number");

	vec4 * udata = (vec4 *)lua_newuserdatauv(L, sizeof(vec4), 0);
	luaL_setmetatable(L, "vec4");
	*udata = {
		(r32)lua_tonumber(L, 1),
		(r32)lua_tonumber(L, 2),
		(r32)lua_tonumber(L, 3),
		(r32)lua_tonumber(L, 4),
	};

	return 1;
}

static int vec4_dot(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_ASSERT_USERDATA(1, "vec4");
	LUA_ASSERT_USERDATA(2, "vec4");

	vec4 * object1 = (vec4 *)lua_touserdata(L, 1);
	vec4 * object2 = (vec4 *)lua_touserdata(L, 2);
	lua_pushnumber(L, dot_product(*object1, *object2));

	return 1;
}

static luaL_Reg const vec4_meta[] = {
	{"__index", vec4_index},
	{"__newindex", vec4_newindex},
	// instance:###
	{"magnitude_squared", vec4_magnitude_squared},
	// Type.###
	{"new", vec4_new},
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
