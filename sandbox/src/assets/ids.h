#include "engine/core/types.h"
#include "engine/core/meta.h"

namespace sandbox {

enum struct Shader : u32
{
	v2_texture_tint,
	v3_texture_tint,
	v0_texture,
	v0_particle_color,
	count,
};
UNDERLYING_TYPE_META(Shader, u32)
IS_ENUM_META(Shader)

enum struct Uniform : u32
{
	view_proj,
	transform,
	texture,
	color,
	count,
};
UNDERLYING_TYPE_META(Uniform, u32)
IS_ENUM_META(Uniform)

enum struct Texture : u32
{
	checkerboard,
	count,
};
UNDERLYING_TYPE_META(Texture, u32)
IS_ENUM_META(Texture)

enum struct Mesh : u32
{
	teapot,
	count,
};
UNDERLYING_TYPE_META(Mesh, u32)
IS_ENUM_META(Mesh)

enum struct Runtime_Mesh : u32
{
	quad,
	cube,
	particle_test,
	count,
};
UNDERLYING_TYPE_META(Runtime_Mesh, u32)
IS_ENUM_META(Runtime_Mesh)

}
