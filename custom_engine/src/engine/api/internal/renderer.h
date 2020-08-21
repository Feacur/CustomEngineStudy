#pragma once
#include "engine/core/math_types.h"
#include "engine/api/graphics_params.h"

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

graphics::unit_id make_unit(RefT<Texture_Asset> const & texture);

void set_shader(RefT<Shader_Asset> const & asset);
void set_mesh(RefT<Mesh_Asset> const & asset);
void set_uniform_bytes(RefT<Shader_Asset> const & shader, u32 uniform, u8 const * data, u32 count, graphics::Data_Type type);

void viewport(ivec2 const & position, ivec2 const & size);
void clear(graphics::Clear_Flag flags);
void draw(void);

template<typename T> void set_uniform(RefT<Shader_Asset> const & shader, u32 uniform, T const & value);

}}
