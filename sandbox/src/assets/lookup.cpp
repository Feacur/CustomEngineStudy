#include "engine/api/asset_structs.h"
#include "engine/core/code.h"

#include "ids.h"

namespace custom {
namespace asset {

namespace script {
	constexpr u32 const ids_count = (u32)sandbox::Script::count;
	u32 count = ids_count;
	cstring paths[ids_count + 1] = {
		"assets/scripts/main.lua",
		NULL
	};
};

namespace uniform {
	constexpr u32 const ids_count = (u32)sandbox::Uniform::count;
	u32 count = ids_count;
	cstring names[ids_count + 1] = {
		"u_Resolution",
		"u_ViewProjection",
		"u_Transform",
		"u_Texture",
		"u_Color",
		"u_Z",
		NULL
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
		"assets/meshes/plane_xz.obj",
		"assets/meshes/suzanne.obj",
		NULL
	};
	
	u8 meta_ids[ids_count] = {
		0,
		0,
	};
};

namespace target {
	Meta meta_presets[] = {
		{
			(u32)sandbox::Texture::offscreen,
			ivec2{0, 0},
			graphics::Data_Type::u32,
			graphics::Texture_Type::DStencil
		},
	};
	u32 meta_presets_count = C_ARRAY_LENGTH(meta_presets);

	constexpr u32 const ids_count = (u32)sandbox::Target::count;
	u32 count = ids_count;
	
	u8 meta_ids[ids_count] = {
		0,
	};
};

}}
