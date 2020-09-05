#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/api/internal/parsing.h"
#include "engine/impl/entity_system.h"
#include "engine/impl/asset_system.h"
#include "engine/impl/math_linear.h"

#include "../asset_system/asset_types.h"
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
		switch ((parse_void(source), **source)) {
			case 's': ++(*source); {
				cstring line_end = (parse_void(source), *source); skip_to_eol(&line_end);
				u32 id = Asset::store_string(*source, (u32)(line_end - *source));
				component->shader = Asset::add<Shader_Asset>(id);
			} break;

			case 't': ++(*source); {
				cstring line_end = (parse_void(source), *source); skip_to_eol(&line_end);
				u32 id = Asset::store_string(*source, (u32)(line_end - *source));
				component->texture = Asset::add<Texture_Asset>(id);
			} break;

			case 'm': ++(*source); {
				cstring line_end = (parse_void(source), *source); skip_to_eol(&line_end);
				u32 id = Asset::store_string(*source, (u32)(line_end - *source));
				component->mesh = Asset::add<Mesh_Asset>(id);
			} break;

			case 'l': ++(*source); {
				component->layer = (u8)(parse_void(source), parse_u32(source));
			} break;

			case '#': break;
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
		switch ((parse_void(source), **source)) {
			case 'a': ++(*source); {
				cstring line_end = (parse_void(source), *source); skip_to_eol(&line_end);
				u32 id = Asset::store_string(*source, (u32)(line_end - *source));
				Asset::add<Lua_Asset>(id);
			} break;

			case 'u': ++(*source); {
				cstring line_end = (parse_void(source), *source); skip_to_eol(&line_end);
				component->update_string_id = Entity::store_string(*source, (u32)(line_end - *source));
			} break;

			case '#': break;
			default: done = true; break;
		}
	}
}

}}

//
// Physical
//

namespace custom {
namespace serialization {

template<> SERIALIZATION_READ_FUNC(component_pool_serialization_read<Physical>) {
	RefT<Physical> & refT = (RefT<Physical> &)ref;

	Physical * component = refT.get_fast();

	bool done = false;
	while (!done && **source) {
		skip_to_eol(source); parse_eol(source);
		switch ((parse_void(source), **source)) {

			case '#': break;
			default: done = true; break;
		}
	}
}

}}

//
// Phys2d
//

namespace custom {
namespace serialization {

template<> SERIALIZATION_READ_FUNC(component_pool_serialization_read<Phys2d>) {
	RefT<Phys2d> & refT = (RefT<Phys2d> &)ref;

	Phys2d * component = refT.get_fast();

	bool done = false;
	while (!done && **source) {
		skip_to_eol(source); parse_eol(source);
		switch ((parse_void(source), **source)) {
			case 'd': ++(*source); {
				component->dynamic = (parse_void(source), parse_r32(source));
			} break;

			case 'm': ++(*source); {
				component->mass = (parse_void(source), parse_r32(source));
			} break;

			case 'r': ++(*source); {
				component->restitution = (parse_void(source), parse_r32(source));
			} break;

			case 'c': ++(*source); {
				cstring line_end = (parse_void(source), *source); skip_to_eol(&line_end);
				u32 id = Asset::store_string(*source, (u32)(line_end - *source));
				component->mesh = Asset::add<Collider2d_Asset>(id);
			} break;

			case '#': break;
			default: done = true; break;
		}
	}
}

}}
