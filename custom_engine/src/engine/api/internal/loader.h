#pragma once
#include "engine/core/types.h"

struct lua_State;

namespace custom {
	// @Forward
	struct ShaderAsset;  template<typename ShaderAsset>  struct RefT;
	struct TextureAsset; template<typename TextureAsset> struct RefT;
	struct MeshAsset;    template<typename MeshAsset>    struct RefT;
}

namespace custom {
	// @Forward
	struct Bytecode;
}

namespace custom {
namespace loader {

void init(Bytecode * bc);

void script(lua_State * L, u32 asset_id);

void image(u32 asset_id);
void imagef(u32 asset_id);
void image16(u32 asset_id);

void uniforms();
void shader(RefT<ShaderAsset> const & asset);

void mesh(u32 asset_id);

}}
