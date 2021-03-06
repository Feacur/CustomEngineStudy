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
	// instance:###
	// Type.###
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
		typedef custom::Asset_RefT<custom::Shader_Asset> Asset_Ref;
		Asset_Ref * udata = (Asset_Ref *)lua_newuserdatauv(L, sizeof(Asset_Ref), 0);
		luaL_setmetatable(L, "Shader_Asset");
		*udata = object->get_fast()->shader;
		return 1;
	}
	
	if (strcmp(id, "texture") == 0) {
		typedef custom::Asset_RefT<custom::Texture_Asset> Asset_Ref;
		Asset_Ref * udata = (Asset_Ref *)lua_newuserdatauv(L, sizeof(Asset_Ref), 0);
		luaL_setmetatable(L, "Texture_Asset");
		*udata = object->get_fast()->texture;
		return 1;
	}
	
	if (strcmp(id, "mesh") == 0) {
		typedef custom::Asset_RefT<custom::Mesh_Asset> Asset_Ref;
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
		typedef custom::Asset_RefT<custom::Shader_Asset> Asset_Ref;
		LUA_ASSERT_USERDATA("Shader_Asset", 3);
		Asset_Ref const * value = (Asset_Ref const *)lua_touserdata(L, 3);
		object->get_fast()->shader = *value; return 0;
	}

	if (strcmp(id, "texture") == 0) {
		typedef custom::Asset_RefT<custom::Texture_Asset> Asset_Ref;
		LUA_ASSERT_USERDATA("Texture_Asset", 3);
		Asset_Ref const * value = (Asset_Ref const *)lua_touserdata(L, 3);
		object->get_fast()->texture = *value; return 0;
	}

	if (strcmp(id, "mesh") == 0) {
		typedef custom::Asset_RefT<custom::Mesh_Asset> Asset_Ref;
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
	// instance:###
	// Type.###
	//
	{NULL, NULL},
};

//
// Physical
//

static int Physical_index(lua_State * L) {
	typedef custom::RefT<Physical> Ref;

	LUA_INDEX_RAWGET_IMPL(Physical);

	Ref const * object = (Ref const *)lua_touserdata(L, 1);
	CUSTOM_LUA_ASSERT(object->exists(), "object doesn't exist");

	cstring id = lua_tostring(L, 2);

	// @Optimize?
	// if (strcmp(id, "") == 0) { lua_pushnil(L); return 1; }

	LUA_REPORT_INDEX();
	lua_pushnil(L); return 1;
}

static int Physical_newindex(lua_State * L) {
	typedef custom::RefT<Physical> Ref;

	Ref * object = (Ref *)lua_touserdata(L, 1);
	CUSTOM_LUA_ASSERT(object->exists(), "object doesn't exist");

	cstring id = lua_tostring(L, 2);

	// @Optimize?
	// if (strcmp(id, "") == 0) { return 0;}

	LUA_REPORT_INDEX();
	return 0;
}

static int Physical_eq(lua_State * L) {
	typedef custom::RefT<Physical> Ref;

	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_ASSERT_USERDATA("Physical", 2);

	Ref const * object1 = (Ref const *)lua_touserdata(L, 1);
	Ref const * object2 = (Ref const *)lua_touserdata(L, 2);

	lua_pushboolean(L, *object1 == *object2);

	return 1;
}

static luaL_Reg const Physical_meta[] = {
	{"__index", Physical_index},
	{"__newindex", Physical_newindex},
	{"__eq", Physical_eq},
	// instance:###
	// Type.###
	//
	{NULL, NULL},
};

//
// Phys2d
//

static int Phys2d_index(lua_State * L) {
	typedef custom::RefT<Phys2d> Ref;

	LUA_INDEX_RAWGET_IMPL(Phys2d);

	Ref const * object = (Ref const *)lua_touserdata(L, 1);
	CUSTOM_LUA_ASSERT(object->exists(), "object doesn't exist");

	cstring id = lua_tostring(L, 2);

	// @Optimize?
	if (strcmp(id, "movable")   == 0) { lua_pushnumber(L, object->get_fast()->movable);   return 1; }
	if (strcmp(id, "rotatable") == 0) { lua_pushnumber(L, object->get_fast()->rotatable); return 1; }

	if (strcmp(id, "mass")       == 0) { lua_pushnumber(L, object->get_fast()->mass);       return 1; }
	if (strcmp(id, "elasticity") == 0) { lua_pushnumber(L, object->get_fast()->elasticity); return 1; }
	if (strcmp(id, "roughness")  == 0) { lua_pushnumber(L, object->get_fast()->roughness);  return 1; }
	if (strcmp(id, "stickiness") == 0) { lua_pushnumber(L, object->get_fast()->stickiness); return 1; }
	if (strcmp(id, "stillness")  == 0) { lua_pushnumber(L, object->get_fast()->stillness);  return 1; }

	if (strcmp(id, "shape") == 0) { lua_pushnumber(L, object->get_fast()->shape); return 1; }
	if (strcmp(id, "mesh")  == 0) {
		typedef custom::Asset_RefT<custom::Collider2d_Asset> Asset_Ref;
		Asset_Ref * udata = (Asset_Ref *)lua_newuserdatauv(L, sizeof(Asset_Ref), 0);
		luaL_setmetatable(L, "Collider2d_Asset");
		*udata = object->get_fast()->mesh;
		return 1;
	}

	if (strcmp(id, "acceleration") == 0) {
		vec2 * udata = (vec2 *)lua_newuserdatauv(L, sizeof(vec2), 0);
		luaL_setmetatable(L, "vec2");
		*udata = object->get_fast()->acceleration;
		return 1;
	}

	if (strcmp(id, "velocity") == 0) {
		vec2 * udata = (vec2 *)lua_newuserdatauv(L, sizeof(vec2), 0);
		luaL_setmetatable(L, "vec2");
		*udata = object->get_fast()->velocity;
		return 1;
	}

	if (strcmp(id, "angular_velocity")     == 0) { lua_pushnumber(L, object->get_fast()->angular_velocity);     return 1; }
	if (strcmp(id, "angular_acceleration") == 0) { lua_pushnumber(L, object->get_fast()->angular_acceleration); return 1; }

	LUA_REPORT_INDEX();
	lua_pushnil(L); return 1;
}

