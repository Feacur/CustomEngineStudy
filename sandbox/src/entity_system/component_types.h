#pragma once
#include "engine/core/types.h"
#include "engine/core/math_types.h"
#include "engine/api/internal/asset_system.h"

namespace custom {
	// @Forward
	struct Shader_Asset;  template<typename Shader_Asset>  struct RefT;
	struct Texture_Asset; template<typename Texture_Asset> struct RefT;
	struct Mesh_Asset;    template<typename Mesh_Asset>    struct RefT;
}

struct Lua_Script
{
	u32 update_string_id = custom::empty_index;
};

struct Visual
{
	// @Change: use resource ids instead?
	custom::Asset_RefT<custom::Shader_Asset> shader = {custom::empty_ref, custom::empty_index};
	custom::Asset_RefT<custom::Texture_Asset> texture = {custom::empty_ref, custom::empty_index};
	custom::Asset_RefT<custom::Mesh_Asset> mesh = {custom::empty_ref, custom::empty_index};
	u8 layer = 0;
};

struct Physical
{
	u8 dummy;
};

struct Phys2d
{
	// aabb2 aabb;
	vec2 position;
	complex rotation;
	// @Todo: use fixed array? store systemically?
	//        - move actual data out, because that what would happen
	//          using an external physics engine
	custom::Array<vec2> points;
	custom::Array<vec2> transformed;
	r32 is_static;
};
