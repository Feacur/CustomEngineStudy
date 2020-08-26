#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/api/lua.h"
#include "engine/impl/asset_system.h"

#include "engine/api/internal/entity_system.h"

#include "../entity_system/component_types.h"

#include <lua.hpp>

// @Todo: reuse userdata?

//
// Lua_Script
//

static int Lua_Script_index(lua_State * L) {
	typedef custom::RefT<Lua_Script> Ref;

	LUA_INDEX_RAWGET_IMPL(Lua_Script);

	Ref const * object = (Ref const *)lua_touserdata(L, 1);
	CUSTOM_LUA_ASSERT(object->exists(), "object doesn't exist");

	cstring id = lua_tostring(L, 2);

	// @Optimize?
	if (strcmp(id, "update") == 0) {
		cstring value = custom::Entity::get_string(object->get_fast()->update_string_id);
		lua_pushstring(L, value); return 1;
	}

	LUA_REPORT_INDEX();
	lua_pushnil(L); return 1;
}

static int Lua_Script_newindex(lua_State * L) {
	typedef custom::RefT<Lua_Script> Ref;

	Ref * object = (Ref *)lua_touserdata(L, 1);
	CUSTOM_LUA_ASSERT(object->exists(), "object doesn't exist");

	cstring id = lua_tostring(L, 2);

	// @Optimize?
	if (strcmp(id, "update") == 0) {
		LUA_ASSERT_TYPE(LUA_TSTRING, 3);
		cstring value = lua_tostring(L, 3);
		object->get_fast()->update_string_id = custom::Entity::store_string(value, custom::empty_index);
		return 0;
	}

	LUA_REPORT_INDEX();
	return 0;
}

static int Lua_Script_eq(lua_State * L) {
	typedef custom::RefT<Lua_Script> Ref;

	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_ASSERT_USERDATA("Lua_Script", 2);

	Ref const * object1 = (Ref const *)lua_touserdata(L, 1);
	Ref const * object2 = (Ref const *)lua_touserdata(L, 2);

	lua_pushboolean(L, *object1 == *object2);

	return 1;
}

static luaL_Reg const Lua_Script_meta[] = {
	{"__index", Lua_Script_index},
	{"__newindex", Lua_Script_newindex},
	{"__eq", Lua_Script_eq},
	//
	{NULL, NULL},
};

//
// Visual
//

static int Visual_index(lua_State * L) {
	typedef custom::RefT<Visual> Ref;

	LUA_INDEX_RAWGET_IMPL(Visual);

	Ref const * object = (Ref const *)lua_touserdata(L, 1);
	CUSTOM_LUA_ASSERT(object->exists(), "object doesn't exist");

	cstring id = lua_tostring(L, 2);

	// @Optimize?
	if (strcmp(id, "shader") == 0) {
		typedef custom::RefT<custom::Shader_Asset> Asset_Ref;
		Asset_Ref * udata = (Asset_Ref *)lua_newuserdatauv(L, sizeof(Asset_Ref), 0);
		luaL_setmetatable(L, "Shader_Asset");
		*udata = object->get_fast()->shader;
		return 1;
	}
	
	if (strcmp(id, "texture") == 0) {
		typedef custom::RefT<custom::Texture_Asset> Asset_Ref;
		Asset_Ref * udata = (Asset_Ref *)lua_newuserdatauv(L, sizeof(Asset_Ref), 0);
		luaL_setmetatable(L, "Texture_Asset");
		*udata = object->get_fast()->texture;
		return 1;
	}
	
	if (strcmp(id, "mesh") == 0) {
		typedef custom::RefT<custom::Mesh_Asset> Asset_Ref;
		Asset_Ref * udata = (Asset_Ref *)lua_newuserdatauv(L, sizeof(Asset_Ref), 0);
		luaL_setmetatable(L, "Mesh_Asset");
		*udata = object->get_fast()->mesh;
		return 1;
	}
	
	if (strcmp(id, "layer") == 0) {
		lua_pushinteger(L, object->get_fast()->layer);
		return 1;
	}

	LUA_REPORT_INDEX();
	lua_pushnil(L); return 1;
}

static int Visual_newindex(lua_State * L) {
	typedef custom::RefT<Visual> Ref;

	Ref * object = (Ref *)lua_touserdata(L, 1);
	CUSTOM_LUA_ASSERT(object->exists(), "object doesn't exist");

	cstring id = lua_tostring(L, 2);

	// @Optimize?
	if (strcmp(id, "shader") == 0) {
		typedef custom::RefT<custom::Shader_Asset> Asset_Ref;
		LUA_ASSERT_USERDATA("Shader_Asset", 3);
		Asset_Ref const * value = (Asset_Ref const *)lua_touserdata(L, 3);
		object->get_fast()->shader = *value; return 0;
	}

	if (strcmp(id, "texture") == 0) {
		typedef custom::RefT<custom::Texture_Asset> Asset_Ref;
		LUA_ASSERT_USERDATA("Texture_Asset", 3);
		Asset_Ref const * value = (Asset_Ref const *)lua_touserdata(L, 3);
		object->get_fast()->texture = *value; return 0;
	}

	if (strcmp(id, "mesh") == 0) {
		typedef custom::RefT<custom::Mesh_Asset> Asset_Ref;
		LUA_ASSERT_USERDATA("Mesh_Asset", 3);
		Asset_Ref const * value = (Asset_Ref const *)lua_touserdata(L, 3);
		object->get_fast()->mesh = *value; return 0;
	}

	if (strcmp(id, "layer") == 0) {
		LUA_ASSERT_TYPE(LUA_TNUMBER, 3);
		object->get_fast()->layer = (u8)lua_tointeger(L, 3); return 0;
	}

	LUA_REPORT_INDEX();
	return 0;
}

static int Visual_eq(lua_State * L) {
	typedef custom::RefT<Visual> Ref;

	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_ASSERT_USERDATA("Visual", 2);

	Ref const * object1 = (Ref const *)lua_touserdata(L, 1);
	Ref const * object2 = (Ref const *)lua_touserdata(L, 2);

	lua_pushboolean(L, *object1 == *object2);

	return 1;
}

static luaL_Reg const Visual_meta[] = {
	{"__index", Visual_index},
	{"__newindex", Visual_newindex},
	{"__eq", Visual_eq},
	//
	{NULL, NULL},
};

//
//
//

namespace custom {
namespace lua {

void init_client_component_types(lua_State * L) {
	#define COMPONENT_IMPL(T) LUA_META_IMPL(T)
	#include "../registry_impl/component_types.h"
}

}}
