#include "engine/api/entity_system.h"
#include "engine/core/math_types.h"

struct Visual
{
	u32 shader;
	u32 texture;
	u32 mesh;

	bool camera_space;
	bool has_transform;
	mat4 transform;

	static u32 offset; // Entity API
	static custom::Ref_Pool<Visual> pool; // Ref<T> API, creation
};
