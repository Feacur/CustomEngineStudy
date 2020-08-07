#pragma once

// @Note: fallback for shipping mode errors
#if defined(CUSTOM_SHIPPING)
	// #define CUSTOM_LUA_BREAK() (void)0
	#define CUSTOM_LUA_BREAK() lua_error(L)
#endif

// @Note: undef if don't want any error checks
#if defined(CUSTOM_LUA_BREAK)
	#define CUSTOM_LUA_ASSERT(statement, ...) CUSTOM_CONDITIONAL(!(statement), CUSTOM_LUA_BREAK(), __VA_ARGS__)
#else
	#define CUSTOM_LUA_ASSERT(statement, ...) CUSTOM_ASSERT(statement, __VA_ARGS__)
#endif

#if defined(CUSTOM_LUA_ASSERT)

#define LUA_REPORT_INDEX() do {\
	lua_getfield(L, 1, "__name");\
	CUSTOM_LUA_ASSERT(false, "%s '%s' doesn't contain '%s'", luaL_typename(L, 1), lua_tostring(L, -1), id);\
	lua_pop(L, 1);\
} while (0)\

#define LUA_ASSERT_USERDATA(index, type) do {\
	CUSTOM_LUA_ASSERT(lua_type(L, index) == LUA_TUSERDATA, "expected a userdata at index %d", index);\
	CUSTOM_LUA_ASSERT(lua_getmetatable(L, index), "expected a userdata with a metatable at index %d", index);\
	CUSTOM_LUA_ASSERT(luaL_getmetatable(L, type) == LUA_TTABLE, "metatable '%s' doesn't exist", type);\
	CUSTOM_LUA_ASSERT(lua_rawequal(L, -1, -2), "userdata at index %d is not '%s'", index, type);\
	lua_pop(L, 2);\
\
} while (0)\

#else
	#define LUA_REPORT_INDEX() (void)0
	#define LUA_ASSERT_USERDATA(index, type) (void)0
#endif

// @Note: metatable contains metamethods, instance functions, static functions;
//        also it becomes a global value
#define LUA_META_IMPL(T)\
	if (luaL_newmetatable(L, #T)) {\
		luaL_setfuncs(L, T##_meta, 0);\
		\
		lua_setglobal(L, #T);\
	}\
	else { lua_pop(L, 1); }\

// @Note: seek for a value in the corresponding metatable first,
//        then pass execution further to the rest of the index method
#define LUA_INDEX_RAWGET_IMPL(T)\
	LUA_ASSERT_USERDATA(1, #T);\
	if (!lua_getmetatable(L, 1)) { lua_pushnil(L); return 1; }\
	\
	lua_pushvalue(L, 2);\
	if (lua_rawget(L, -2) != LUA_TNIL) { lua_remove(L, -2); return 1; }\
	lua_rotate(L, -3, -2); lua_pop(L, 2);\

/*
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
*/
