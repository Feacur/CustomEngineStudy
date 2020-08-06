#pragma once
#include "engine/core/math_types.h"
// #include "engine/api/internal/entity_system.h"

struct Lua_Script
{
	cstring function;
};

struct Visual
{
	u32 shader;
	u32 texture;
	u32 mesh;
};

struct Transform
{
	vec3 position;
	quat rotation;
	vec3 scale;
};

struct Camera
{
	mat4 projection;
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
