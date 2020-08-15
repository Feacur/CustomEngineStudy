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
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_INDEX_RAWGET_IMPL(vec2);

	vec2 const * object = (vec2 const *)lua_touserdata(L, 1);
	cstring id = lua_tostring(L, 2);

	// @Optimize?
	if (strcmp(id, "x") == 0) { lua_pushnumber(L, object->x); return 1; }
	if (strcmp(id, "y") == 0) { lua_pushnumber(L, object->y); return 1; }

	LUA_REPORT_INDEX();
	lua_pushnil(L); return 1;
}

static int vec2_newindex(lua_State * L) {
	vec2 * object = (vec2 *)lua_touserdata(L, 1);
	cstring id = lua_tostring(L, 2);

	// @Optimize?
	LUA_ASSERT_TYPE(LUA_TNUMBER, 3);
	if (strcmp(id, "x") == 0) { object->x = (r32)lua_tonumber(L, 3); return 0; }
	if (strcmp(id, "y") == 0) { object->y = (r32)lua_tonumber(L, 3); return 0; }

	LUA_REPORT_INDEX();
	return 0;
}

static int vec2_add(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_ASSERT_USERDATA("vec2", 2);

	vec2 const * object1 = (vec2 const *)lua_touserdata(L, 1);
	vec2 const * object2 = (vec2 const *)lua_touserdata(L, 2);

	vec2 * udata = (vec2 *)lua_newuserdatauv(L, sizeof(vec2), 0);
	luaL_setmetatable(L, "vec2");
	*udata = *object1 + *object2;

	return 1;
}

static int vec2_sub(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_ASSERT_USERDATA("vec2", 2);

	vec2 const * object1 = (vec2 const *)lua_touserdata(L, 1);
	vec2 const * object2 = (vec2 const *)lua_touserdata(L, 2);

	vec2 * udata = (vec2 *)lua_newuserdatauv(L, sizeof(vec2), 0);
	luaL_setmetatable(L, "vec2");
	*udata = *object1 - *object2;

	return 1;
}

static int vec2_mul(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");

	vec2 const * object1 = (vec2 const *)lua_touserdata(L, 1);

	vec2 * udata = (vec2 *)lua_newuserdatauv(L, sizeof(vec2), 0);
	luaL_setmetatable(L, "vec2");

	if (lua_type(L, 2) == LUA_TNUMBER) {
		*udata = *object1 * (r32)lua_tonumber(L, 2);
	}
	else {
		LUA_ASSERT_USERDATA("vec2", 2);
		vec2 const * object2 = (vec2 const *)lua_touserdata(L, 2);
		*udata = *object1 * *object2;
	}

	return 1;
}

static int vec2_div(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	vec2 const * object1 = (vec2 const *)lua_touserdata(L, 1);

	vec2 * udata = (vec2 *)lua_newuserdatauv(L, sizeof(vec2), 0);
	luaL_setmetatable(L, "vec2");

	if (lua_type(L, 2) == LUA_TNUMBER) {
		*udata = *object1 / (r32)lua_tonumber(L, 2);
	}
	else {
		LUA_ASSERT_USERDATA("vec2", 2);
		vec2 const * object2 = (vec2 const *)lua_touserdata(L, 2);
		*udata = *object1 / *object2;
	}

	return 1;
}

static int vec2_unm(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 1 argument");
	vec2 const * object = (vec2 const *)lua_touserdata(L, 1);

	vec2 * udata = (vec2 *)lua_newuserdatauv(L, sizeof(vec2), 0);
	luaL_setmetatable(L, "vec2");
	*udata = -*object;

	return 1;
}

static int vec2_eq(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_ASSERT_USERDATA("vec2", 2);

	vec2 const * object1 = (vec2 const *)lua_touserdata(L, 1);
	vec2 const * object2 = (vec2 const *)lua_touserdata(L, 2);
	lua_pushboolean(L, *object1 == *object2);

	return 1;
}

static int vec2_new(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_ASSERT_TYPE(LUA_TNUMBER, 1);
	LUA_ASSERT_TYPE(LUA_TNUMBER, 2);

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
	LUA_ASSERT_USERDATA("vec2", 1);
	LUA_ASSERT_USERDATA("vec2", 2);

	vec2 const * object1 = (vec2 const *)lua_touserdata(L, 1);
	vec2 const * object2 = (vec2 const *)lua_touserdata(L, 2);
	lua_pushnumber(L, dot_product(*object1, *object2));

	return 1;
}

