#include "engine/api/internal/entity_system.h"
#include "engine/core/math_types.h"

#define COMPONENT_IMPL(T)\
	static u32 offset;\
	static custom::Ref_Pool<T> pool;\

struct Visual
{
	u32 shader;
	u32 texture;
	u32 mesh;
	COMPONENT_IMPL(Visual)
};

struct Transform
{
	vec3 position;
	quat rotation;
	vec3 scale;
	COMPONENT_IMPL(Transform)
};

struct Transform2d
{
	vec2 position;
	complex rotation;
	vec2 scale;
	COMPONENT_IMPL(Transform2d)
};

#undef COMPONENT_IMPL
