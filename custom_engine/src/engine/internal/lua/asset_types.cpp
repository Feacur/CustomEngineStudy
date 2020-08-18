#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/api/lua.h"
#include "engine/api/internal/asset_types.h"
#include "engine/api/internal/types_names_lookup.h"
#include "engine/impl/asset_system.h"

#include <lua.hpp>

// @Todo: reuse userdata?

typedef custom::Shader_Asset Shader_Asset;
typedef custom::Texture_Asset Texture_Asset;
typedef custom::Mesh_Asset Mesh_Asset;

//
// Shader_Asset
//

static int Shader_Asset_index(lua_State * L) {
	typedef custom::RefT<custom::Shader_Asset> Ref;

	LUA_INDEX_RAWGET_IMPL(Shader_Asset);

	Ref const * object = (Ref const *)lua_touserdata(L, 1);
	CUSTOM_LUA_ASSERT(object->exists(), "object doesn't exist");

	cstring id = lua_tostring(L, 2);

	// @Optimize?
	// if (strcmp(id, "") == 0) { return 0; }

	LUA_REPORT_INDEX();
	lua_pushnil(L); return 1;
}

static int Shader_Asset_newindex(lua_State * L) {
	typedef custom::RefT<custom::Shader_Asset> Ref;

	Ref * object = (Ref *)lua_touserdata(L, 1);
	CUSTOM_LUA_ASSERT(object->exists(), "object doesn't exist");

	cstring id = lua_tostring(L, 2);

	// @Optimize?
	// if (strcmp(id, "") == 0) { return 0; }

	LUA_REPORT_INDEX();
	return 0;
}

static int Shader_Asset_eq(lua_State * L) {
	typedef custom::RefT<custom::Shader_Asset> Ref;

	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_ASSERT_USERDATA("Shader_Asset", 2);

	Ref const * object1 = (Ref const *)lua_touserdata(L, 1);
	Ref const * object2 = (Ref const *)lua_touserdata(L, 2);

	lua_pushboolean(L, *object1 == *object2);

	return 1;
}

static luaL_Reg const Shader_Asset_meta[] = {
	{"__index", Shader_Asset_index},
	{"__newindex", Shader_Asset_newindex},
	{"__eq", Shader_Asset_eq},
	// instance:###
	// Type.###
	//
	{NULL, NULL},
};

//
// Texture_Asset
//

static int Texture_Asset_index(lua_State * L) {
	typedef custom::RefT<custom::Texture_Asset> Ref;

	LUA_INDEX_RAWGET_IMPL(Texture_Asset);

	Ref const * object = (Ref const *)lua_touserdata(L, 1);
	CUSTOM_LUA_ASSERT(object->exists(), "object doesn't exist");

	cstring id = lua_tostring(L, 2);

	// @Optimize?
	// if (strcmp(id, "") == 0) { return 0; }

	LUA_REPORT_INDEX();
	lua_pushnil(L); return 1;
}

static int Texture_Asset_newindex(lua_State * L) {
	typedef custom::RefT<custom::Texture_Asset> Ref;

	Ref * object = (Ref *)lua_touserdata(L, 1);
	CUSTOM_LUA_ASSERT(object->exists(), "object doesn't exist");

	cstring id = lua_tostring(L, 2);

	// @Optimize?
	// if (strcmp(id, "") == 0) { return 0; }

	LUA_REPORT_INDEX();
	return 0;
}

static int Texture_Asset_eq(lua_State * L) {
	typedef custom::RefT<custom::Texture_Asset> Ref;

	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_ASSERT_USERDATA("Texture_Asset", 2);

	Ref const * object1 = (Ref const *)lua_touserdata(L, 1);
	Ref const * object2 = (Ref const *)lua_touserdata(L, 2);

	lua_pushboolean(L, *object1 == *object2);

	return 1;
}

static luaL_Reg const Texture_Asset_meta[] = {
	{"__index", Texture_Asset_index},
	{"__newindex", Texture_Asset_newindex},
	{"__eq", Texture_Asset_eq},
	// instance:###
	// Type.###
	//
	{NULL, NULL},
};

//
// Mesh_Asset
//

static int Mesh_Asset_index(lua_State * L) {
	typedef custom::RefT<custom::Mesh_Asset> Ref;

	LUA_INDEX_RAWGET_IMPL(Mesh_Asset);

	Ref const * object = (Ref const *)lua_touserdata(L, 1);
	CUSTOM_LUA_ASSERT(object->exists(), "object doesn't exist");

	cstring id = lua_tostring(L, 2);

	// @Optimize?
	// if (strcmp(id, "") == 0) { return 0; }

	LUA_REPORT_INDEX();
	lua_pushnil(L); return 1;
}

static int Mesh_Asset_newindex(lua_State * L) {
	typedef custom::RefT<custom::Mesh_Asset> Ref;

	Ref * object = (Ref *)lua_touserdata(L, 1);
	CUSTOM_LUA_ASSERT(object->exists(), "object doesn't exist");

	cstring id = lua_tostring(L, 2);

	// @Optimize?
	// if (strcmp(id, "") == 0) { return 0; }

	LUA_REPORT_INDEX();
	return 0;
}

static int Mesh_Asset_eq(lua_State * L) {
	typedef custom::RefT<custom::Mesh_Asset> Ref;

	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_ASSERT_USERDATA("Mesh_Asset", 2);

	Ref const * object1 = (Ref const *)lua_touserdata(L, 1);
	Ref const * object2 = (Ref const *)lua_touserdata(L, 2);

	lua_pushboolean(L, *object1 == *object2);

	return 1;
}

static luaL_Reg const Mesh_Asset_meta[] = {
	{"__index", Mesh_Asset_index},
	{"__newindex", Mesh_Asset_newindex},
	{"__eq", Mesh_Asset_eq},
	// instance:###
	// Type.###
	//
	{NULL, NULL},
};

//
//
//

namespace custom {
namespace lua {

void init_asset_types(lua_State * L) {
	#define ASSET_IMPL(T) LUA_META_IMPL(T)
	#include "../assets_registry_impl.h"
}

}}