static int vec2_cross(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_ASSERT_USERDATA("vec2", 1);
	LUA_ASSERT_USERDATA("vec2", 2);

	vec2 const * object1 = (vec2 const *)lua_touserdata(L, 1);
	vec2 const * object2 = (vec2 const *)lua_touserdata(L, 2);
	lua_pushnumber(L, cross_product(*object1, *object2));

	return 1;
}

static int vec2_magnitude_squared(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 1, "expected 1 argument");
	LUA_ASSERT_USERDATA("vec2", 1);

	vec2 const * object = (vec2 const *)lua_touserdata(L, 1);
	lua_pushnumber(L, magnitude_squared(*object));

	return 1;
}

static luaL_Reg const vec2_meta[] = {
	{"__index", vec2_index},
	{"__newindex", vec2_newindex},
	{"__add", vec2_add},
	{"__sub", vec2_sub},
	{"__mul", vec2_mul},
	{"__div", vec2_div},
	{"__unm", vec2_unm},
	{"__eq",  vec2_eq},
	// instance:###
	// Type.###
	{"new", vec2_new},
	{"dot", vec2_dot},
	{"cross", vec2_cross},
	{"magnitude_squared", vec2_magnitude_squared},
	{NULL, NULL},
};

static int complex_from_radians(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 1, "expected 1 argument");
	LUA_ASSERT_TYPE(LUA_TNUMBER, 1);

	vec2 * udata = (vec2 *)lua_newuserdatauv(L, sizeof(vec2), 0);
	luaL_setmetatable(L, "vec2");
	*udata = complex_from_radians((r32)lua_tonumber(L, 1));

	return 1;
}

static int complex_product(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_ASSERT_USERDATA("vec2", 1);
	LUA_ASSERT_USERDATA("vec2", 2);

	vec2 const * object1 = (vec2 const *)lua_touserdata(L, 1);
	vec2 const * object2 = (vec2 const *)lua_touserdata(L, 2);

	vec2 * udata = (vec2 *)lua_newuserdatauv(L, sizeof(vec2), 0);
	luaL_setmetatable(L, "vec2");
	*udata = complex_product(*object1, *object2);

	return 1;
}

static luaL_Reg const complex_aux[] = {
	// Lib.###
	{"complex_from_radians", complex_from_radians},
	{"complex_product",      complex_product},
	{NULL, NULL},
};

//
// vec3
//

static int vec3_index(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_INDEX_RAWGET_IMPL(vec3);

	vec3 const * object = (vec3 const *)lua_touserdata(L, 1);
	cstring id = lua_tostring(L, 2);

	// @Optimize?
	if (strcmp(id, "x") == 0) { lua_pushnumber(L, object->x); return 1; }
	if (strcmp(id, "y") == 0) { lua_pushnumber(L, object->y); return 1; }
	if (strcmp(id, "z") == 0) { lua_pushnumber(L, object->z); return 1; }

	LUA_REPORT_INDEX();
	lua_pushnil(L); return 1;
}

static int vec3_newindex(lua_State * L) {
	vec3 * object = (vec3 *)lua_touserdata(L, 1);
	cstring id = lua_tostring(L, 2);

	// @Optimize?
	LUA_ASSERT_TYPE(LUA_TNUMBER, 3);
	if (strcmp(id, "x") == 0) { object->x = (r32)lua_tonumber(L, 3); return 0; }
	if (strcmp(id, "y") == 0) { object->y = (r32)lua_tonumber(L, 3); return 0; }
	if (strcmp(id, "z") == 0) { object->z = (r32)lua_tonumber(L, 3); return 0; }

	LUA_REPORT_INDEX();
	return 0;
}

static int vec3_add(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_ASSERT_USERDATA("vec3", 2);

	vec3 const * object1 = (vec3 const *)lua_touserdata(L, 1);
	vec3 const * object2 = (vec3 const *)lua_touserdata(L, 2);

	vec3 * udata = (vec3 *)lua_newuserdatauv(L, sizeof(vec3), 0);
	luaL_setmetatable(L, "vec3");
	*udata = *object1 + *object2;

	return 1;
}

static int vec3_sub(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_ASSERT_USERDATA("vec3", 2);

	vec3 const * object1 = (vec3 const *)lua_touserdata(L, 1);
	vec3 const * object2 = (vec3 const *)lua_touserdata(L, 2);

	vec3 * udata = (vec3 *)lua_newuserdatauv(L, sizeof(vec3), 0);
	luaL_setmetatable(L, "vec3");
	*udata = *object1 - *object2;

	return 1;
}

