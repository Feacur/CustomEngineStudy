#include "engine/api/internal/entity_system.h"
#include "engine/core/math_types.h"

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

struct Transform2d
{
	vec2 position;
	complex rotation;
	vec2 scale;
};
