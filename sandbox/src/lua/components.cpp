#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/api/lua.h"
#include "engine/api/internal/entity_system.h"
#include "../components/types.h"

#include <lua.hpp>
#include <lstate.h>

static int Lua_Script_index(lua_State * L) {
	if (!lua_getmetatable(L, 1)) { lua_pushnil(L); return 1; }

	lua_pushvalue(L, 2);
	int type = lua_rawget(L, -2);
	lua_remove(L, -2);
	if (type != LUA_TNIL) { return 1; }

	custom::RefT<Lua_Script> * object = (custom::RefT<Lua_Script> *)lua_touserdata(L, 1);
	if (!object->exists()) { return 1; }
	lua_pop(L, 1);

	cstring id = lua_tostring(L, 2);

	// @Optimize?
	if (strcmp(id, "function") == 0) { lua_pushstring(L, object->get_fast()->function); return 1; }

	LUA_INDEX_ASSERT();
	lua_pushnil(L); return 1;
}

static int Lua_Script_newindex(lua_State * L) {
	custom::RefT<Lua_Script> * object = (custom::RefT<Lua_Script> *)lua_touserdata(L, 1);
	if (!object->exists()) { return 0; }

	cstring id = lua_tostring(L, 2);

	// @Optimize?
	if (strcmp(id, "function") == 0) { object->get_fast()->function = lua_tostring(L, 3); return 0; }

	LUA_INDEX_ASSERT();
	return 0;
}

static luaL_Reg const Lua_Script_meta[] = {
	{"__index", Lua_Script_index},
	{"__newindex", Lua_Script_newindex},
	//
	{NULL, NULL},
};

static int Transform_index(lua_State * L) {
	if (!lua_getmetatable(L, 1)) { lua_pushnil(L); return 1; }

	lua_pushvalue(L, 2);
	int type = lua_rawget(L, -2);
	lua_remove(L, -2);
	if (type != LUA_TNIL) { return 1; }

	custom::RefT<Transform> * object = (custom::RefT<Transform> *)lua_touserdata(L, 1);
	if (!object->exists()) { return 1; }
	lua_pop(L, 1);

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

	LUA_INDEX_ASSERT();
	lua_pushnil(L); return 1;
}

static int Transform_newindex(lua_State * L) {
	custom::RefT<Transform> * object = (custom::RefT<Transform> *)lua_touserdata(L, 1);
	if (!object->exists()) { return 0; }

	cstring id = lua_tostring(L, 2);

	// @Optimize?
	if (strcmp(id, "position") == 0) {
		vec3 const * value = (vec3 const *)luaL_checkudata(L, 3, "vec3");
		object->get_fast()->position = *value;
		return 0;
	}

	if (strcmp(id, "rotation") == 0) {
		vec4 const * value = (vec4 const *)luaL_checkudata(L, 3, "vec4");
		object->get_fast()->rotation = *value;
		return 0;
	}

	if (strcmp(id, "scale") == 0) {
		vec3 const * value = (vec3 const *)luaL_checkudata(L, 3, "vec3");
		object->get_fast()->scale = *value;
		return 0;
	}

	LUA_INDEX_ASSERT();
	return 0;
}

static luaL_Reg const Transform_meta[] = {
	{"__index", Transform_index},
	{"__newindex", Transform_newindex},
	//
	{NULL, NULL},
};

static int Visual_index(lua_State * L) {
	if (!lua_getmetatable(L, 1)) { lua_pushnil(L); return 1; }

	lua_pushvalue(L, 2);
	int type = lua_rawget(L, -2);
	lua_remove(L, -2);
	if (type != LUA_TNIL) { return 1; }

	custom::RefT<Visual> * object = (custom::RefT<Visual> *)lua_touserdata(L, 1);
	if (!object->exists()) { return 1; }
	lua_pop(L, 1);

	cstring id = lua_tostring(L, 2);

	// @Optimize?
	if (strcmp(id, "shader") == 0) { lua_pushinteger(L, object->get_fast()->shader); return 1; }
	if (strcmp(id, "texture") == 0) { lua_pushinteger(L, object->get_fast()->texture); return 1; }
	if (strcmp(id, "mesh") == 0) { lua_pushinteger(L, object->get_fast()->mesh); return 1; }

	LUA_INDEX_ASSERT();
	lua_pushnil(L); return 1;
}

static int Visual_newindex(lua_State * L) {
	custom::RefT<Visual> * object = (custom::RefT<Visual> *)lua_touserdata(L, 1);
	if (!object->exists()) { return 0; }

	cstring id = lua_tostring(L, 2);

	// @Optimize?
	if (strcmp(id, "shader") == 0) { object->get_fast()->shader = (u32)lua_tointeger(L, 3); return 1; }
	if (strcmp(id, "texture") == 0) { object->get_fast()->texture = (u32)lua_tointeger(L, 3); return 1; }
	if (strcmp(id, "mesh") == 0) { object->get_fast()->mesh = (u32)lua_tointeger(L, 3); return 1; }

	LUA_INDEX_ASSERT();
	return 0;
}

static luaL_Reg const Visual_meta[] = {
	{"__index", Visual_index},
	{"__newindex", Visual_newindex},
	{NULL, NULL},
};

namespace custom {
namespace lua_client {

void init_components(lua_State * L) {
	#define COMPONENT_IMPL(T) LUA_META_IMPL(T)
	#include "../components/registry_impl.h"
}

}}
