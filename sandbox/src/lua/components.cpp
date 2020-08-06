#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/api/internal/entity_system.h"
#include "../components/types.h"

#include <lua.hpp>
// #include <lstate.h>

static luaL_Reg const Lua_Script_meta[] = {
	{"__index", NULL}, /* place holder */
	{NULL, NULL},
};

static luaL_Reg const Lua_Script_methods[] = {
	{NULL, NULL},
};

static luaL_Reg const Lua_Script_lib[] = {
	{NULL, NULL},
};

static int transform_get_position(lua_State * L) {
	CUSTOM_ASSERT(lua_gettop(L) == 1, "expected 1 argument");
	custom::RefT<Transform> * component_ref = (custom::RefT<Transform> *)luaL_checkudata(L, 1, "Transform");
	if (!component_ref->exists()) { lua_pushnil(L); return 1; }
	vec3 * udata = (vec3 *)lua_newuserdatauv(L, sizeof(vec3), 0);
	luaL_setmetatable(L, "vec3");
	*udata = component_ref->get_fast()->position;
	return 1;
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
	{"__index", NULL}, /* place holder */
	{NULL, NULL},
};

static luaL_Reg const Transform_methods[] = {
	{"get_position", transform_get_position},
	{"set", transform_set},
	{NULL, NULL},
};

static luaL_Reg const Transform_lib[] = {
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

static luaL_Reg const Visual_meta[] = {
	{"__index", NULL}, /* place holder */
	{NULL, NULL},
};

static luaL_Reg const Visual_methods[] = {
	{"get_shader", visual_get_shader},
	{"set", visual_set},
	{NULL, NULL},
};

static luaL_Reg const Visual_lib[] = {
	{NULL, NULL},
};

namespace custom {
namespace lua_client {

void init_components(lua_State * L) {
	#define COMPONENT_IMPL(T)\
		if (luaL_newmetatable(L, #T)) {\
			luaL_setfuncs(L, T##_meta, 0);\
			lua_pushvalue(L, -1);\
			\
			luaL_newlib(L, T##_methods);\
			lua_setfield(L, -2, "__index");\
			\
			luaL_setfuncs(L, T##_lib, 0);\
			lua_setglobal(L, #T);\
		}\
		else { lua_pop(L, 1); }\

	#include "../components/registry_impl.h"
}

}}
