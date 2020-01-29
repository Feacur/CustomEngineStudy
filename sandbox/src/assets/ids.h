#include "engine/core/types.h"
#include "engine/core/meta.h"

namespace sandbox {

enum struct Shader : u32
{
	renderer2d,
	device,
	count,
};
UNDERLYING_TYPE_META(Shader, u32)
IS_ENUM_META(Shader)

enum struct Texture : u32
{
	checkerboard,
	count,
};
UNDERLYING_TYPE_META(Texture, u32)
IS_ENUM_META(Texture)

enum struct Mesh : u32
{
	quad,
	count,
};
UNDERLYING_TYPE_META(Mesh, u32)
IS_ENUM_META(Mesh)

}