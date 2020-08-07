#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/api/lua.h"
#include "engine/api/internal/entity_system.h"
#include "../components/types.h"

#include <lua.hpp>
// #include <lstate.h>

static int Lua_Script_index(lua_State * L) {
	LUA_INDEX_RAWGET_IMPL(Lua_Script)

	// LUA_ASSERT_USERDATA(1, "Lua_Script");
	custom::RefT<Lua_Script> * object = (custom::RefT<Lua_Script> *)lua_touserdata(L, 1);
	CUSTOM_LUA_ASSERT(object->exists(), "object doesn't exist");

	cstring id = lua_tostring(L, 2);

	// @Optimize?
	if (strcmp(id, "update") == 0) { lua_pushstring(L, object->get_fast()->update); return 1; }

	LUA_REPORT_INDEX();
	lua_pushnil(L); return 1;
}

static int Lua_Script_newindex(lua_State * L) {
	LUA_ASSERT_USERDATA(1, "Lua_Script");
	custom::RefT<Lua_Script> * object = (custom::RefT<Lua_Script> *)lua_touserdata(L, 1);
	CUSTOM_LUA_ASSERT(object->exists(), "object doesn't exist");

	cstring id = lua_tostring(L, 2);

	// @Optimize?
	if (strcmp(id, "update") == 0) {
		CUSTOM_LUA_ASSERT(lua_type(L, 3) == LUA_TSTRING, "expected a string");
		object->get_fast()->update = lua_tostring(L, 3); return 0;
	}

	LUA_REPORT_INDEX();
	return 0;
}

static luaL_Reg const Lua_Script_meta[] = {
	{"__index", Lua_Script_index},
	{"__newindex", Lua_Script_newindex},
	//
	{NULL, NULL},
};

static int Transform_index(lua_State * L) {
	LUA_INDEX_RAWGET_IMPL(Transform)

	// LUA_ASSERT_USERDATA(1, "Transform");
	custom::RefT<Transform> * object = (custom::RefT<Transform> *)lua_touserdata(L, 1);
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
	LUA_ASSERT_USERDATA(1, "Transform");
	custom::RefT<Transform> * object = (custom::RefT<Transform> *)lua_touserdata(L, 1);
	CUSTOM_LUA_ASSERT(object->exists(), "object doesn't exist");

	cstring id = lua_tostring(L, 2);

	// @Optimize?
	if (strcmp(id, "position") == 0) {
		LUA_ASSERT_USERDATA(3, "vec3");
		vec3 const * value = (vec3 const *)lua_touserdata(L, 3);
		object->get_fast()->position = *value;
		return 0;
	}

	if (strcmp(id, "rotation") == 0) {
		LUA_ASSERT_USERDATA(3, "vec4");
		vec4 const * value = (vec4 const *)lua_touserdata(L, 3);
		object->get_fast()->rotation = *value;
		return 0;
	}

	if (strcmp(id, "scale") == 0) {
		LUA_ASSERT_USERDATA(3, "vec3");
		vec3 const * value = (vec3 const *)lua_touserdata(L, 3);
		object->get_fast()->scale = *value;
		return 0;
	}

	LUA_REPORT_INDEX();
	return 0;
}

static luaL_Reg const Transform_meta[] = {
	{"__index", Transform_index},
	{"__newindex", Transform_newindex},
	//
	{NULL, NULL},
};

static int Visual_index(lua_State * L) {
	LUA_INDEX_RAWGET_IMPL(Visual)

	// LUA_ASSERT_USERDATA(1, "Visual");
	custom::RefT<Visual> * object = (custom::RefT<Visual> *)lua_touserdata(L, 1);
	CUSTOM_LUA_ASSERT(object->exists(), "object doesn't exist");

	cstring id = lua_tostring(L, 2);

	// @Optimize?
	if (strcmp(id, "shader") == 0) { lua_pushinteger(L, object->get_fast()->shader); return 1; }
	if (strcmp(id, "texture") == 0) { lua_pushinteger(L, object->get_fast()->texture); return 1; }
	if (strcmp(id, "mesh") == 0) { lua_pushinteger(L, object->get_fast()->mesh); return 1; }

	LUA_REPORT_INDEX();
	lua_pushnil(L); return 1;
}

static int Visual_newindex(lua_State * L) {
	LUA_ASSERT_USERDATA(1, "Visual");
	custom::RefT<Visual> * object = (custom::RefT<Visual> *)lua_touserdata(L, 1);
	CUSTOM_LUA_ASSERT(object->exists(), "object doesn't exist");

	cstring id = lua_tostring(L, 2);

	// @Optimize?
	if (strcmp(id, "shader") == 0) {
		CUSTOM_LUA_ASSERT(lua_type(L, 3) == LUA_TNUMBER, "expected a number");
		object->get_fast()->shader = (u32)lua_tonumber(L, 3); return 1;
	}
	if (strcmp(id, "texture") == 0) {
		CUSTOM_LUA_ASSERT(lua_type(L, 3) == LUA_TNUMBER, "expected a number");
		object->get_fast()->texture = (u32)lua_tonumber(L, 3); return 1;
	}
	if (strcmp(id, "mesh") == 0) {
		CUSTOM_LUA_ASSERT(lua_type(L, 3) == LUA_TNUMBER, "expected a number");
		object->get_fast()->mesh = (u32)lua_tonumber(L, 3); return 1;
	}

	LUA_REPORT_INDEX();
	return 0;
}

static luaL_Reg const Visual_meta[] = {
	{"__index", Visual_index},
	{"__newindex", Visual_newindex},
	//
	{NULL, NULL},
};

namespace custom {
namespace lua_client {

void init_components(lua_State * L) {
	#define COMPONENT_IMPL(T) LUA_META_IMPL(T)
	#include "../components/registry_impl.h"
}

}}
