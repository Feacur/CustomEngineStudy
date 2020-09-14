#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/api/internal/parsing.h"
#include "engine/impl/entity_system.h"
#include "engine/impl/asset_system.h"
#include "engine/impl/math_linear.h"

#include "../asset_system/asset_types.h"
#include "component_types.h"

template<u16 N>
inline static int strncmp_auto(cstring first, char const (& second)[N]) {
	return strncmp(first, second, N - 1);
}

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

	while (**source) {
		skip_to_eol(source); parse_eol(source);

		parse_void(source);

		if (**source == '#') { continue; }

		if (strncmp_auto(*source, "movable ")    == 0) { component->movable    = (skip_to_void(source), parse_void(source), parse_r32(source)); continue; }
		if (strncmp_auto(*source, "rotatable ")  == 0) { component->rotatable  = (skip_to_void(source), parse_void(source), parse_r32(source)); continue; }
		if (strncmp_auto(*source, "mass ")       == 0) { component->mass       = (skip_to_void(source), parse_void(source), parse_r32(source)); continue; }
		if (strncmp_auto(*source, "elasticity ") == 0) { component->elasticity = (skip_to_void(source), parse_void(source), parse_r32(source)); continue; }
		if (strncmp_auto(*source, "roughness ")  == 0) { component->roughness  = (skip_to_void(source), parse_void(source), parse_r32(source)); continue; }
		if (strncmp_auto(*source, "stickiness ") == 0) { component->stickiness = (skip_to_void(source), parse_void(source), parse_r32(source)); continue; }
		if (strncmp_auto(*source, "stillness ")  == 0) { component->stillness  = (skip_to_void(source), parse_void(source), parse_r32(source)); continue; }
		if (strncmp_auto(*source, "shape ")      == 0) { component->shape      = (skip_to_void(source), parse_void(source), parse_r32(source)); continue; }

		if (strncmp_auto(*source, "collider ") == 0) {
			cstring line_end = (skip_to_void(source), parse_void(source), *source); skip_to_eol(&line_end);
			u32 id = Asset::store_string(*source, (u32)(line_end - *source));
			component->mesh = Asset::add<Collider2d_Asset>(id);
			continue;
		}

		break;
	}
}

}}
