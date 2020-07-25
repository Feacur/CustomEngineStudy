#include "engine/core/types.h"
#include "engine/core/meta.h"

namespace sandbox {

enum struct Script : u32
{
	main,
	count,
};
UNDERLYING_TYPE_META(Script, u32)
IS_ENUM_META(Script)

enum struct Shader : u32
{
	v2_texture_tint,
	v3_texture_tint,
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
	depth,
	count,
};
UNDERLYING_TYPE_META(Uniform, u32)
IS_ENUM_META(Uniform)

enum struct Texture : u32
{
	checkerboard,
	blue_noise,
	proto_blue,
	count,
};
UNDERLYING_TYPE_META(Texture, u32)
IS_ENUM_META(Texture)

enum struct Mesh : u32
{
	suzanne,
	suzanne_tris,
	count,
};
UNDERLYING_TYPE_META(Mesh, u32)
IS_ENUM_META(Mesh)

enum struct Runtime_Mesh : u32
{
	quad_xy,
	quad_xz,
	quad2d,
	cube,
	particle_test,
	count,
};
UNDERLYING_TYPE_META(Runtime_Mesh, u32)
IS_ENUM_META(Runtime_Mesh)

}
