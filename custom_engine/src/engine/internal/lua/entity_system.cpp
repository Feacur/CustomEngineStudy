#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/api/lua.h"
#include "engine/api/internal/entity_system.h"
#include "engine/api/client/components_lookup.h"
#include "engine/api/client/lua.h"

#include <lua.hpp>
// #include <lstate.h>

static int Entity_index(lua_State * L) {
	if (!lua_getmetatable(L, 1)) { lua_pushnil(L); return 1; }

	lua_pushvalue(L, 2);
	int type = lua_rawget(L, -2);
	lua_remove(L, -2);
	if (type != LUA_TNIL) { return 1; }
	// lua_pop(L, 1);

	custom::Entity * object = (custom::Entity *)lua_touserdata(L, 1);
	if (!object->exists()) { lua_pushnil(L); return 1; }

	cstring id = lua_tostring(L, 2);

	LUA_INDEX_ASSERT();
	// lua_pushnil(L);
	return 1;
}

static int Entity_newindex(lua_State * L) {
	custom::Entity * object = (custom::Entity *)lua_touserdata(L, 1);
	if (!object->exists()) { lua_pushnil(L); return 1; }

	cstring id = lua_tostring(L, 2);

	// @Optimize?
	// if (strcmp(id, "") == 0) { return 0; }

	LUA_INDEX_ASSERT();
	return 0;
}

static int Entity_tostring(lua_State * L) {
	CUSTOM_ASSERT(lua_gettop(L) == 1, "expected 1 argument");
	custom::Entity * object = (custom::Entity *)luaL_checkudata(L, 1, "Entity");
	lua_pushfstring(L, "(e %d:%d)", object->id, object->gen);
	return 1;
}

static int Entity_eq(lua_State * L) {
	CUSTOM_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	custom::Entity * object1 = (custom::Entity *)luaL_checkudata(L, 1, "Entity");
	custom::Entity * object2 = (custom::Entity *)luaL_testudata(L, 2, "Entity");
	lua_pushboolean(L, object2 ? ((object1 == object2) || (*object1 == *object2)) : !object1->exists());
	return 1;
}

static int Entity_add_component(lua_State * L) {
	CUSTOM_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	custom::Entity * object = (custom::Entity *)luaL_checkudata(L, 1, "Entity");
	u32 type = (u32)luaL_checkinteger(L, 2);
	CUSTOM_ASSERT(object->exists(), "entity doesn't exist");
	custom::Ref const & component_ref = object->add_component(type);
	
	custom::Ref * udata = (custom::Ref *)lua_newuserdatauv(L, sizeof(custom::Ref), 0);
	luaL_setmetatable(L, custom::component::names[type]);
	*udata = component_ref;
	return 1;
}

static int Entity_rem_component(lua_State * L) {
	CUSTOM_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	custom::Entity * object = (custom::Entity *)luaL_checkudata(L, 1, "Entity");
	u32 type = (u32)luaL_checkinteger(L, 2);
	CUSTOM_ASSERT(object->exists(), "entity doesn't exist");
	object->rem_component(type);

	return 0;
}

static int Entity_has_component(lua_State * L) {
	CUSTOM_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	custom::Entity * object = (custom::Entity *)luaL_checkudata(L, 1, "Entity");
	u32 type = (u32)luaL_checkinteger(L, 2);
	CUSTOM_ASSERT(object->exists(), "entity doesn't exist");

	lua_pushboolean(L, object->has_component(type));
	return 1;
}

static int Entity_get_component(lua_State * L) {
	CUSTOM_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	custom::Entity * object = (custom::Entity *)luaL_checkudata(L, 1, "Entity");
	u32 type = (u32)luaL_checkinteger(L, 2);
	CUSTOM_ASSERT(object->exists(), "entity doesn't exist");
	custom::Ref component_ref = object->get_component(type);

	bool has_component = (*custom::Entity::component_containers[type])(component_ref);
	if (!has_component) { lua_pushnil(L); return 1; }

	custom::Ref * udata = (custom::Ref *)lua_newuserdatauv(L, sizeof(custom::Ref), 0);
	luaL_setmetatable(L, custom::component::names[type]);
	
	*udata = component_ref;
	return 1;
}

static int Entity_create(lua_State * L) {
	CUSTOM_ASSERT(lua_gettop(L) == 0, "expected 0 arguments");
	custom::Entity * udata = (custom::Entity *)lua_newuserdatauv(L, sizeof(custom::Entity), 0);
	luaL_setmetatable(L, "Entity");
	*udata = custom::Entity::create();
	return 1;
}

static int Entity_destroy(lua_State * L) {
	CUSTOM_ASSERT(lua_gettop(L) == 1, "expected 1 argument");
	custom::Entity * object = (custom::Entity *)luaL_checkudata(L, 1, "Entity");
	custom::Entity::destroy(*object);
	return 0;
}

static luaL_Reg const Entity_meta[] = {
	{"__index", Entity_index},
	{"__newindex", Entity_newindex},
	{"__tostring", Entity_tostring},
	{"__eq", Entity_eq},
	// instance:###
	{"add_component", Entity_add_component},
	{"rem_component", Entity_rem_component},
	{"has_component", Entity_has_component},
	{"get_component", Entity_get_component},
	// Type.###
	{"create", Entity_create},
	{"destroy", Entity_destroy},
	//
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