static int vec3_mul(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	vec3 const * object1 = (vec3 const *)lua_touserdata(L, 1);

	vec3 * udata = (vec3 *)lua_newuserdatauv(L, sizeof(vec3), 0);
	luaL_setmetatable(L, "vec3");

	if (lua_type(L, 2) == LUA_TNUMBER) {
		*udata = *object1 * (r32)lua_tonumber(L, 2);
	}
	else {
		LUA_ASSERT_USERDATA("vec3", 2);
		vec3 const * object2 = (vec3 const *)lua_touserdata(L, 2);
		*udata = *object1 * *object2;
	}

	return 1;
}

static int vec3_div(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	vec3 const * object1 = (vec3 const *)lua_touserdata(L, 1);

	vec3 * udata = (vec3 *)lua_newuserdatauv(L, sizeof(vec3), 0);
	luaL_setmetatable(L, "vec3");

	if (lua_type(L, 2) == LUA_TNUMBER) {
		*udata = *object1 / (r32)lua_tonumber(L, 2);
	}
	else {
		LUA_ASSERT_USERDATA("vec3", 2);
		vec3 const * object2 = (vec3 const *)lua_touserdata(L, 2);
		*udata = *object1 / *object2;
	}

	return 1;
}

static int vec3_unm(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 1 argument");
	vec3 const * object = (vec3 const *)lua_touserdata(L, 1);

	vec3 * udata = (vec3 *)lua_newuserdatauv(L, sizeof(vec3), 0);
	luaL_setmetatable(L, "vec3");
	*udata = -*object;

	return 1;
}

static int vec3_eq(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_ASSERT_USERDATA("vec3", 2);

	vec3 const * object1 = (vec3 const *)lua_touserdata(L, 1);
	vec3 const * object2 = (vec3 const *)lua_touserdata(L, 2);
	lua_pushboolean(L, *object1 == *object2);

	return 1;
}

static int vec3_new(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 3, "expected 3 arguments");
	LUA_ASSERT_TYPE(LUA_TNUMBER, 1);
	LUA_ASSERT_TYPE(LUA_TNUMBER, 2);
	LUA_ASSERT_TYPE(LUA_TNUMBER, 3);

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
	LUA_ASSERT_USERDATA("vec3", 1);
	LUA_ASSERT_USERDATA("vec3", 2);

	vec3 const * object1 = (vec3 const *)lua_touserdata(L, 1);
	vec3 const * object2 = (vec3 const *)lua_touserdata(L, 2);
	lua_pushnumber(L, dot_product(*object1, *object2));

	return 1;
}

static int vec3_cross(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_ASSERT_USERDATA("vec3", 1);
	LUA_ASSERT_USERDATA("vec3", 2);

	vec3 const * object1 = (vec3 const *)lua_touserdata(L, 1);
	vec3 const * object2 = (vec3 const *)lua_touserdata(L, 2);
	
	vec3 * udata = (vec3 *)lua_newuserdatauv(L, sizeof(vec3), 0);
	luaL_setmetatable(L, "vec3");
	*udata = cross_product(*object1, *object2);

	return 1;
}

static int vec3_magnitude_squared(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 1, "expected 1 argument");
	LUA_ASSERT_USERDATA("vec3", 1);

	vec3 const * object = (vec3 const *)lua_touserdata(L, 1);
	lua_pushnumber(L, magnitude_squared(*object));

	return 1;
}

static luaL_Reg const vec3_meta[] = {
	{"__index", vec3_index},
	{"__newindex", vec3_newindex},
	{"__add", vec3_add},
	{"__sub", vec3_sub},
	{"__mul", vec3_mul},
	{"__div", vec3_div},
	{"__unm", vec3_unm},
	{"__eq",  vec3_eq},
	// instance:###
	// Type.###
	{"new", vec3_new},
	{"dot", vec3_dot},
	{"cross", vec3_cross},
	{"magnitude_squared", vec3_magnitude_squared},
	{NULL, NULL},
};

//
// vec4
//

