#pragma once
#include "engine/core/types.h"
#include "engine/core/collection_types.h"

struct Lua_Asset {
	custom::Array<u8> source;
	~Lua_Asset() = default;
};

struct Prefab {
	u8 dummy;
	~Prefab() = default;
};
