#pragma once
#include "engine/core/math_types.h"
// #include "engine/api/internal/entity_system.h"

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
	r32 persp;
};

// struct Hierarchy : public custom::Ref
// {
// 	// @Note: another entity reference
// };

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