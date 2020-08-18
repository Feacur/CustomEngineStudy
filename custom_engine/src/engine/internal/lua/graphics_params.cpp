#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/core/types.h"
#include "engine/debug/log.h"
#include "engine/api/graphics_params.h"
#include "engine/api/lua.h"

#include <lua.hpp>

namespace custom {
namespace lua {

void init_graphics_params(lua_State * L) {
	lua_newtable(L);
	#define CLEAR_FLAG_IMPL(T, i)\
		lua_pushinteger(L, (u8)graphics::Clear_Flag::T);\
		lua_setfield(L, -2, #T);\

	#include "engine/registry_impl/clear_flag.h"
	lua_setglobal(L, "Clear_Flag");
}

}}