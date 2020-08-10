#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/api/lua.h"
#include "engine/api/internal/entity_system.h"
#include "engine/api/client/components_lookup.h"
#include "engine/api/client/lua.h"

#include <lua.hpp>
// #include <lstate.h>

typedef custom::Entity Entity;

static int Entity_index(lua_State * L) {
	LUA_INDEX_RAWGET_IMPL(Entity);

	LUA_DECLARE_USERDATA_CONST_FAST(Entity, object, 1);
	CUSTOM_LUA_ASSERT(object->exists(), "object doesn't exist");

	cstring id = lua_tostring(L, 2);

	LUA_REPORT_INDEX();
	lua_pushnil(L); return 1;
}

static int Entity_newindex(lua_State * L) {
	LUA_DECLARE_USERDATA_FAST(Entity, object, 1);
	CUSTOM_LUA_ASSERT(object->exists(), "object doesn't exist");

	cstring id = lua_tostring(L, 2);

	// @Optimize?
	// if (strcmp(id, "") == 0) { return 0; }

	LUA_REPORT_INDEX();
	return 0;
}

static int Entity_eq(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_DECLARE_USERDATA_CONST_FAST(Entity, object1, 1);
	LUA_DECLARE_USERDATA_CONST_SAFE(Entity, object2, 2);

	lua_pushboolean(L, *object1 == *object2);

	return 1;
}

static int Entity_add_component(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");

	LUA_DECLARE_USERDATA_SAFE(Entity, object, 1);

	LUA_ASSERT_TYPE(LUA_TNUMBER, 2);
	u32 type = (u32)lua_tointeger(L, 2);
	custom::Ref const & component_ref = object->add_component(type);
	
	custom::Ref * udata = (custom::Ref *)lua_newuserdatauv(L, sizeof(custom::Ref), 0);
	luaL_setmetatable(L, custom::component::names[type]);
	*udata = component_ref;

	return 1;
}

static int Entity_rem_component(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");

	LUA_DECLARE_USERDATA_SAFE(Entity, object, 1);

	LUA_ASSERT_TYPE(LUA_TNUMBER, 2);
	u32 type = (u32)lua_tointeger(L, 2);
	object->rem_component(type);

	return 0;
}

static int Entity_has_component(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");

	LUA_DECLARE_USERDATA_CONST_SAFE(Entity, object, 1);

	LUA_ASSERT_TYPE(LUA_TNUMBER, 2);
	u32 type = (u32)lua_tointeger(L, 2);
	lua_pushboolean(L, object->has_component(type));

	return 1;
}

static int Entity_get_component(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");

	LUA_DECLARE_USERDATA_CONST_SAFE(Entity, object, 1);

	LUA_ASSERT_TYPE(LUA_TNUMBER, 2);
	u32 type = (u32)lua_tointeger(L, 2);
	custom::Ref component_ref = object->get_component(type);

	bool has_component = (*custom::Entity::component_containers[type])(component_ref);
	if (!has_component) { lua_pushnil(L); return 1; }

	custom::Ref * udata = (custom::Ref *)lua_newuserdatauv(L, sizeof(custom::Ref), 0);
	luaL_setmetatable(L, custom::component::names[type]);
	*udata = component_ref;

	return 1;
}

static int Entity_create(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 0, "expected 0 arguments");

	custom::Entity * udata = (custom::Entity *)lua_newuserdatauv(L, sizeof(custom::Entity), 0);
	luaL_setmetatable(L, "Entity");
	*udata = custom::Entity::create();

	return 1;
}

static int Entity_destroy(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 1, "expected 1 argument");

	LUA_DECLARE_USERDATA_CONST_SAFE(Entity, object, 1);
	custom::Entity::destroy(*object);

	return 0;
}

static luaL_Reg const Entity_meta[] = {
	{"__index", Entity_index},
	{"__newindex", Entity_newindex},
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