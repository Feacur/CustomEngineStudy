#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/impl/entity_system.h"
#include "engine/impl/asset_system.h"

#include "component_types.h"

//
// Visual
//

namespace custom {
namespace serialization {

template<> SERIALIZATION_READ_FUNC(component_pool_serialization_read<Visual>) {
	RefT<Visual> & refT = (RefT<Visual> &)ref;

	Visual * component = refT.get_fast();

	// @Todo
	(*component) = {
		Asset::add<Shader_Asset>("assets/shaders/v3_texture_tint.glsl", true),
		Asset::add<Texture_Asset>("assets/textures/checkerboard.png", true),
		Asset::add<Mesh_Asset>("assets/meshes/suzanne.obj", true),
		0,
	};
}

}}

//
// Lua_Script
//

namespace custom {
namespace serialization {

template<> SERIALIZATION_READ_FUNC(component_pool_serialization_read<Lua_Script>) {
	RefT<Lua_Script> & refT = (RefT<Lua_Script> &)ref;

	Lua_Script * component = refT.get_fast();

	// @Todo
	(*component) = {
		"script_rotate"
	};
}

}}
