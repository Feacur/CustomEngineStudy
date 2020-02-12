#include "engine/api/entity_system.h"
#include "engine/core/math_types.h"

struct Visual
{
	u32 shader;
	u32 texture;
	u32 mesh;

	static u32 offset; // Entity API
	static custom::Ref_Pool<Visual> pool; // Ref<T> API, creation
};

struct Transform
{
	vec3 position;
	quat rotation;
	vec3 scale;

	static u32 offset; // Entity API
	static custom::Ref_Pool<Transform> pool; // Ref<T> API, creation
};

struct Transform2d
{
	vec2 position;
	complex rotation;
	vec2 scale;

	static u32 offset; // Entity API
	static custom::Ref_Pool<Transform2d> pool; // Ref<T> API, creation
};
