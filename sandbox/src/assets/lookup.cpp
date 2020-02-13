#include "ids.h"
#include "engine/api/asset_lookup.h"
#include "engine/core/code.h"

namespace custom {
namespace asset {

namespace shader {
	Meta meta_presets[] = {
		{
			graphics::Shader_Part::Vertex | graphics::Shader_Part::Pixel,
		},
	};
	u32 meta_presets_count = C_ARRAY_LENGTH(meta_presets);

	constexpr u32 const ids_count = (u32)sandbox::Shader::count;
	u32 count = ids_count;
	cstring paths[ids_count + 1] = {
		"assets/shaders/v2_texture_tint.glsl",
		"assets/shaders/v3_texture_tint.glsl",
		NULL
	};
	
	u8 meta_ids[ids_count] = {
		0,
		0,
	};
};

namespace uniform {
	constexpr u32 const ids_count = (u32)sandbox::Uniform::count;
	u32 count = ids_count;
	cstring names[ids_count + 1] = {
		"u_ViewProjection",
		"u_Transform",
		"u_Texture",
		"u_Color",
		"u_Depth",
		NULL
	};
};

namespace texture {
	Meta meta_presets[] = {
		{
			graphics::Filter_Mode::Linear,
			graphics::Filter_Mode::None,
			graphics::Filter_Mode::None,
			graphics::Wrap_Mode::Repeat, graphics::Wrap_Mode::Repeat,
		},
		{
			graphics::Filter_Mode::None,
			graphics::Filter_Mode::None,
			graphics::Filter_Mode::None,
			graphics::Wrap_Mode::Repeat, graphics::Wrap_Mode::Repeat,
		},
	};
	u32 meta_presets_count = C_ARRAY_LENGTH(meta_presets);

	constexpr u32 const ids_count = (u32)sandbox::Texture::count;
	u32 count = ids_count;
	cstring paths[ids_count + 1] = {
		"assets/textures/checkerboard.png",
		"assets/textures/blue_noise.png",
		NULL
	};
	
	u8 meta_ids[ids_count] = {
		0,
		1,
	};
};

namespace mesh {
	Meta meta_presets[] = {
		{
			graphics::Mesh_Frequency::Static, graphics::Mesh_Access::Draw,
			graphics::Mesh_Frequency::Static, graphics::Mesh_Access::Draw,
		},
		{
			graphics::Mesh_Frequency::Dynamic, graphics::Mesh_Access::Draw,
			graphics::Mesh_Frequency::Static, graphics::Mesh_Access::Draw,
		},
	};
	u32 meta_presets_count = C_ARRAY_LENGTH(meta_presets);

	constexpr u32 const ids_count = (u32)sandbox::Mesh::count;
	u32 count = ids_count;
	cstring paths[ids_count + 1] = {
		"assets/meshes/teapot.obj",
		NULL
	};
	
	u8 meta_ids[ids_count] = {
		0,
	};
};

}}
