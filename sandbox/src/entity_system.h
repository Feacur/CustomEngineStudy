#include "engine/api/entity_system.h"

struct Visual
{
	u32 mesh;
	u32 shader;
	u32 texture;

	// Entity API
	static u32 offset;

	// Ref<T> API, creation
	static custom::RefPool<Visual> pool;
};
