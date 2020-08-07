#pragma once

#if !defined(CUSTOM_SHIPPING)
#define LUA_INDEX_ASSERT() do {\
	lua_getfield(L, 1, "__name");\
	CUSTOM_ASSERT(false, "%s '%s' doesn't contain '%s'", luaL_typename(L, 1), lua_tostring(L, -1), id);\
	lua_pop(L, 1);\
} while (0)\

#else
	#define LUA_INDEX_ASSERT() (void)0
#endif

#define LUA_META_IMPL(T)\
	if (luaL_newmetatable(L, #T)) {\
		luaL_setfuncs(L, T##_meta, 0);\
		\
		lua_setglobal(L, #T);\
	}\
	else { lua_pop(L, 1); }\

#define LUA_META_INDEX_SELF_IMPL(T)\
	if (luaL_newmetatable(L, #T)) {\
		luaL_setfuncs(L, T##_meta, 0);\
		\
		lua_pushvalue(L, -1);\
		lua_setfield(L, -2, "__index");\
		\
		lua_setglobal(L, #T);\
	}\
	else { lua_pop(L, 1); }\

#define LUA_META_INDEX_SCOPE_IMPL(T)\
	if (luaL_newmetatable(L, #T)) {\
		luaL_setfuncs(L, T##_meta, 0);\
		\
		luaL_newlib(L, T##_functions);\
		lua_setfield(L, -2, "__index");\
		\
		lua_setglobal(L, #T);\
	}\
	else { lua_pop(L, 1); }\

// @Note: fallback '__index' receives 'T##_functions' as the table
#define LUA_META_INDEX_SCOPE_FALLBACK_IMPL(T)\
	if (luaL_newmetatable(L, #T)) {\
		luaL_setfuncs(L, T##_meta, 0);\
		\
		luaL_newlib(L, T##_functions);\
		if (T##_fallback && C_ARRAY_LENGTH(T##_fallback) > 1) {\
			CUSTOM_TRACE("fb " #T);\
			luaL_newlib(L, T##_fallback);\
			lua_setmetatable(L, -2);\
		}\
		lua_setfield(L, -2, "__index");\
		\
		lua_setglobal(L, #T);\
	}\
	else { lua_pop(L, 1); }\
