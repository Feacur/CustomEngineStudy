#pragma once
#include "engine/core/math_types.h"

namespace custom {
	// @Forward
	struct Shader_Asset;  template<typename Shader_Asset>  struct RefT;
	struct Texture_Asset; template<typename Texture_Asset> struct RefT;
	struct Mesh_Asset;    template<typename Mesh_Asset>    struct RefT;
}

namespace custom {
	// @Forward
	struct Bytecode;
}

namespace custom {
namespace renderer {

void init(Bytecode * bytecode);

void set_shader(RefT<Shader_Asset> const & asset);
void set_mesh(u32 mesh);
void set_texture(RefT<Shader_Asset> const & shader, u32 uniform, u32 texture);
void set_uniform(RefT<Shader_Asset> const & shader, u32 uniform, mat4 const & matrix);
void set_uniform(RefT<Shader_Asset> const & shader, u32 uniform, mat3 const & matrix);
void set_uniform(RefT<Shader_Asset> const & shader, u32 uniform, ivec2 const & value);

void viewport(ivec2 const & position, ivec2 const & size);
void clear(void);
void draw(void);

}}
