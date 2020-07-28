#include "engine/core/math_types.h"

struct Lua_Script
{
	cstring function;
	bool loaded = false;
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
	Transform transform;
	mat4 projection;
};

// struct Transform2d
// {
// 	vec2 position;
// 	complex rotation;
// 	vec2 scale;
// };

// struct Camera2d
// {
// 	Transform2d transform;
// 	mat3 projection;
// };
