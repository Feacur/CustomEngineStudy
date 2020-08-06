#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/api/lua.h"
#include "engine/api/internal/entity_system.h"
#include "engine/api/client/components_lookup.h"
#include "engine/api/client/lua.h"

#include <lua.hpp>
// #include <lstate.h>

static int entity_tostring(lua_State * L) {
	CUSTOM_ASSERT(lua_gettop(L) == 1, "expected 1 argument");
	custom::Entity * entity = (custom::Entity *)luaL_checkudata(L, 1, "Entity");
	lua_pushfstring(L, "(e %d:%d)", entity->id, entity->gen);
	return 1;
}

static int entity_eq(lua_State * L) {
	CUSTOM_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	custom::Entity * entity1 = (custom::Entity *)luaL_checkudata(L, 1, "Entity");
	custom::Entity * entity2 = (custom::Entity *)luaL_testudata(L, 2, "Entity");
	lua_pushboolean(L, entity2 ? ((entity1 == entity2) || (*entity1 == *entity2)) : !entity1->exists());
	return 1;
}

static luaL_Reg const Entity_meta[] = {
	{"__index", NULL}, /* place holder */
	{"__tostring", entity_tostring},
	{"__eq", entity_eq},
	{NULL, NULL},
};

static int entity_add_component(lua_State * L) {
	CUSTOM_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	custom::Entity * entity = (custom::Entity *)luaL_checkudata(L, 1, "Entity");
	u32 type = (u32)luaL_checkinteger(L, 2);
	CUSTOM_ASSERT(entity->exists(), "entity doesn't exist");
	custom::Ref const & component_ref = entity->add_component(type);
	
	custom::Ref * udata = (custom::Ref *)lua_newuserdatauv(L, sizeof(custom::Ref), 0);
	luaL_setmetatable(L, custom::component::names[type]);
	*udata = component_ref;
	return 1;
}

static int entity_rem_component(lua_State * L) {
	CUSTOM_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	custom::Entity * entity = (custom::Entity *)luaL_checkudata(L, 1, "Entity");
	u32 type = (u32)luaL_checkinteger(L, 2);
	CUSTOM_ASSERT(entity->exists(), "entity doesn't exist");
	entity->rem_component(type);

	return 0;
}

static int entity_has_component(lua_State * L) {
	CUSTOM_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	custom::Entity * entity = (custom::Entity *)luaL_checkudata(L, 1, "Entity");
	u32 type = (u32)luaL_checkinteger(L, 2);
	CUSTOM_ASSERT(entity->exists(), "entity doesn't exist");

	lua_pushboolean(L, entity->has_component(type));
	return 1;
}

static int entity_get_component(lua_State * L) {
	CUSTOM_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
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

static luaL_Reg const Entity_methods[] = {
	{"add_component", entity_add_component},
	{"rem_component", entity_rem_component},
	{"has_component", entity_has_component},
	{"get_component", entity_get_component},
	{NULL, NULL},
};

static int entity_create(lua_State * L) {
	CUSTOM_ASSERT(lua_gettop(L) == 0, "expected 0 arguments");
	custom::Entity * udata = (custom::Entity *)lua_newuserdatauv(L, sizeof(custom::Entity), 0);
	luaL_setmetatable(L, "Entity");
	*udata = custom::Entity::create();
	return 1;
}

static int entity_destroy(lua_State * L) {
	CUSTOM_ASSERT(lua_gettop(L) == 1, "expected 1 argument");
	custom::Entity * entity = (custom::Entity *)luaL_checkudata(L, 1, "Entity");
	custom::Entity::destroy(*entity);
	return 0;
}

static luaL_Reg const Entity_lib[] = {
	{"create", entity_create},
	{"destroy", entity_destroy},
	{NULL, NULL},
};

namespace custom {
namespace lua {

void init_entity_system(lua_State * L) {
	LUA_META_IMPL(Entity)
	custom::lua_client::init_components(L);
	for (u32 i = 0; i < custom::component::count; ++i) {
		lua_getglobal(L, custom::component::names[i]);
		lua_pushinteger(L, i);
		lua_setfield(L, -2, "type");
		lua_pop(L, 1);
	}
}

}}