static int Phys2d_newindex(lua_State * L) {
	typedef custom::RefT<Phys2d> Ref;

	Ref * object = (Ref *)lua_touserdata(L, 1);
	CUSTOM_LUA_ASSERT(object->exists(), "object doesn't exist");

	cstring id = lua_tostring(L, 2);

	// @Optimize?
	if (strcmp(id, "movable")   == 0) { object->get_fast()->movable   = (r32)lua_tonumber(L, 3); return 0; }
	if (strcmp(id, "rotatable") == 0) { object->get_fast()->rotatable = (r32)lua_tonumber(L, 3); return 0; }

	if (strcmp(id, "mass")       == 0) { object->get_fast()->mass       = (r32)lua_tonumber(L, 3); return 0; }
	if (strcmp(id, "elasticity") == 0) { object->get_fast()->elasticity = (r32)lua_tonumber(L, 3); return 0; }
	if (strcmp(id, "roughness")  == 0) { object->get_fast()->roughness  = (r32)lua_tonumber(L, 3); return 0; }
	if (strcmp(id, "stickiness") == 0) { object->get_fast()->stickiness = (r32)lua_tonumber(L, 3); return 0; }
	if (strcmp(id, "stillness")  == 0) { object->get_fast()->stillness  = (r32)lua_tonumber(L, 3); return 0; }

	if (strcmp(id, "shape") == 0) { object->get_fast()->shape = (r32)lua_tonumber(L, 3); return 0; }
	if (strcmp(id, "mesh")  == 0) {
		typedef custom::Asset_RefT<custom::Collider2d_Asset> Asset_Ref;
		LUA_ASSERT_USERDATA("Collider2d_Asset", 3);
		Asset_Ref const * value = (Asset_Ref const *)lua_touserdata(L, 3);
		object->get_fast()->mesh = *value; return 0;
	}

	if (strcmp(id, "acceleration") == 0) {
		LUA_ASSERT_USERDATA("vec2", 3);
		vec2 const * value = (vec2 const *)lua_touserdata(L, 3);
		object->get_fast()->acceleration = *value;
		return 0;
	}

	if (strcmp(id, "velocity") == 0) {
		LUA_ASSERT_USERDATA("vec2", 3);
		vec2 const * value = (vec2 const *)lua_touserdata(L, 3);
		object->get_fast()->velocity = *value;
		return 0;
	}

	if (strcmp(id, "angular_velocity")     == 0) { object->get_fast()->angular_velocity     = (r32)lua_tonumber(L, 3); return 0; }
	if (strcmp(id, "angular_acceleration") == 0) { object->get_fast()->angular_acceleration = (r32)lua_tonumber(L, 3); return 0; }

	LUA_REPORT_INDEX();
	return 0;
}

static int Phys2d_eq(lua_State * L) {
	typedef custom::RefT<Phys2d> Ref;

	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_ASSERT_USERDATA("Phys2d", 2);

	Ref const * object1 = (Ref const *)lua_touserdata(L, 1);
	Ref const * object2 = (Ref const *)lua_touserdata(L, 2);

	lua_pushboolean(L, *object1 == *object2);

	return 1;
}

static int Phys2d_add_impulse(lua_State * L) {
	typedef custom::RefT<Phys2d> Ref;

	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_ASSERT_USERDATA("Phys2d", 1);
	LUA_ASSERT_USERDATA("vec2", 2);

	Ref * object = (Ref *)lua_touserdata(L, 1);
	CUSTOM_LUA_ASSERT(object->exists(), "object doesn't exist");

	vec2 const * value = (vec2 const *)lua_touserdata(L, 2);
	object->get_fast()->add_impulse(*value, {0, 0});

	return 0;
}

static int Phys2d_add_force(lua_State * L) {
	typedef custom::RefT<Phys2d> Ref;

	CUSTOM_LUA_ASSERT(lua_gettop(L) == 2, "expected 2 arguments");
	LUA_ASSERT_USERDATA("Phys2d", 1);
	LUA_ASSERT_USERDATA("vec2", 2);

	Ref * object = (Ref *)lua_touserdata(L, 1);
	CUSTOM_LUA_ASSERT(object->exists(), "object doesn't exist");

	vec2 const * value = (vec2 const *)lua_touserdata(L, 2);
	object->get_fast()->add_force(*value, {0, 0});

	return 0;
}

static luaL_Reg const Phys2d_meta[] = {
	{"__index", Phys2d_index},
	{"__newindex", Phys2d_newindex},
	{"__eq", Phys2d_eq},
	// instance:###
	{"add_impulse", Phys2d_add_impulse},
	{"add_force", Phys2d_add_force},
	// Type.###
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
