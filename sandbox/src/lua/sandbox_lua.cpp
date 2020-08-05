#include "engine/api/internal/entity_system.h"
#include "../entity_system/components.h"

#include <lua.hpp>
// #include <lstate.h>

#define COMPONENT_IMPL(T)\
static int entity_get_##T(lua_State * L) {\
	/*if (lua_isnil(L, 1)) { lua_pushnil(L); return 1; }*/\
	custom::Entity * entity = (custom::Entity *)luaL_checkudata(L, 1, "Entity");\
	if (!entity->exists()) { lua_pushnil(L); return 1; }\
	custom::RefT<T> component_ref = entity->get_component<T>();\
	if (!component_ref.exists()) { lua_pushnil(L); return 1; }\
	custom::RefT<T> * udata = (custom::RefT<T> *)lua_newuserdatauv(L, sizeof(custom::RefT<T>), 0);\
	luaL_setmetatable(L, #T);\
	*udata = component_ref;\
	return 1;\
}\

#include "../entity_system/components_registry_impl.h"

#define COMPONENT_IMPL(T) {"get_" #T, entity_get_##T},
static luaL_Reg const entity_components_mt[] = {
	#include "../entity_system/components_registry_impl.h"
	{NULL, NULL},
};

static luaL_Reg const Lua_Script_mt[] = {
	{"__index", NULL}, /* place holder */
	{NULL, NULL},
};

static int transform_get_position(lua_State * L) {
	/*if (lua_isnil(L, 1)) { lua_pushnil(L); return 1; }*/
	custom::RefT<Transform> * component_ref = (custom::RefT<Transform> *)luaL_checkudata(L, 1, "Transform");
	if (!component_ref->exists()) { lua_pushnil(L); return 1; }
	vec3 * udata = (vec3 *)lua_newuserdatauv(L, sizeof(vec3), 0);
	luaL_setmetatable(L, "vec3");
	*udata = component_ref->get_fast()->position;
	return 1;
}

static luaL_Reg const Transform_mt[] = {
	{"__index", NULL}, /* place holder */
	{"get_position", transform_get_position},
	{NULL, NULL},
};

static int visual_get_shader(lua_State * L) {
	/*if (lua_isnil(L, 1)) { lua_pushnil(L); return 1; }*/
	custom::RefT<Visual> * component_ref = (custom::RefT<Visual> *)luaL_checkudata(L, 1, "Visual");
	if (!component_ref->exists()) { lua_pushnil(L); return 1; }
	lua_pushinteger(L, component_ref->get_fast()->shader);
	return 1;
}

static luaL_Reg const Visual_mt[] = {
	{"__index", NULL}, /* place holder */
	{"get_shader", visual_get_shader},
	{NULL, NULL},
};

namespace sandbox {
namespace lua {

void init(lua_State * L) {
	if (luaL_getmetatable(L, "Entity") != LUA_TNIL) {
		luaL_setfuncs(L, entity_components_mt, 0);
	}
	lua_pop(L, 1);

	#define COMPONENT_IMPL(T)\
		if (luaL_newmetatable(L, #T)) {\
			luaL_setfuncs(L, T##_mt, 0);\
			lua_setfield(L, -1, "__index");\
		}\
		else { lua_pop(L, 1); }\

	#include "../entity_system/components_registry_impl.h"
}

}}
