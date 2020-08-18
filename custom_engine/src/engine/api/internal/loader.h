#pragma once
#include "engine/core/types.h"

struct lua_State;

namespace custom {
	// @Forward
	struct Lua_Asset;     template<typename Lua_Asset>     struct RefT;
	struct Shader_Asset;  template<typename Shader_Asset>  struct RefT;
	struct Texture_Asset; template<typename Texture_Asset> struct RefT;
	struct Mesh_Asset;    template<typename Mesh_Asset>    struct RefT;
}

namespace custom {
	// @Forward
	struct Bytecode;
}

namespace custom {
namespace loader {

void init(Bytecode * bc);

void uniforms();

}}
