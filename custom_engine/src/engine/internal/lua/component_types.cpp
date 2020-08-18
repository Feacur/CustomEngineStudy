#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/api/lua.h"
#include "engine/api/internal/component_types.h"
#include "engine/impl/asset_system.h"

#include <lua.hpp>

// @Todo: reuse userdata?

//
// Transform
//

static int Transform_index(lua_State * L) {
	typedef custom::RefT<Transform> Ref;

	LUA_INDEX_RAWGET_IMPL(Transform);

	Ref const * object = (Ref const *)lua_touserdata(L, 1);
	CUSTOM_LUA_ASSERT(object->exists(), "object doesn't exist");

	cstring id = lua_tostring(L, 2);

	// @Optimize?
	if (strcmp(id, "position") == 0) {
		vec3 * udata = (vec3 *)lua_newuserdatauv(L, sizeof(vec3), 0);
		luaL_setmetatable(L, "vec3");
		*udata = object->get_fast()->position;
		return 1;
	}

	if (strcmp(id, "rotation") == 0) {
		vec4 * udata = (vec4 *)lua_newuserdatauv(L, sizeof(vec4), 0);
		luaL_setmetatable(L, "vec4");
		*udata = object->get_fast()->rotation;
		return 1;
	}

	if (strcmp(id, "scale") == 0) {
		vec3 * udata = (vec3 *)lua_newuserdatauv(L, sizeof(vec3), 0);
		luaL_setmetatable(L, "vec3");
		*udata = object->get_fast()->scale;
		return 1;
	}

	LUA_REPORT_INDEX();
	lua_pushnil(L); return 1;
}

static int Transform_newindex(lua_State * L) {
	typedef custom::RefT<Transform> Ref;

	Ref * object = (Ref *)lua_touserdata(L, 1);
	CUSTOM_LUA_ASSERT(object->exists(), "object doesn't exist");

	cstring id = lua_tostring(L, 2);

	// @Optimize?
	if (strcmp(id, "position") == 0) {
		LUA_ASSERT_USERDATA("vec3", 3);
		vec3 const * value = (vec3 const *)lua_touserdata(L, 3);
		object->get_fast()->position = *value;
		return 0;
	}

	if (strcmp(id, "rotation") == 0) {
		LUA_ASSERT_USERDATA("vec4", 3);
		vec4 const * value = (vec4 const *)lua_touserdata(L, 3);
		object->get_fast()->rotation = *value;
		return 0;
	}

	if (strcmp(id, "scale") == 0) {
		LUA_ASSERT_USERDATA("vec3", 3);
		vec3 const * value = (vec3 const *)lua_touserdata(L, 3);
		object->get_fast()->scale = *value;
		return 0;
	}

	LUA_REPORT_INDEX();
	return 0;
}

static int Transform_eq(lua_State * L) {
	typedef custom::RefT<Transform> Ref;

	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_ASSERT_USERDATA("Transform", 2);

	Ref const * object1 = (Ref const *)lua_touserdata(L, 1);
	Ref const * object2 = (Ref const *)lua_touserdata(L, 2);

	lua_pushboolean(L, *object1 == *object2);

	return 1;
}

static luaL_Reg const Transform_meta[] = {
	{"__index", Transform_index},
	{"__newindex", Transform_newindex},
	{"__eq", Transform_eq},
	//
	{NULL, NULL},
};

//
// Camera
//

static int Camera_index(lua_State * L) {
	typedef custom::RefT<Camera> Ref;

	LUA_INDEX_RAWGET_IMPL(Camera);

	Ref const * object = (Ref const *)lua_touserdata(L, 1);
	CUSTOM_LUA_ASSERT(object->exists(), "object doesn't exist");

	cstring id = lua_tostring(L, 2);

	// @Optimize?
	if (strcmp(id, "near")  == 0) { lua_pushnumber(L, object->get_fast()->near);  return 1; }
	if (strcmp(id, "far")   == 0) { lua_pushnumber(L, object->get_fast()->far);   return 1; }
	if (strcmp(id, "scale") == 0) { lua_pushnumber(L, object->get_fast()->scale); return 1; }
	if (strcmp(id, "persp") == 0) { lua_pushnumber(L, object->get_fast()->persp); return 1; }

	LUA_REPORT_INDEX();
	lua_pushnil(L); return 1;
}

static int Camera_newindex(lua_State * L) {
	typedef custom::RefT<Camera> Ref;

	Ref * object = (Ref *)lua_touserdata(L, 1);
	CUSTOM_LUA_ASSERT(object->exists(), "object doesn't exist");

	cstring id = lua_tostring(L, 2);

	// @Optimize?
	LUA_ASSERT_TYPE(LUA_TNUMBER, 3);
	if (strcmp(id, "near")  == 0) { object->get_fast()->near  = (r32)lua_tonumber(L, 3); return 0; }
	if (strcmp(id, "far")   == 0) { object->get_fast()->far   = (r32)lua_tonumber(L, 3); return 0; }
	if (strcmp(id, "scale") == 0) { object->get_fast()->scale = (r32)lua_tonumber(L, 3); return 0; }
	if (strcmp(id, "persp") == 0) { object->get_fast()->persp = (r32)lua_tonumber(L, 3); return 0; }

	LUA_REPORT_INDEX();
	return 0;
}

static int Camera_eq(lua_State * L) {
	typedef custom::RefT<Camera> Ref;

	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_ASSERT_USERDATA("Camera", 2);

	Ref const * object1 = (Ref const *)lua_touserdata(L, 1);
	Ref const * object2 = (Ref const *)lua_touserdata(L, 2);

	lua_pushboolean(L, *object1 == *object2);

	return 1;
}

static luaL_Reg const Camera_meta[] = {
	{"__index", Camera_index},
	{"__newindex", Camera_newindex},
	{"__eq", Camera_eq},
	//
	{NULL, NULL},
};

//
//
//

namespace custom {
namespace lua {

void init_component_types(lua_State * L) {
	#define COMPONENT_IMPL(T) LUA_META_IMPL(T)
	#include "../components_registry_impl.h"
}

}}