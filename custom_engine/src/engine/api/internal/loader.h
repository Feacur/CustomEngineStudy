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

void script(lua_State * L, RefT<Lua_Asset> const & ref);

void image(RefT<Texture_Asset> const & ref);
void imagef(RefT<Texture_Asset> const & ref);
void image16(RefT<Texture_Asset> const & ref);

void uniforms();
void shader(RefT<Shader_Asset> const & ref);

void mesh(RefT<Mesh_Asset> const & ref);

}}
