#include "custom_pch.h"
#include "engine/api/internal/entity_system.h"

#include <lua.hpp>

static int entity_to_string(lua_State * L) {
	custom::Entity * e = (custom::Entity *)lua_touserdata(L, 1);
	luaL_argcheck(L, e != NULL, 1, "'Entity' expected");
	lua_pushfstring(L, "{Entity (%d:%d)}", e->id, e->gen);
	return 1;
}

static const luaL_Reg Entity_MT[] = {
	{"__tostring", entity_to_string},
	{NULL, NULL},
};

namespace custom {
namespace lua_entity_system {

void init(lua_State * L) {
	if (luaL_newmetatable(L, "Entity")) {
		luaL_setfuncs(L, Entity_MT, 0);
	}
	lua_pop(L, 1);
}

}}
