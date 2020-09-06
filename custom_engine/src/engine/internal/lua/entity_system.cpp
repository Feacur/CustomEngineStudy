#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/api/lua.h"
#include "engine/api/internal/entity_system.h"
#include "engine/api/internal/names_lookup.h"

#include <lua.hpp>

// @Todo: reuse userdata?

typedef custom::Entity Entity;
typedef custom::Ref Ref;

static int Entity_index(lua_State * L) {
	LUA_INDEX_RAWGET_IMPL(Entity);

	Entity const * object = (Entity const *)lua_touserdata(L, 1);
	CUSTOM_LUA_ASSERT(object->exists(), "object doesn't exist");

	cstring id = lua_tostring(L, 2);

	// @Optimize?
	// if (strcmp(id, "") == 0) { return 0; }

	LUA_REPORT_INDEX();
	lua_pushnil(L); return 1;
}

static int Entity_newindex(lua_State * L) {
	Entity * object = (Entity *)lua_touserdata(L, 1);
	CUSTOM_LUA_ASSERT(object->exists(), "object doesn't exist");

	cstring id = lua_tostring(L, 2);

	// @Optimize?
	// if (strcmp(id, "") == 0) { return 0; }

	LUA_REPORT_INDEX();
	return 0;
}

static int Entity_eq(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_ASSERT_USERDATA("Entity", 2);

	Entity const * object1 = (Entity const *)lua_touserdata(L, 1);
	Entity const * object2 = (Entity const *)lua_touserdata(L, 2);

	lua_pushboolean(L, *object1 == *object2);

	return 1;
}

static int Entity_add_component(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_ASSERT_USERDATA("Entity", 1);
	LUA_ASSERT_TYPE(LUA_TNUMBER, 2);

	u32 type = (u32)lua_tointeger(L, 2);
	Entity * object = (Entity *)lua_touserdata(L, 1);
	Ref component_ref = object->add_component(type);
	
	Ref * udata = (Ref *)lua_newuserdatauv(L, sizeof(Ref), 0);
	luaL_setmetatable(L, custom::component_names.get_string(type));
	*udata = component_ref;

	return 1;
}

static int Entity_rem_component(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_ASSERT_USERDATA("Entity", 1);
	LUA_ASSERT_TYPE(LUA_TNUMBER, 2);

	Entity * object = (Entity *)lua_touserdata(L, 1);
	u32 type = (u32)lua_tointeger(L, 2);
	object->rem_component(type);

	return 0;
}

static int Entity_has_component(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_ASSERT_USERDATA("Entity", 1);
	LUA_ASSERT_TYPE(LUA_TNUMBER, 2);

	Entity * object = (Entity *)lua_touserdata(L, 1);
	u32 type = (u32)lua_tointeger(L, 2);
	lua_pushboolean(L, object->has_component(type));

	return 1;
}

static int Entity_get_component(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_ASSERT_USERDATA("Entity", 1);
	LUA_ASSERT_TYPE(LUA_TNUMBER, 2);

	Entity * object = (Entity *)lua_touserdata(L, 1);
	u32 type = (u32)lua_tointeger(L, 2);
	Ref component_ref = object->get_component(type);

	bool has_component = (*Entity::vtable.contains[type])(component_ref);
	if (!has_component) { lua_pushnil(L); return 1; }

	Ref * udata = (Ref *)lua_newuserdatauv(L, sizeof(Ref), 0);
	luaL_setmetatable(L, custom::component_names.get_string(type));
	*udata = component_ref;

	return 1;
}

static int Entity_reset_system(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 0, "expected 0 arguments");

	Entity::reset_system();

	return 0;
}

static int Entity_create(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 1, "expected 1 argument");
	LUA_ASSERT_TYPE(LUA_TBOOLEAN, 1);

	bool is_instance = lua_toboolean(L, 1);

	Entity * udata = (Entity *)lua_newuserdatauv(L, sizeof(Entity), 0);
	luaL_setmetatable(L, "Entity");
	*udata = Entity::create(is_instance);

	return 1;
}

static int Entity_exists(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 1, "expected 1 argument");
	LUA_ASSERT_USERDATA("Entity", 1);

	Entity const * object = (Entity const *)lua_touserdata(L, 1);

	lua_pushboolean(L, object->exists());

	return 1;
}

static int Entity_copy(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 1 argument");
	LUA_ASSERT_USERDATA("Entity", 1);
	LUA_ASSERT_TYPE(LUA_TBOOLEAN, 2);

	Entity * object = (Entity *)lua_touserdata(L, 1);
	bool force_instance = lua_toboolean(L, 2);

	Entity * udata = (Entity *)lua_newuserdatauv(L, sizeof(Entity), 0);
	luaL_setmetatable(L, "Entity");
	*udata = object->copy(force_instance);

	return 1;
}

static int Entity_destroy(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 1, "expected 1 argument");
	LUA_ASSERT_USERDATA("Entity", 1);

	Entity * object = (Entity *)lua_touserdata(L, 1);
	object->destroy();

	return 0;
}

static luaL_Reg const Entity_meta[] = {
	{"__index", Entity_index},
	{"__newindex", Entity_newindex},
	{"__eq", Entity_eq},
	// instance:###
	{"exists",  Entity_exists},
	{"copy",    Entity_copy},
	{"destroy", Entity_destroy},
	{"add_component", Entity_add_component},
	{"rem_component", Entity_rem_component},
	{"has_component", Entity_has_component},
	{"get_component", Entity_get_component},
	// Type.###
	{"reset_system", Entity_reset_system},
	{"create", Entity_create},
	//
	{NULL, NULL},
};

//
//
//

namespace custom {
namespace lua {

// @Change: let client call `init` itself?
//          although why potentially discarding engine-side types?
void init_component_types(lua_State * L);
extern void init_client_component_types(lua_State * L);

void init_entity_system(lua_State * L) {
	LUA_META_IMPL(Entity)
	custom::lua::init_component_types(L);
	custom::lua::init_client_component_types(L);
	for (u32 type = 0; type < custom::component_names.get_count(); ++type) {
		lua_getglobal(L, custom::component_names.get_string(type));
		lua_pushinteger(L, type);
		lua_setfield(L, -2, "type");
		lua_pop(L, 1);
	}
}

}}
