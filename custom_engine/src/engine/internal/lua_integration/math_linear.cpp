#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/api/lua.h"
#include "engine/impl/math_linear.h"

#include <lua.hpp>

// @Todo: reuse userdata?

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
	{"dot", vec2_dot},
	{"cross", vec2_cross},
	{"magnitude_squared", vec2_magnitude_squared},
	// Type.###
	{"new", vec2_new},
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
	// Lib_instance.###
	{"complex_product",      complex_product},
	// Lib_Type.###
	{"complex_from_radians", complex_from_radians},
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
	{"dot", vec3_dot},
	{"cross", vec3_cross},
	{"magnitude_squared", vec3_magnitude_squared},
	// Type.###
	{"new", vec3_new},
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
	{"dot", vec4_dot},
	{"magnitude_squared", vec4_magnitude_squared},
	// Type.###
	{"new", vec4_new},
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

static int quat_rotate(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_ASSERT_USERDATA("vec4", 1);
	LUA_ASSERT_USERDATA("vec3", 2);

	vec4 const * object1 = (vec4 const *)lua_touserdata(L, 1);
	vec3 const * object2 = (vec3 const *)lua_touserdata(L, 2);

	vec3 * udata = (vec3 *)lua_newuserdatauv(L, sizeof(vec3), 0);
	luaL_setmetatable(L, "vec3");
	*udata = quat_rotate(*object1, *object2);

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
	// Lib_instance.###
	{"product",      quat_product},
	{"rotate",       quat_rotate},
	{"get_right",    quat_get_right},
	{"get_up",       quat_get_up},
	{"get_forward",  quat_get_forward},
	// Lib_Type.###
	{"from_axis",    quat_from_axis},
	{"from_radians", quat_from_radians},
	{NULL, NULL},
};

//
// mat4
//

static int mat4_index(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_INDEX_RAWGET_IMPL(mat4);

	mat4 const * object = (mat4 const *)lua_touserdata(L, 1);
	cstring id = lua_tostring(L, 2);

	// @Optimize?
	if (strcmp(id, "x") == 0) {
		vec4 * udata = (vec4 *)lua_newuserdatauv(L, sizeof(vec4), 0);
		luaL_setmetatable(L, "vec4");
		*udata = object->x;
		return 1;
	}

	if (strcmp(id, "y") == 0) {
		vec4 * udata = (vec4 *)lua_newuserdatauv(L, sizeof(vec4), 0);
		luaL_setmetatable(L, "vec4");
		*udata = object->y;
		return 1;
	}

	if (strcmp(id, "z") == 0) {
		vec4 * udata = (vec4 *)lua_newuserdatauv(L, sizeof(vec4), 0);
		luaL_setmetatable(L, "vec4");
		*udata = object->z;
		return 1;
	}

	if (strcmp(id, "w") == 0) {
		vec4 * udata = (vec4 *)lua_newuserdatauv(L, sizeof(vec4), 0);
		luaL_setmetatable(L, "vec4");
		*udata = object->w;
		return 1;
	}

	LUA_REPORT_INDEX();
	lua_pushnil(L); return 1;
}

static int mat4_newindex(lua_State * L) {	
	mat4 * object = (mat4 *)lua_touserdata(L, 1);
	cstring id = lua_tostring(L, 2);

	// @Optimize?
	if (strcmp(id, "x") == 0) {
		LUA_ASSERT_USERDATA("vec4", 3);
		vec4 const * value = (vec4 const *)lua_touserdata(L, 3);
		object->x = *value;
		return 0;
	}

	if (strcmp(id, "y") == 0) {
		LUA_ASSERT_USERDATA("vec4", 3);
		vec4 const * value = (vec4 const *)lua_touserdata(L, 3);
		object->y = *value;
		return 0;
	}

	if (strcmp(id, "z") == 0) {
		LUA_ASSERT_USERDATA("vec4", 3);
		vec4 const * value = (vec4 const *)lua_touserdata(L, 3);
		object->z = *value;
		return 0;
	}

	if (strcmp(id, "w") == 0) {
		LUA_ASSERT_USERDATA("vec4", 3);
		vec4 const * value = (vec4 const *)lua_touserdata(L, 3);
		object->w = *value;
		return 0;
	}

	LUA_REPORT_INDEX();
	return 0;
}

static int mat4_transform(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_ASSERT_USERDATA("mat4", 1);
	LUA_ASSERT_USERDATA("mat4", 2);

	mat4 const * object1 = (mat4 const *)lua_touserdata(L, 1);
	mat4 const * object2 = (mat4 const *)lua_touserdata(L, 2);

	mat4 * udata = (mat4 *)lua_newuserdatauv(L, sizeof(mat4), 0);
	luaL_setmetatable(L, "mat4");
	*udata = mat_transform(*object1, *object2);

	return 1;
}

static int mat4_transform_vec(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_ASSERT_USERDATA("mat4", 1);
	LUA_ASSERT_USERDATA("vec4", 2);

	mat4 const * object1 = (mat4 const *)lua_touserdata(L, 1);
	vec4 const * object2 = (vec4 const *)lua_touserdata(L, 2);

	vec4 * udata = (vec4 *)lua_newuserdatauv(L, sizeof(vec4), 0);
	luaL_setmetatable(L, "vec4");
	*udata = mat_transform(*object1, *object2);

	return 1;
}

static int mat4_persp(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 4, "expected 4 arguments");
	LUA_ASSERT_TYPE(LUA_TNUMBER, 1);
	LUA_ASSERT_TYPE(LUA_TNUMBER, 2);
	LUA_ASSERT_TYPE(LUA_TNUMBER, 3);
	LUA_ASSERT_TYPE(LUA_TNUMBER, 4);

	mat4 * udata = (mat4 *)lua_newuserdatauv(L, sizeof(mat4), 0);
	luaL_setmetatable(L, "mat4");
	*udata = mat_persp(
		{(r32)lua_tonumber(L, 1), (r32)lua_tonumber(L, 2)},
		(r32)lua_tonumber(L, 3),
		(r32)lua_tonumber(L, 4)
	);

	return 1;
}

static int mat4_ortho(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 4, "expected 4 arguments");
	LUA_ASSERT_TYPE(LUA_TNUMBER, 1);
	LUA_ASSERT_TYPE(LUA_TNUMBER, 2);
	LUA_ASSERT_TYPE(LUA_TNUMBER, 3);
	LUA_ASSERT_TYPE(LUA_TNUMBER, 4);

	mat4 * udata = (mat4 *)lua_newuserdatauv(L, sizeof(mat4), 0);
	luaL_setmetatable(L, "mat4");
	*udata = mat_ortho(
		{(r32)lua_tonumber(L, 1), (r32)lua_tonumber(L, 2)},
		(r32)lua_tonumber(L, 3),
		(r32)lua_tonumber(L, 4)
	);

	return 1;
}

static luaL_Reg const mat4_meta[] = {
	{"__index", mat4_index},
	{"__newindex", mat4_newindex},
	// instance:###
	{"transform", mat4_transform},
	{"transform_vec", mat4_transform_vec},
	// Type.###
	{"persp", mat4_persp},
	{"ortho", mat4_ortho},
	{NULL, NULL},
};

//
//
//

namespace custom {
namespace lua {

void init_math_linear(lua_State * L) {
	LUA_META_IMPL(vec2)
	LUA_META_IMPL(vec3)
	LUA_META_IMPL(vec4)
	//
	LUA_META_IMPL(mat4)
	//
	LUA_AUX_IMPL(complex)
	LUA_AUX_IMPL(quat)
}

}}