static int vec4_index(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_INDEX_RAWGET_IMPL(vec4);

	vec4 const * object = (vec4 const *)lua_touserdata(L, 1);
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
	vec4 * object = (vec4 *)lua_touserdata(L, 1);
	cstring id = lua_tostring(L, 2);

	// @Optimize?
	LUA_ASSERT_TYPE(LUA_TNUMBER, 3);
	if (strcmp(id, "x") == 0) { object->x = (r32)lua_tonumber(L, 3); return 0; }
	if (strcmp(id, "y") == 0) { object->y = (r32)lua_tonumber(L, 3); return 0; }
	if (strcmp(id, "z") == 0) { object->z = (r32)lua_tonumber(L, 3); return 0; }
	if (strcmp(id, "w") == 0) { object->w = (r32)lua_tonumber(L, 3); return 0; }

	LUA_REPORT_INDEX();
	return 0;
}

static int vec4_add(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_ASSERT_USERDATA("vec4", 2);

	vec4 const * object1 = (vec4 const *)lua_touserdata(L, 1);
	vec4 const * object2 = (vec4 const *)lua_touserdata(L, 2);

	vec4 * udata = (vec4 *)lua_newuserdatauv(L, sizeof(vec4), 0);
	luaL_setmetatable(L, "vec4");
	*udata = *object1 + *object2;

	return 1;
}

static int vec4_sub(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_ASSERT_USERDATA("vec4", 2);

	vec4 const * object1 = (vec4 const *)lua_touserdata(L, 1);
	vec4 const * object2 = (vec4 const *)lua_touserdata(L, 2);

	vec4 * udata = (vec4 *)lua_newuserdatauv(L, sizeof(vec4), 0);
	luaL_setmetatable(L, "vec4");
	*udata = *object1 - *object2;

	return 1;
}

static int vec4_mul(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	vec4 const * object1 = (vec4 const *)lua_touserdata(L, 1);

	vec4 * udata = (vec4 *)lua_newuserdatauv(L, sizeof(vec4), 0);
	luaL_setmetatable(L, "vec4");

	if (lua_type(L, 2) == LUA_TNUMBER) {
		*udata = *object1 * (r32)lua_tonumber(L, 2);
	}
	else {
		LUA_ASSERT_USERDATA("vec4", 2);
		vec4 const * object2 = (vec4 const *)lua_touserdata(L, 2);
		*udata = *object1 * *object2;
	}

	return 1;
}

static int vec4_div(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	vec4 const * object1 = (vec4 const *)lua_touserdata(L, 1);

	vec4 * udata = (vec4 *)lua_newuserdatauv(L, sizeof(vec4), 0);
	luaL_setmetatable(L, "vec4");

	if (lua_type(L, 2) == LUA_TNUMBER) {
		*udata = *object1 / (r32)lua_tonumber(L, 2);
	}
	else {
		LUA_ASSERT_USERDATA("vec4", 2);
		vec4 const * object2 = (vec4 const *)lua_touserdata(L, 2);
		*udata = *object1 / *object2;
	}

	return 1;
}

static int vec4_unm(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 1 argument");
	vec4 const * object = (vec4 const *)lua_touserdata(L, 1);

	vec4 * udata = (vec4 *)lua_newuserdatauv(L, sizeof(vec4), 0);
	luaL_setmetatable(L, "vec4");
	*udata = -*object;

	return 1;
}

static int vec4_eq(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_ASSERT_USERDATA("vec4", 2);

	vec4 const * object1 = (vec4 const *)lua_touserdata(L, 1);
	vec4 const * object2 = (vec4 const *)lua_touserdata(L, 2);

	lua_pushboolean(L, *object1 == *object2);

	return 1;
}

static int vec4_new(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 4, "expected 4 arguments");
	LUA_ASSERT_TYPE(LUA_TNUMBER, 1);
	LUA_ASSERT_TYPE(LUA_TNUMBER, 2);
	LUA_ASSERT_TYPE(LUA_TNUMBER, 3);
	LUA_ASSERT_TYPE(LUA_TNUMBER, 4);

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
	LUA_ASSERT_USERDATA("vec4", 1);
	LUA_ASSERT_USERDATA("vec4", 2);

	vec4 const * object1 = (vec4 const *)lua_touserdata(L, 1);
	vec4 const * object2 = (vec4 const *)lua_touserdata(L, 2);
	lua_pushnumber(L, dot_product(*object1, *object2));

	return 1;
}

static int vec4_magnitude_squared(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 1, "expected 1 argument");
	LUA_ASSERT_USERDATA("vec4", 1);

	vec4 const * object = (vec4 const *)lua_touserdata(L, 1);
	lua_pushnumber(L, magnitude_squared(*object));

	return 1;
}

