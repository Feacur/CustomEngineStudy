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
	// material
	r32  dynamic    = 1; // no need to make the mass infinite, just set this to 0
	r32  mass       = 1;
	r32  elasticity = 0.5; // a bit artificial way to state restitution for a single object
	r32  roughness  = 0.3; // a bit artificial way to state dynamic friction for a single object
	r32  stickiness = 0.6; // a bit artificial way to state static friction for a single object

	// shape
	custom::Asset_RefT<custom::Collider2d_Asset> mesh = {custom::empty_ref, custom::empty_index};

	// state
	// @Change: store momentum and force instead, so to be able to change mass,
	//          affecting the velocity and acceleration?
	vec2 velocity     = {0, 0};
	vec2 acceleration = {0, 0};

	void add_impulse(vec2 value);
	void add_force(vec2 value);

	// @Note: storing acceleration in here is a bit odd
};
