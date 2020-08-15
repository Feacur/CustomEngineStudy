#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/api/lua.h"
#include "engine/impl/asset_system.h"
#include "../components/types.h"

#include <lua.hpp>
// #include <lstate.h>

static int Lua_Script_index(lua_State * L) {
	typedef custom::RefT<Lua_Script> Ref;

	LUA_INDEX_RAWGET_IMPL(Lua_Script);

	Ref const * object = (Ref const *)lua_touserdata(L, 1);
	CUSTOM_LUA_ASSERT(object->exists(), "object doesn't exist");

	cstring id = lua_tostring(L, 2);

	// @Optimize?
	if (strcmp(id, "update") == 0) { lua_pushstring(L, object->get_fast()->update); return 1; }

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
		object->get_fast()->update = lua_tostring(L, 3); return 0;
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

static int Transform_index(lua_State * L) {
	typedef custom::RefT<Transform> Ref;

	LUA_INDEX_RAWGET_IMPL(Transform);

	Ref const * object = (Ref const *)lua_touserdata(L, 1);
	CUSTOM_LUA_ASSERT(object->exists(), "object doesn't exist");

	cstring id = lua_tostring(L, 2);

	// @Optimize?
	if (strcmp(id, "position") == 0) {
		vec3 * udata = (vec3 *)lua_newuserdatauv(L, sizeof(vec3), 0);
		luaL_setmetatable(L, "vec3");
		*udata = object->get_fast()->position;
		return 1;
	}

	if (strcmp(id, "rotation") == 0) {
		vec4 * udata = (vec4 *)lua_newuserdatauv(L, sizeof(vec4), 0);
		luaL_setmetatable(L, "vec4");
		*udata = object->get_fast()->rotation;
		return 1;
	}

	if (strcmp(id, "scale") == 0) {
		vec3 * udata = (vec3 *)lua_newuserdatauv(L, sizeof(vec3), 0);
		luaL_setmetatable(L, "vec3");
		*udata = object->get_fast()->scale;
		return 1;
	}

	LUA_REPORT_INDEX();
	lua_pushnil(L); return 1;
}

static int Transform_newindex(lua_State * L) {
	typedef custom::RefT<Transform> Ref;

	Ref * object = (Ref *)lua_touserdata(L, 1);
	CUSTOM_LUA_ASSERT(object->exists(), "object doesn't exist");

	cstring id = lua_tostring(L, 2);

	// @Optimize?
	if (strcmp(id, "position") == 0) {
		LUA_ASSERT_USERDATA("vec3", 3);
		vec3 const * value = (vec3 const *)lua_touserdata(L, 3);
		object->get_fast()->position = *value;
		return 0;
	}

	if (strcmp(id, "rotation") == 0) {
		LUA_ASSERT_USERDATA("vec4", 3);
		vec4 const * value = (vec4 const *)lua_touserdata(L, 3);
		object->get_fast()->rotation = *value;
		return 0;
	}

	if (strcmp(id, "scale") == 0) {
		LUA_ASSERT_USERDATA("vec3", 3);
		vec3 const * value = (vec3 const *)lua_touserdata(L, 3);
		object->get_fast()->scale = *value;
		return 0;
	}

	LUA_REPORT_INDEX();
	return 0;
}

static int Transform_eq(lua_State * L) {
	typedef custom::RefT<Transform> Ref;

	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_ASSERT_USERDATA("Transform", 2);

	Ref const * object1 = (Ref const *)lua_touserdata(L, 1);
	Ref const * object2 = (Ref const *)lua_touserdata(L, 2);

	lua_pushboolean(L, *object1 == *object2);

	return 1;
}

static luaL_Reg const Transform_meta[] = {
	{"__index", Transform_index},
	{"__newindex", Transform_newindex},
	{"__eq", Transform_eq},
	//
	{NULL, NULL},
};

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
		luaL_setmetatable(L, "Shader_Asset");
		*udata = object->get_fast()->texture;
		return 1;
	}

	if (strcmp(id, "mesh") == 0) {
		typedef custom::RefT<custom::Mesh_Asset> Asset_Ref;
		lua_pushinteger(L, object->get_fast()->mesh);
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

	// @Todo: implement assets interface
	if (strcmp(id, "mesh") == 0) {
		typedef custom::Mesh_Asset Mesh_Asset;
		LUA_ASSERT_TYPE(LUA_TNUMBER, 3);
		object->get_fast()->mesh = (u32)lua_tonumber(L, 3); return 0;
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

namespace custom {
namespace lua_client {

void init_components(lua_State * L) {
	#define COMPONENT_IMPL(T) LUA_META_IMPL(T)
	#include "../components/registry_impl.h"
}

}}
