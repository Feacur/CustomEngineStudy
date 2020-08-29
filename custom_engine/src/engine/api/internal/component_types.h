#pragma once
#include "engine/core/math_types.h"
#include "engine/api/graphics_params.h"
#include "engine/api/internal/entity_system.h"

namespace custom {

}

struct Transform
{
	vec3 position;
	quat rotation;
	vec3 scale;
};

struct Camera
{
	r32 near;
	r32 far;
	r32 scale;
	r32 ortho;
	custom::graphics::Clear_Flag clear;
	u8 layer;
};

struct Hierarchy {
	struct Link { u32 parent; custom::Entity child; };
	static custom::Array<Link> links;
	custom::Entity parent = {custom::empty_ref};

	static void set_parent(custom::Entity & child, custom::Entity const & entity);
};

// struct Transform2d
// {
// 	vec2 position;
// 	complex rotation;
// 	vec2 scale;
// };

// struct Camera2d
// {
// 	mat3 projection;
// };
