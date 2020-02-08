#include "engine/entity_system/reference.h"

struct Visual
{
	u32 shader;
	u32 texture;
	u32 mesh;

	static u32 offset; // Entity API
	static custom::Ref_Pool<Visual> pool; // Ref<T> API, creation
};
