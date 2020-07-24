#include "ecs_lua_runner.h"

#include "engine/core/code.h"
#include "engine/core/types.h"
#include "engine/debug/log.h"

#include "components.h"

#include <lua.hpp>
#include <lstate.h>

namespace sandbox {
namespace ecs_lua_runner {

static lua_State * L;

void init() {
	L = luaL_newstate();

	// luaL_openlibs(L);
	luaL_requiref(L, LUA_GNAME, luaopen_base, 1); lua_pop(L, 1);
	luaL_requiref(L, LUA_STRLIBNAME, luaopen_string, 1); lua_pop(L, 1);
}

void process() {
	// @Todo: prefetch all relevant components into a contiguous array?
	for (u32 i = 0; i < custom::Entity::instances.count; ++i) {
		custom::Entity entity = custom::Entity::instances[i];
		if (!entity.exists()) { continue; }

		Lua_Script * lua_script = entity.get_component<Lua_Script>().get_safe();
		if (!lua_script) { continue; }

		//

		if (!lua_script->loaded) {
			lua_script->loaded = true;
			if (luaL_dofile(L, lua_script->file_path) != LUA_OK) {
				CUSTOM_ERROR("lua error: '%s'", lua_tostring(L, -1));
				lua_pop(L, 1);
				continue;
			}
			CUSTOM_TRACE("loaded %s", lua_script->file_path);
		}

		if (lua_getglobal(L, "ecs_update") != LUA_TFUNCTION) {
			CUSTOM_ERROR("lua error: '%s'", lua_tostring(L, -1));
			lua_pop(L, 1);
		}
		else if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
			CUSTOM_ERROR("lua error: '%s'", lua_tostring(L, -1));
			lua_pop(L, 1);
		}
	}
}

}}
