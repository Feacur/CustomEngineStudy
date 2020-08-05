#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/api/internal/entity_system.h"
#include "engine/api/client/lua.h"

#include <lua.hpp>
// #include <lstate.h>

static int entity_tostring(lua_State * L) {
	custom::Entity * entity = (custom::Entity *)luaL_checkudata(L, 1, "Entity");
	lua_pushfstring(L, "(e %d:%d)", entity->id, entity->gen);
	return 1;
}

static int entity_eq(lua_State * L) {
	custom::Entity * entity1 = (custom::Entity *)luaL_checkudata(L, 1, "Entity");
	custom::Entity * entity2 = (custom::Entity *)luaL_testudata(L, 2, "Entity");
	lua_pushboolean(L, entity2 ? ((entity1 == entity2) || (*entity1 == *entity2)) : !entity1->exists());
	return 1;
}

static luaL_Reg const entity_mt[] = {
	{"__index", NULL}, /* place holder */
	{"__tostring", entity_tostring},
	{"__eq", entity_eq},
	{NULL, NULL},
};

namespace custom {
namespace lua_entity_system {

void init(lua_State * L) {
	if (luaL_newmetatable(L, "Entity")) {
		luaL_setfuncs(L, entity_mt, 0);
		lua_client::init_entity(L);
		lua_setfield(L, -1, "__index");
	}
	else { lua_pop(L, 1); }
}

}}
