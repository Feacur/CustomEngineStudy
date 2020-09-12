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
	// meta
	// @Note: no need to make the mass infinite, just set this to 0
	// @Change: might factor this into XY and angle settings
	r32 dynamic = 1;

	// material
	r32 mass       = 1;
	r32 elasticity = 0.5;
	r32 roughness  = 0.3;
	r32 stickiness = 0.6;
	r32 stillness  = 0.8;

	// shape
	// @Note: represents `angular mass`, `moment of inertia`, `rotational inertia`
	//        without mass itself; thus, it's only `area`, not `mass` * `area`
	r32 shape = 1;
	custom::Asset_RefT<custom::Collider2d_Asset> mesh = {custom::empty_ref, custom::empty_index};

	// state
	// @Change: store momentum and force instead, so to be able to change mass,
	//          affecting the velocity and acceleration?
	vec2 velocity             = {0, 0};
	vec2 acceleration         = {0, 0};
	r32  angular_velocity     = 0;
	r32  angular_acceleration = 0;

	void add_impulse(vec2 value, vec2 radius);
	void add_force(vec2 value, vec2 radius);

	// @Note: storing acceleration in here is a bit odd
};
