#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/api/internal/strings.h"
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
	while (!done && **source) {
		skip_to_eol(source); parse_eol(source);
		parse_void(source);
		switch (**source) {
			case 's': ++(*source); {
				u32 id = custom::get_or_add_id("assets/shaders/v3_texture_tint.glsl", custom::empty_index);
				component->shader = Asset::add<Shader_Asset>(id, true);
			} break;

			case 't': ++(*source); {
				u32 id = custom::get_or_add_id("assets/textures/checkerboard.png", custom::empty_index);
				component->texture = Asset::add<Texture_Asset>(id, true);
			} break;

			case 'm': ++(*source); {
				u32 id = custom::get_or_add_id("assets/meshes/suzanne.obj", custom::empty_index);
				component->mesh = Asset::add<Mesh_Asset>(id, true);
			} break;

			case 'l': ++(*source); component->layer = (u8)parse_u32(source); break;
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
	while (!done && **source) {
		skip_to_eol(source); parse_eol(source);
		parse_void(source);
		switch (**source) {
			case 'u': ++(*source); {
				parse_void(source);
				cstring line_end = *source; skip_to_eol(&line_end);
				// @Todo
				component->update_todo_strings_index = custom::get_or_add_id(*source, (u32)(line_end - *source));
			} break;

			default: done = true; break;
		}
	}
}

}}
