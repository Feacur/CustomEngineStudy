#pragma once
#include "engine/core/types.h"
#include "engine/core/math_types.h"
#include "engine/api/internal/asset_system.h"

namespace custom {
	// @Forward
	struct Shader_Asset;
	struct Texture_Asset;
	struct Mesh_Asset;
	struct Collider2d_Asset;
}

struct Lua_Script
{
	u32 update_string_id = custom::empty_index;
};

struct Visual
{
	// @Change: use resource ids instead?
	custom::Asset_RefT<custom::Shader_Asset>  shader  = {custom::empty_ref, custom::empty_index};
	custom::Asset_RefT<custom::Texture_Asset> texture = {custom::empty_ref, custom::empty_index};
	custom::Asset_RefT<custom::Mesh_Asset>    mesh    = {custom::empty_ref, custom::empty_index};
	u8 layer = 0;
};

struct Physical
{
	u8 dummy;
};

struct Phys2d
{
	vec2 position;
	vec2 scale;
	complex rotation;
	custom::Asset_RefT<custom::Collider2d_Asset> mesh = {custom::empty_ref, custom::empty_index};
	custom::Ref internal_data;
	custom::Array<vec2> transformed;
	r32 movable;
};
