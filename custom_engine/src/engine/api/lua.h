#pragma once

#define LUA_META_IMPL(T)\
	if (luaL_newmetatable(L, #T)) {\
		luaL_setfuncs(L, T##_meta, 0);\
		\
		luaL_newlib(L, T##_methods);\
		lua_setfield(L, -2, "__index");\
		\
		luaL_setfuncs(L, T##_lib, 0);\
		lua_setglobal(L, #T);\
	}\
	else { lua_pop(L, 1); }\
