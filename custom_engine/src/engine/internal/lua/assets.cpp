#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/api/lua.h"
#include "engine/api/internal/assets.h"
#include "engine/api/internal/entity_system.h"
#include "engine/impl/asset_system.h"

#include <lua.hpp>
// #include <lstate.h>

typedef custom::Assets Assets;
typedef custom::Ref Ref;

typedef custom::Shader_Asset Shader_Asset;
typedef custom::Texture_Asset Texture_Asset;
typedef custom::Mesh_Asset Mesh_Asset;

//
// Shader_Asset
//

static int Shader_Asset_index(lua_State * L) {
	LUA_INDEX_RAWGET_IMPL(Shader_Asset);

	LUA_DECLARE_USERDATA_CONST_REF_FAST(Shader_Asset, object, 1);
	CUSTOM_LUA_ASSERT(object->exists(), "object doesn't exist");

	cstring id = lua_tostring(L, 2);

	// @Optimize?
	// if (strcmp(id, "") == 0) { return 0; }

	LUA_REPORT_INDEX();
	lua_pushnil(L); return 1;
}

static int Shader_Asset_newindex(lua_State * L) {
	LUA_DECLARE_USERDATA_REF_FAST(Shader_Asset, object, 1);
	CUSTOM_LUA_ASSERT(object->exists(), "object doesn't exist");

	cstring id = lua_tostring(L, 2);

	// @Optimize?
	// if (strcmp(id, "") == 0) { return 0; }

	LUA_REPORT_INDEX();
	return 0;
}

static int Shader_Asset_eq(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_DECLARE_USERDATA_CONST_REF_FAST(Shader_Asset, object1, 1);
	LUA_DECLARE_USERDATA_CONST_REF_SAFE(Shader_Asset, object2, 2);

	lua_pushboolean(L, *object1 == *object2);

	return 1;
}

static int Shader_Asset_get_path(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 1, "expected 1 argument");
	LUA_DECLARE_USERDATA_CONST_REF_SAFE(Shader_Asset, object, 1);

	cstring path = Assets::get_path<Shader_Asset>(*object);

	if (!path) { lua_pushnil(L); return 1; }

	lua_pushstring(L, path);

	return 1;
}

static luaL_Reg const Shader_Asset_meta[] = {
	{"__index", Shader_Asset_index},
	{"__newindex", Shader_Asset_newindex},
	{"__eq", Shader_Asset_eq},
	// instance:###
	{"get_path", Shader_Asset_get_path},
	// Type.###
	//
	{NULL, NULL},
};

//
// Texture_Asset
//

static int Texture_Asset_index(lua_State * L) {
	LUA_INDEX_RAWGET_IMPL(Texture_Asset);

	LUA_DECLARE_USERDATA_CONST_REF_FAST(Texture_Asset, object, 1);
	CUSTOM_LUA_ASSERT(object->exists(), "object doesn't exist");

	cstring id = lua_tostring(L, 2);

	// @Optimize?
	// if (strcmp(id, "") == 0) { return 0; }

	LUA_REPORT_INDEX();
	lua_pushnil(L); return 1;
}

static int Texture_Asset_newindex(lua_State * L) {
	LUA_DECLARE_USERDATA_REF_FAST(Texture_Asset, object, 1);
	CUSTOM_LUA_ASSERT(object->exists(), "object doesn't exist");

	cstring id = lua_tostring(L, 2);

	// @Optimize?
	// if (strcmp(id, "") == 0) { return 0; }

	LUA_REPORT_INDEX();
	return 0;
}

static int Texture_Asset_eq(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_DECLARE_USERDATA_CONST_REF_FAST(Texture_Asset, object1, 1);
	LUA_DECLARE_USERDATA_CONST_REF_SAFE(Texture_Asset, object2, 2);

	lua_pushboolean(L, *object1 == *object2);

	return 1;
}

static int Texture_Asset_get_path(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 1, "expected 1 argument");
	LUA_DECLARE_USERDATA_CONST_REF_SAFE(Texture_Asset, object, 1);

	cstring path = Assets::get_path(*object);

	if (!path) { lua_pushnil(L); return 1; }

	lua_pushstring(L, path);

	return 1;
}

static luaL_Reg const Texture_Asset_meta[] = {
	{"__index", Texture_Asset_index},
	{"__newindex", Texture_Asset_newindex},
	{"__eq", Texture_Asset_eq},
	// instance:###
	{"get_path", Texture_Asset_get_path},
	// Type.###
	//
	{NULL, NULL},
};

//
// Mesh_Asset
//

static int Mesh_Asset_index(lua_State * L) {
	LUA_INDEX_RAWGET_IMPL(Mesh_Asset);

	LUA_DECLARE_USERDATA_CONST_REF_FAST(Mesh_Asset, object, 1);
	CUSTOM_LUA_ASSERT(object->exists(), "object doesn't exist");

	cstring id = lua_tostring(L, 2);

	// @Optimize?
	// if (strcmp(id, "") == 0) { return 0; }

	LUA_REPORT_INDEX();
	lua_pushnil(L); return 1;
}

static int Mesh_Asset_newindex(lua_State * L) {
	LUA_DECLARE_USERDATA_REF_FAST(Mesh_Asset, object, 1);
	CUSTOM_LUA_ASSERT(object->exists(), "object doesn't exist");

	cstring id = lua_tostring(L, 2);

	// @Optimize?
	// if (strcmp(id, "") == 0) { return 0; }

	LUA_REPORT_INDEX();
	return 0;
}

static int Mesh_Asset_eq(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_DECLARE_USERDATA_CONST_REF_FAST(Mesh_Asset, object1, 1);
	LUA_DECLARE_USERDATA_CONST_REF_SAFE(Mesh_Asset, object2, 2);

	lua_pushboolean(L, *object1 == *object2);

	return 1;
}

static int Mesh_Asset_get_path(lua_State * L) {
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 1, "expected 1 argument");
	LUA_DECLARE_USERDATA_CONST_REF_SAFE(Mesh_Asset, object, 1);

	cstring path = Assets::get_path(*object);

	if (!path) { lua_pushnil(L); return 1; }

	lua_pushstring(L, path);

	return 1;
}

static luaL_Reg const Mesh_Asset_meta[] = {
	{"__index", Mesh_Asset_index},
	{"__newindex", Mesh_Asset_newindex},
	{"__eq", Mesh_Asset_eq},
	// instance:###
	{"get_path", Mesh_Asset_get_path},
	// Type.###
	//
	{NULL, NULL},
};

namespace custom {
namespace lua {

void init_assets(lua_State * L) {
	#define ASSET_IMPL(T) LUA_META_IMPL(T)
	#include "../assets_registry_impl.h"
}

}}