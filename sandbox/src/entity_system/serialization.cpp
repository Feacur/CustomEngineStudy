#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/api/internal/parsing.h"
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

	bool done = false;
	while (!done && *source < end) {
		parse_eol(source, end); parse_void(source);
		switch (**source) {
			case 's': component->shader  = Asset::add<Shader_Asset>("assets/shaders/v3_texture_tint.glsl", true); break;
			case 't': component->texture = Asset::add<Texture_Asset>("assets/textures/checkerboard.png", true);   break;
			case 'm': component->mesh    = Asset::add<Mesh_Asset>("assets/meshes/suzanne.obj", true);             break;
			case 'l': component->layer   = 0; break;
			default: done = true; break;
		}
	}
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

	bool done = false;
	while (!done && *source < end) {
		parse_eol(source, end); parse_void(source);
		switch (**source) {
			case 'u': component->update = "script_rotate"; break;
			default: done = true; break;
		}
	}
}

}}