static luaL_Reg const vec4_meta[] = {
	{"__index", vec4_index},
	{"__newindex", vec4_newindex},
	{"__add", vec4_add},
	{"__sub", vec4_sub},
	{"__mul", vec4_mul},
	{"__div", vec4_div},
	{"__unm", vec4_unm},
	{"__eq",  vec4_eq},
	// instance:###
	// Type.###
	{"new", vec4_new},
	{"dot", vec4_dot},
	{"magnitude_squared", vec4_magnitude_squared},
	{NULL, NULL},
};

static int quat_from_axis(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_ASSERT_USERDATA("vec3", 1);
	LUA_ASSERT_TYPE(LUA_TNUMBER, 2);

	vec3 const * axis = (vec3 const *)lua_touserdata(L, 1);

	vec4 * udata = (vec4 *)lua_newuserdatauv(L, sizeof(vec4), 0);
	luaL_setmetatable(L, "vec4");
	*udata = quat_from_axis(*axis, (r32)lua_tonumber(L, 2));

	return 1;
}

static int quat_product(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_ASSERT_USERDATA("vec4", 1);
	LUA_ASSERT_USERDATA("vec4", 2);

	vec4 const * object1 = (vec4 const *)lua_touserdata(L, 1);
	vec4 const * object2 = (vec4 const *)lua_touserdata(L, 2);

	vec4 * udata = (vec4 *)lua_newuserdatauv(L, sizeof(vec4), 0);
	luaL_setmetatable(L, "vec4");
	*udata = quat_product(*object1, *object2);

	return 1;
}

static int quat_from_radians(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 3, "expected 3 arguments");
	LUA_ASSERT_TYPE(LUA_TNUMBER, 1);
	LUA_ASSERT_TYPE(LUA_TNUMBER, 2);
	LUA_ASSERT_TYPE(LUA_TNUMBER, 3);

	vec4 * udata = (vec4 *)lua_newuserdatauv(L, sizeof(vec4), 0);
	luaL_setmetatable(L, "vec4");
	*udata = quat_from_radians({
		(r32)lua_tonumber(L, 1),
		(r32)lua_tonumber(L, 2),
		(r32)lua_tonumber(L, 3),
	});

	return 1;
}

static int quat_rotate_vector(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_ASSERT_USERDATA("vec4", 1);
	LUA_ASSERT_USERDATA("vec3", 2);

	vec4 const * object1 = (vec4 const *)lua_touserdata(L, 1);
	vec3 const * object2 = (vec3 const *)lua_touserdata(L, 2);

	vec3 * udata = (vec3 *)lua_newuserdatauv(L, sizeof(vec3), 0);
	luaL_setmetatable(L, "vec3");
	*udata = quat_rotate_vector(*object1, *object2);

	return 1;
}

static int quat_get_right(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 1 argument");
	LUA_ASSERT_USERDATA("vec4", 1);

	vec4 const * object = (vec4 const *)lua_touserdata(L, 1);

	vec3 * udata = (vec3 *)lua_newuserdatauv(L, sizeof(vec3), 0);
	luaL_setmetatable(L, "vec3");
	*udata = quat_get_right(*object);

	return 1;
}

static int quat_get_up(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 1 argument");
	LUA_ASSERT_USERDATA("vec4", 1);

	vec4 const * object = (vec4 const *)lua_touserdata(L, 1);

	vec3 * udata = (vec3 *)lua_newuserdatauv(L, sizeof(vec3), 0);
	luaL_setmetatable(L, "vec3");
	*udata = quat_get_up(*object);

	return 1;
}

static int quat_get_forward(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 1 argument");
	LUA_ASSERT_USERDATA("vec4", 1);

	vec4 const * object = (vec4 const *)lua_touserdata(L, 1);

	vec3 * udata = (vec3 *)lua_newuserdatauv(L, sizeof(vec3), 0);
	luaL_setmetatable(L, "vec3");
	*udata = quat_get_forward(*object);

	return 1;
}

static luaL_Reg const quat_aux[] = {
	// Lib.###
	{"from_axis",     quat_from_axis},
	{"product",       quat_product},
	{"from_radians",  quat_from_radians},
	{"rotate_vector", quat_rotate_vector},
	{"get_right",     quat_get_right},
	{"get_up",        quat_get_up},
	{"get_forward",   quat_get_forward},
	{NULL, NULL},
};

namespace custom {
namespace lua {

void init_math_linear(lua_State * L) {
	LUA_META_IMPL(vec2)
	LUA_META_IMPL(vec3)
	LUA_META_IMPL(vec4)
	//
	LUA_AUX_IMPL(complex)
	LUA_AUX_IMPL(quat)
}

}}
