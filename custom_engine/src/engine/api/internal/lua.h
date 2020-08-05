#pragma once
struct lua_State;

#define LUA_CUSTOM_ENTITY_LIBNAME "custom_entity"
int luaopen_custom_entity(lua_State * L);

#define LUA_CUSTOM_MATH_LIBNAME "custom_math"
int luaopen_custom_math(lua_State * L);
