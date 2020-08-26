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
	while (!done && **source) {
		skip_to_eol(source); parse_eol(source);
		parse_void(source);
		switch (**source) {
			case 's': ++(*source); {
				parse_void(source);
				cstring line_end = *source; skip_to_eol(&line_end);
				u32 id = Asset::store_string(*source, (u32)(line_end - *source));
				component->shader = Asset::add<Shader_Asset>(id, true);
			} break;

			case 't': ++(*source); {
				parse_void(source);
				cstring line_end = *source; skip_to_eol(&line_end);
				u32 id = Asset::store_string(*source, (u32)(line_end - *source));
				component->texture = Asset::add<Texture_Asset>(id, true);
			} break;

			case 'm': ++(*source); {
				parse_void(source);
				cstring line_end = *source; skip_to_eol(&line_end);
				u32 id = Asset::store_string(*source, (u32)(line_end - *source));
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
				component->update_string_id = Entity::store_string(*source, (u32)(line_end - *source));
			} break;

			default: done = true; break;
		}
	}
}

}}
