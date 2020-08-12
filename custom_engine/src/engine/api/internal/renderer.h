#pragma once
#include "engine/core/math_types.h"

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
namespace renderer {

void init(Bytecode * bytecode);

void set_shader(RefT<ShaderAsset> const & asset);
void set_mesh(u32 mesh);
void set_texture(RefT<ShaderAsset> const & shader, u32 uniform, u32 texture);
void set_uniform(RefT<ShaderAsset> const & shader, u32 uniform, mat4 const & matrix);
void set_uniform(RefT<ShaderAsset> const & shader, u32 uniform, mat3 const & matrix);
void set_uniform(RefT<ShaderAsset> const & shader, u32 uniform, ivec2 const & value);

void viewport(ivec2 const & position, ivec2 const & size);
void clear(void);
void draw(void);

}}
