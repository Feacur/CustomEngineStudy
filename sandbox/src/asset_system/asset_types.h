#pragma once
#include "engine/core/types.h"
#include "engine/core/collection_types.h"

struct Lua_Asset {
	custom::Array<u8> source;

	void update(custom::Array<u8> & file);

	~Lua_Asset() = default;
};
