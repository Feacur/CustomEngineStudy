#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/api/internal/entity_system.h"
#include "engine/api/client/components_lookup.h"
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

static int entity_add_component(lua_State * L) {
	custom::Entity * entity = (custom::Entity *)luaL_checkudata(L, 1, "Entity");
	u32 type = (u32)luaL_checkinteger(L, 2);
	CUSTOM_ASSERT(entity->exists(), "entity doesn't exist");
	entity->add_component(type);
	return 0;
}

static int entity_rem_component(lua_State * L) {
	custom::Entity * entity = (custom::Entity *)luaL_checkudata(L, 1, "Entity");
	u32 type = (u32)luaL_checkinteger(L, 2);
	CUSTOM_ASSERT(entity->exists(), "entity doesn't exist");
	entity->rem_component(type);
	return 0;
}

static int entity_has_component(lua_State * L) {
	custom::Entity * entity = (custom::Entity *)luaL_checkudata(L, 1, "Entity");
	u32 type = (u32)luaL_checkinteger(L, 2);
	CUSTOM_ASSERT(entity->exists(), "entity doesn't exist");
	lua_pushboolean(L, entity->has_component(type));
	return 1;
}

static int entity_get_component(lua_State * L) {
	custom::Entity * entity = (custom::Entity *)luaL_checkudata(L, 1, "Entity");
	u32 type = (u32)luaL_checkinteger(L, 2);
	CUSTOM_ASSERT(entity->exists(), "entity doesn't exist");
	custom::Ref component_ref = entity->get_component(type);
	bool has_component = (*custom::Entity::component_containers[type])(component_ref);
	if (!has_component) { lua_pushnil(L); return 1; }
	custom::Ref * udata = (custom::Ref *)lua_newuserdatauv(L, sizeof(custom::Ref), 0);
	luaL_setmetatable(L, custom::component::names[type]);
	*udata = component_ref;
	return 1;
}

static luaL_Reg const entity_mt[] = {
	{"__index", NULL}, /* place holder */
	{"__tostring", entity_tostring},
	{"__eq", entity_eq},
	{"add_component", entity_add_component},
	{"rem_component", entity_rem_component},
	{"has_component", entity_has_component},
	{"get_component", entity_get_component},
	{NULL, NULL},
};

static int entity_create(lua_State * L) {
	return 0;
}

static int entity_destroy(lua_State * L) {
	return 0;
}

static luaL_Reg const entity_lib[] = {
	{"create", entity_create},
	{"destroy", entity_destroy},
	{NULL, NULL},
};

namespace custom {
namespace lua_entity_system {

void init(lua_State * L) {
	// luaL_newlib(L, entity_lib);
	if (luaL_newmetatable(L, "Entity")) {
		luaL_setfuncs(L, entity_mt, 0);
		lua_setfield(L, -1, "__index");
	}
	else { lua_pop(L, 1); }

	lua_createtable(L, 0, custom::component::count);
	for (u32 i = 0; i < custom::component::count; ++i) {
		lua_pushinteger(L, i);
		lua_setfield(L, -2, custom::component::names[i]);
	}
	lua_setglobal(L, "Component_Type");

	lua_client::init_components(L);
}

}}
