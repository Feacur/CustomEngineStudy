#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/api/lua.h"
#include "engine/api/internal/asset_types.h"
#include "engine/api/internal/names_lookup.h"
#include "engine/impl/asset_system.h"

#include <lua.hpp>

// @Todo: reuse userdata?

typedef custom::Shader_Asset Shader_Asset;
typedef custom::Texture_Asset Texture_Asset;
typedef custom::Mesh_Asset Mesh_Asset;
typedef custom::Prefab_Asset Prefab_Asset;

//
// Shader_Asset
//

static int Shader_Asset_index(lua_State * L) {
	typedef custom::Asset_RefT<custom::Shader_Asset> Asset_Ref;

	LUA_INDEX_RAWGET_IMPL(Shader_Asset);

	Asset_Ref const * object = (Asset_Ref const *)lua_touserdata(L, 1);
	CUSTOM_LUA_ASSERT(object->exists(), "object doesn't exist");

	cstring id = lua_tostring(L, 2);

	// @Optimize?
	// if (strcmp(id, "") == 0) { return 0; }

	LUA_REPORT_INDEX();
	lua_pushnil(L); return 1;
}

static int Shader_Asset_newindex(lua_State * L) {
	typedef custom::Asset_RefT<custom::Shader_Asset> Asset_Ref;

	Asset_Ref * object = (Asset_Ref *)lua_touserdata(L, 1);
	CUSTOM_LUA_ASSERT(object->exists(), "object doesn't exist");

	cstring id = lua_tostring(L, 2);

	// @Optimize?
	// if (strcmp(id, "") == 0) { return 0; }

	LUA_REPORT_INDEX();
	return 0;
}

static int Shader_Asset_eq(lua_State * L) {
	typedef custom::Asset_RefT<custom::Shader_Asset> Asset_Ref;

	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_ASSERT_USERDATA("Shader_Asset", 2);

	Asset_Ref const * object1 = (Asset_Ref const *)lua_touserdata(L, 1);
	Asset_Ref const * object2 = (Asset_Ref const *)lua_touserdata(L, 2);

	lua_pushboolean(L, object1->ref == object2->ref);

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
	typedef custom::Asset_RefT<custom::Texture_Asset> Asset_Ref;

	LUA_INDEX_RAWGET_IMPL(Texture_Asset);

	Asset_Ref const * object = (Asset_Ref const *)lua_touserdata(L, 1);
	CUSTOM_LUA_ASSERT(object->exists(), "object doesn't exist");

	cstring id = lua_tostring(L, 2);

	// @Optimize?
	// if (strcmp(id, "") == 0) { return 0; }

	LUA_REPORT_INDEX();
	lua_pushnil(L); return 1;
}

static int Texture_Asset_newindex(lua_State * L) {
	typedef custom::Asset_RefT<custom::Texture_Asset> Asset_Ref;

	Asset_Ref * object = (Asset_Ref *)lua_touserdata(L, 1);
	CUSTOM_LUA_ASSERT(object->exists(), "object doesn't exist");

	cstring id = lua_tostring(L, 2);

	// @Optimize?
	// if (strcmp(id, "") == 0) { return 0; }

	LUA_REPORT_INDEX();
	return 0;
}

static int Texture_Asset_eq(lua_State * L) {
	typedef custom::Asset_RefT<custom::Texture_Asset> Asset_Ref;

	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_ASSERT_USERDATA("Texture_Asset", 2);

	Asset_Ref const * object1 = (Asset_Ref const *)lua_touserdata(L, 1);
	Asset_Ref const * object2 = (Asset_Ref const *)lua_touserdata(L, 2);

	lua_pushboolean(L, object1->ref == object2->ref);

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
	typedef custom::Asset_RefT<custom::Mesh_Asset> Asset_Ref;

	LUA_INDEX_RAWGET_IMPL(Mesh_Asset);

	Asset_Ref const * object = (Asset_Ref const *)lua_touserdata(L, 1);
	CUSTOM_LUA_ASSERT(object->exists(), "object doesn't exist");

	cstring id = lua_tostring(L, 2);

	// @Optimize?
	// if (strcmp(id, "") == 0) { return 0; }

	LUA_REPORT_INDEX();
	lua_pushnil(L); return 1;
}

static int Mesh_Asset_newindex(lua_State * L) {
	typedef custom::Asset_RefT<custom::Mesh_Asset> Asset_Ref;

	Asset_Ref * object = (Asset_Ref *)lua_touserdata(L, 1);
	CUSTOM_LUA_ASSERT(object->exists(), "object doesn't exist");

	cstring id = lua_tostring(L, 2);

	// @Optimize?
	// if (strcmp(id, "") == 0) { return 0; }

	LUA_REPORT_INDEX();
	return 0;
}

