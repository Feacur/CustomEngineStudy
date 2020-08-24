#pragma once
#include "engine/core/types.h"
#include "engine/api/internal/reference.h"

namespace custom {
	// @Forward
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
	custom::RefT<custom::Shader_Asset> shader = {custom::empty_ref};
	custom::RefT<custom::Texture_Asset> texture = {custom::empty_ref};
	custom::RefT<custom::Mesh_Asset> mesh = {custom::empty_ref};
	u8 layer;
};
