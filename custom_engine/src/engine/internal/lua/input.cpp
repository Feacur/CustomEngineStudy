#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/core/types.h"
#include "engine/debug/log.h"
#include "engine/api/lua.h"
#include "engine/api/internal/application.h"

#include <lua.hpp>

static int Input_get_key(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 1, "expected 1 argument");
	LUA_ASSERT_TYPE(LUA_TNUMBER, 1);

	custom::Key_Code key = (custom::Key_Code)lua_tointeger(L, 1);

	lua_pushboolean(L, custom::application::get_key(key));

	return 1;
}

static int Input_get_key_transition(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 1, "expected 1 argument");
	LUA_ASSERT_TYPE(LUA_TNUMBER, 1);
	LUA_ASSERT_TYPE(LUA_TBOOLEAN, 2);

	custom::Key_Code key = (custom::Key_Code)lua_tointeger(L, 1);
	bool to_state = lua_toboolean(L, 2);

	lua_pushboolean(L, custom::application::get_key_transition(key, to_state));

	return 1;
}

static int Input_get_mouse_key(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 1, "expected 1 argument");
	LUA_ASSERT_TYPE(LUA_TNUMBER, 1);

	custom::Mouse_Code key = (custom::Mouse_Code)lua_tointeger(L, 1);

	lua_pushboolean(L, custom::application::get_mouse_key(key));

	return 1;
}

static int Input_get_mouse_key_transition(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 1, "expected 1 argument");
	LUA_ASSERT_TYPE(LUA_TNUMBER, 1);
	LUA_ASSERT_TYPE(LUA_TBOOLEAN, 2);

	custom::Mouse_Code key = (custom::Mouse_Code)lua_tointeger(L, 1);
	bool to_state = lua_toboolean(L, 2);

	lua_pushboolean(L, custom::application::get_mouse_key_transition(key, to_state));

	return 1;
}

static int Input_get_mouse_pos(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 0, "expected 0 arguments");

	ivec2 const & value = custom::application::get_mouse_pos();

	vec2 * udata = (vec2 *)lua_newuserdatauv(L, sizeof(vec2), 0);
	luaL_setmetatable(L, "vec2");
	*udata = {(r32)value.x, (r32)value.y};

	return 1;
}

static int Input_get_mouse_delta(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 0, "expected 0 arguments");

	ivec2 const & value = custom::application::get_mouse_delta();

	vec2 * udata = (vec2 *)lua_newuserdatauv(L, sizeof(vec2), 0);
	luaL_setmetatable(L, "vec2");
	*udata = {(r32)value.x, (r32)value.y};

	return 1;
}

static int Input_get_mouse_wheel(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 0, "expected 0 arguments");

	vec2 const & value = custom::application::get_mouse_wheel();

	vec2 * udata = (vec2 *)lua_newuserdatauv(L, sizeof(vec2), 0);
	luaL_setmetatable(L, "vec2");
	*udata = value;

	return 1;
}

static luaL_Reg const Input_aux[] = {
	// Lib.###
	{"get_key", Input_get_key},
	{"get_key_transition", Input_get_key_transition},
	{"get_mouse_key", Input_get_mouse_key},
	{"get_mouse_key_transition", Input_get_mouse_key_transition},
	{"get_mouse_pos", Input_get_mouse_pos},
	{"get_mouse_delta", Input_get_mouse_delta},
	{"get_mouse_wheel", Input_get_mouse_wheel},
	{NULL, NULL},
};

//
//
//

namespace custom {
namespace lua {

void init_input(lua_State * L) {
	LUA_AUX_IMPL(Input)

	lua_newtable(L);
	#define KEY_CODE_IMPL(T)                         \
		lua_pushinteger(L, (u8)custom::Key_Code::T); \
		lua_setfield(L, -2, #T);                     \

	#include "engine/registry_impl/key_codes.h"
	lua_setglobal(L, "Key_Code");

	lua_newtable(L);
	#define MOUSE_CODE_IMPL(T)                         \
		lua_pushinteger(L, (u8)custom::Mouse_Code::T); \
		lua_setfield(L, -2, #T);                       \

	#include "engine/registry_impl/mouse_codes.h"
	lua_setglobal(L, "Mouse_Code");
}

}}