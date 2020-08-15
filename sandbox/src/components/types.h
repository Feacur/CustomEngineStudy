#pragma once
#include "engine/core/math_types.h"
#include "engine/api/internal/entity_system.h"

namespace custom {
	// @Forward
	struct Shader_Asset;  template<typename Shader_Asset>  struct RefT;
	struct Texture_Asset; template<typename Texture_Asset> struct RefT;
	struct Mesh_Asset;    template<typename Mesh_Asset>    struct RefT;
}

struct Lua_Script
{
	cstring update;
};

struct Visual
{
	custom::RefT<custom::Shader_Asset> shader;
	custom::RefT<custom::Texture_Asset> texture;
	custom::RefT<custom::Mesh_Asset> mesh;
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
