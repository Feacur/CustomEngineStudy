#include "engine/core/code.h"
#include "engine/core/types.h"
#include "engine/debug/log.h"
#include "engine/api/internal/entity_system.h"
#include "engine/impl/array.h"

#include "../entity_system/component_types.h"

#include <lua.hpp>

namespace {

struct Script_Blob {
	custom::Entity     entity;
	Lua_Script const * script;
};

}

//
//
//

namespace sandbox {

void lua_function(lua_State * L, cstring name) {
	if (lua_getglobal(L, name) != LUA_TFUNCTION) {
		CUSTOM_ERROR("lua: '%s'", lua_tostring(L, -1));
		lua_pop(L, 1);
		return;
	}

	if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
		CUSTOM_ERROR("lua: '%s'", lua_tostring(L, -1));
		lua_pop(L, 1);
		return;
	}
}

static void lua_function(lua_State * L, cstring name, custom::Entity const & entity, r32 dt) {
	if (lua_getglobal(L, name) != LUA_TFUNCTION) {
		CUSTOM_ERROR("lua: '%s'", lua_tostring(L, -1));
		lua_pop(L, 1);
		return;
	}

	custom::Entity * e = (custom::Entity *)lua_newuserdatauv(L, sizeof(custom::Entity), 0);
	luaL_setmetatable(L, "Entity");
	*e = entity;

	lua_pushnumber(L, dt);

	if (lua_pcall(L, 2, 0, 0) != LUA_OK) {
		CUSTOM_ERROR("lua: '%s'", lua_tostring(L, -1));
		lua_pop(L, 1);
		return;
	}
}

void ecs_update_lua(lua_State * L, r32 dt) {
	custom::Array<Script_Blob> scripts(8);
	for (u32 i = 0; i < custom::Entity::state.instances.count; ++i) {
		custom::Entity entity = custom::Entity::state.instances[i];
		if (!entity.exists()) { continue; }

		Lua_Script const * script = entity.get_component<Lua_Script>().get_safe();
		if (!script) { continue; }

		scripts.push({entity, script});
	}

	for (u32 i = 0; i < scripts.count; ++i) {
		if (scripts[i].script->update_string_id == custom::empty_index) { continue; }
		cstring update_name = custom::Entity::get_string(scripts[i].script->update_string_id);
		lua_function(L, update_name, scripts[i].entity, dt);
	}
}

}
