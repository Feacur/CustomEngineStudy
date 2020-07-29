#include "ecs_lua_runner.h"

#include "engine/core/code.h"
#include "engine/core/types.h"
#include "engine/debug/log.h"
#include "engine/api/internal/loader.h"
#include "engine/impl/array.h"

#include "components.h"
#include "../assets/ids.h"

#include <lua.hpp>

namespace sandbox {
namespace ecs_lua_runner {

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

void update(lua_State * L) {
	// @Todo: prefetch all relevant components into a contiguous array?
	for (u32 i = 0; i < custom::Entity::instances.count; ++i) {
		custom::Entity entity = custom::Entity::instances[i];
		if (!entity.exists()) { continue; }

		Lua_Script * lua_script = entity.get_component<Lua_Script>().get_safe();
		if (!lua_script) { continue; }
		if (!lua_script->function) { continue; }
		lua_function(L, lua_script->function);
	}
}

}}