static int Mesh_Asset_eq(lua_State * L) {
	typedef custom::Asset_RefT<custom::Mesh_Asset> Asset_Ref;

	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_ASSERT_USERDATA("Mesh_Asset", 2);

	Asset_Ref const * object1 = (Asset_Ref const *)lua_touserdata(L, 1);
	Asset_Ref const * object2 = (Asset_Ref const *)lua_touserdata(L, 2);

	lua_pushboolean(L, object1->ref == object2->ref);

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
// Prefab_Asset
//

static int Prefab_Asset_index(lua_State * L) {
	typedef custom::Asset_RefT<Prefab_Asset> Asset_Ref;

	LUA_INDEX_RAWGET_IMPL(Prefab_Asset);

	Asset_Ref const * object = (Asset_Ref const *)lua_touserdata(L, 1);
	CUSTOM_LUA_ASSERT(object->exists(), "object doesn't exist");

	cstring id = lua_tostring(L, 2);

	// @Optimize?
	// if (strcmp(id, "") == 0) { return 0; }

	LUA_REPORT_INDEX();
	lua_pushnil(L); return 1;
}

static int Prefab_Asset_newindex(lua_State * L) {
	typedef custom::Asset_RefT<Prefab_Asset> Asset_Ref;

	Asset_Ref * object = (Asset_Ref *)lua_touserdata(L, 1);
	CUSTOM_LUA_ASSERT(object->exists(), "object doesn't exist");

	cstring id = lua_tostring(L, 2);

	// @Optimize?
	// if (strcmp(id, "") == 0) { return 0; }

	LUA_REPORT_INDEX();
	return 0;
}

static int Prefab_Asset_eq(lua_State * L) {
	typedef custom::Asset_RefT<Prefab_Asset> Asset_Ref;

	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_ASSERT_USERDATA("Prefab_Asset", 2);

	Asset_Ref const * object1 = (Asset_Ref const *)lua_touserdata(L, 1);
	Asset_Ref const * object2 = (Asset_Ref const *)lua_touserdata(L, 2);

	lua_pushboolean(L, object1->ref == object2->ref);

	return 1;
}

static int Prefab_Asset_instantiate(lua_State * L) {
	typedef custom::Asset_RefT<Prefab_Asset> Asset_Ref;
	
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 1, "expected 1 argument");
	LUA_ASSERT_USERDATA("Prefab_Asset", 1);

	Asset_Ref const * object = (Asset_Ref const *)lua_touserdata(L, 1);
	CUSTOM_LUA_ASSERT(object->exists(), "object doesn't exist");

	custom::Prefab_Asset const * prefab = object->ref.get_fast();
	CUSTOM_LUA_ASSERT(prefab->exists(), "prefab doesn't exist");

	// @Note: can potentially reallocate memory; ping ref pool once more afterwards
	custom::Entity instance = prefab->copy(true);
	// prefab = object->ref.get_fast();

	custom::Entity * udata = (custom::Entity *)lua_newuserdatauv(L, sizeof(custom::Entity), 0);
	luaL_setmetatable(L, "Entity");
	*udata = instance;

	return 1;
}

static int Prefab_Asset_promote_to_instance(lua_State * L) {
	typedef custom::Asset_RefT<Prefab_Asset> Asset_Ref;
	
	CUSTOM_LUA_ASSERT(lua_gettop(L) == 1, "expected 1 argument");
	LUA_ASSERT_USERDATA("Prefab_Asset", 1);

	Asset_Ref * object = (Asset_Ref *)lua_touserdata(L, 1);
	CUSTOM_LUA_ASSERT(object->exists(), "object doesn't exist");

	custom::Prefab_Asset * prefab = object->ref.get_fast();
	CUSTOM_LUA_ASSERT(prefab->exists(), "prefab doesn't exist");

	custom::Entity instance = *prefab;
	instance.promote_to_instance();

	custom::Entity * udata = (custom::Entity *)lua_newuserdatauv(L, sizeof(custom::Entity), 0);
	luaL_setmetatable(L, "Entity");
	*udata = instance;

	return 1;
}

static luaL_Reg const Prefab_Asset_meta[] = {
	{"__index", Prefab_Asset_index},
	{"__newindex", Prefab_Asset_newindex},
	{"__eq", Prefab_Asset_eq},
	// instance:###
	{"instantiate", Prefab_Asset_instantiate},
	{"promote_to_instance", Prefab_Asset_promote_to_instance},
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
	#include "engine/registry_impl/asset_types.h"
}

}}