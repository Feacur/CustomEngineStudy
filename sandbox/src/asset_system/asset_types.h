#pragma once
#include "engine/core/types.h"
#include "engine/core/collection_types.h"
#include "engine/api/internal/entity_system.h"

struct Lua_Asset {
	custom::Array<u8> source;
	~Lua_Asset() = default;
};

struct Prefab : public custom::Entity { };
