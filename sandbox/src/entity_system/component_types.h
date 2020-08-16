#pragma once
#include "engine/core/types.h"
#include "engine/api/internal/reference.h"

namespace custom {
	// @Forward
	struct Lua_Asset;     template<typename Lua_Asset>     struct RefT;
	struct Shader_Asset;  template<typename Shader_Asset>  struct RefT;
	struct Texture_Asset; template<typename Texture_Asset> struct RefT;
	struct Mesh_Asset;    template<typename Mesh_Asset>    struct RefT;
}

struct Lua_Script
{
	cstring update;
};

struct Visual
{
	custom::RefT<custom::Shader_Asset> shader = {UINT32_MAX, 0};
	custom::RefT<custom::Texture_Asset> texture = {UINT32_MAX, 0};
	custom::RefT<custom::Mesh_Asset> mesh = {UINT32_MAX, 0};
};
