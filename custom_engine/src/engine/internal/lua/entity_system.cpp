#include "custom_pch.h"
#include "engine/api/internal/entity_system.h"

#include <lua.hpp>
// #include <lstate.h>

static int entity_tostring(lua_State * L) {
	custom::Entity * e = (custom::Entity *)luaL_checkudata(L, 1, "Entity");
	lua_pushfstring(L, "(e %d:%d)", e->id, e->gen);
	return 1;
}

static luaL_Reg const entity_mt[] = {
	{"__index", NULL}, /* place holder */
	{"__tostring", entity_tostring},
	{NULL, NULL},
};

namespace custom {
namespace lua_entity_system {

void init(lua_State * L) {
	if (luaL_newmetatable(L, "Entity")) {
		luaL_setfuncs(L, entity_mt, 0);
		lua_setfield(L, -1, "__index");
	}
	else { lua_pop(L, 1); }
}

}}
