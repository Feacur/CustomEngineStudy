#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/api/lua.h"
#include "engine/api/internal/entity_system.h"
#include "../components/types.h"

#include <lua.hpp>
#include <lstate.h>

static luaL_Reg const Lua_Script_meta[] = {
	{"__index", NULL},
	//
	{NULL, NULL},
};

static int transform_index(lua_State * L) {
	if (!lua_getmetatable(L, 1)) { lua_pushnil(L); return 1; }

	lua_pushvalue(L, 2);
	int type = lua_rawget(L, -2);
	lua_remove(L, -2);
	if (type != LUA_TNIL) { return 1; }

	custom::RefT<Transform> * component_ref = (custom::RefT<Transform> *)lua_touserdata(L, 1);
	if (!component_ref->exists()) { return 1; }
	lua_pop(L, 1);

	cstring id = lua_tostring(L, 2);

	// @Optimize?
	if (strcmp(id, "position") == 0) {
		vec3 * udata = (vec3 *)lua_newuserdatauv(L, sizeof(vec3), 0);
		luaL_setmetatable(L, "vec3");
		*udata = component_ref->get_fast()->position;
		return 1;
	}

	if (strcmp(id, "rotation") == 0) {
		vec4 * udata = (vec4 *)lua_newuserdatauv(L, sizeof(vec4), 0);
		luaL_setmetatable(L, "vec4");
		*udata = component_ref->get_fast()->rotation;
		return 1;
	}

	if (strcmp(id, "scale") == 0) {
		vec3 * udata = (vec3 *)lua_newuserdatauv(L, sizeof(vec3), 0);
		luaL_setmetatable(L, "vec3");
		*udata = component_ref->get_fast()->scale;
		return 1;
	}

	lua_pushnil(L); return 1;
}

static int transform_set(lua_State * L) {
	CUSTOM_ASSERT(lua_gettop(L) == 1, "expected 1 argument");
	custom::RefT<Transform> * component_ref = (custom::RefT<Transform> *)luaL_checkudata(L, 1, "Transform");
	if (!component_ref->exists()) { lua_pushnil(L); return 1; }

	Transform * component = component_ref->get_fast();
	(*component) = {{0, 1, 0}, {0, 0, 0, 1}, {1, 1, 1}};

	return 0;
}

static luaL_Reg const Transform_meta[] = {
	{"__index", transform_index},
	// instance:###
	{"set", transform_set},
	//
	{NULL, NULL},
};

static int visual_get_shader(lua_State * L) {
	custom::RefT<Visual> * component_ref = (custom::RefT<Visual> *)luaL_checkudata(L, 1, "Visual");
	if (!component_ref->exists()) { lua_pushnil(L); return 1; }
	lua_pushinteger(L, component_ref->get_fast()->shader);
	return 1;
}

static int visual_set(lua_State * L) {
	custom::RefT<Visual> * component_ref = (custom::RefT<Visual> *)luaL_checkudata(L, 1, "Visual");
	if (!component_ref->exists()) { lua_pushnil(L); return 1; }

	Visual * component = component_ref->get_fast();
	(*component) = {1, 0, 1};

	return 0;
}

static int visual_index(lua_State * L) {
	if (!lua_getmetatable(L, 1)) { lua_pushnil(L); return 1; }

	lua_pushvalue(L, 2);
	int type = lua_rawget(L, -2);
	lua_remove(L, -2);
	if (type != LUA_TNIL) { return 1; }

	custom::RefT<Visual> * component_ref = (custom::RefT<Visual> *)lua_touserdata(L, 1);
	if (!component_ref->exists()) { return 1; }
	lua_pop(L, 1);

	cstring id = lua_tostring(L, 2);

	// @Optimize?
	if (strcmp(id, "shader") == 0) {
		lua_pushinteger(L, component_ref->get_fast()->shader);
		return 1;
	}
	
	if (strcmp(id, "texture") == 0) {
		lua_pushinteger(L, component_ref->get_fast()->texture);
		return 1;
	}
	
	if (strcmp(id, "mesh") == 0) {
		lua_pushinteger(L, component_ref->get_fast()->mesh);
		return 1;
	}

	lua_pushnil(L); return 1;
}

static luaL_Reg const Visual_meta[] = {
	{"__index", visual_index},
	// instance:###
	{"get_shader", visual_get_shader},
	{"set", visual_set},
	{NULL, NULL},
};

namespace custom {
namespace lua_client {

void init_components(lua_State * L) {
	#define COMPONENT_IMPL(T) LUA_META_IMPL(T)
	#include "../components/registry_impl.h"
}

}}
