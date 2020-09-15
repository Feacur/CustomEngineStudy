#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/api/internal/parsing.h"
#include "engine/impl/entity_system.h"
#include "engine/impl/asset_system.h"
#include "engine/impl/math_linear.h"
#include "engine/impl/parsing.h"

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

	while (**source) {
		skip_to_eol(source); parse_eol(source);

		parse_void(source);
		if (**source == '#') { continue; }

		if (strncmp_auto(*source, "shader ") == 0) {
			cstring line_end = to_eol(source);
			u32 id = Asset::store_string(*source, (u32)(line_end - *source));
			component->shader = Asset::add<Shader_Asset>(id);
			continue;
		}

		if (strncmp_auto(*source, "texture ") == 0) {
			cstring line_end = to_eol(source);
			u32 id = Asset::store_string(*source, (u32)(line_end - *source));
			component->texture = Asset::add<Texture_Asset>(id);
			continue;
		}

		if (strncmp_auto(*source, "mesh ") == 0) {
			cstring line_end = to_eol(source);
			u32 id = Asset::store_string(*source, (u32)(line_end - *source));
			component->mesh = Asset::add<Mesh_Asset>(id);
			continue;
		}

		if (strncmp_auto(*source, "layer ") == 0) { component->layer = (u8)to_u32(source); continue; }

		break;
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

	while (**source) {
		skip_to_eol(source); parse_eol(source);

		parse_void(source);
		if (**source == '#') { continue; }

		if (strncmp_auto(*source, "asset ") == 0) {
			cstring line_end = to_eol(source);
			u32 id = Asset::store_string(*source, (u32)(line_end - *source));
			Asset::add<Lua_Asset>(id);
			continue;
		}

		if (strncmp_auto(*source, "update ") == 0) {
			cstring line_end = to_eol(source);
			component->update_string_id = Entity::store_string(*source, (u32)(line_end - *source));
			continue;
		}

		break;
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

	while (**source) {
		skip_to_eol(source); parse_eol(source);

		parse_void(source);
		if (**source == '#') { continue; }

		break;
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

		if (strncmp_auto(*source, "movable ")    == 0) { component->movable    = to_r32(source); continue; }
		if (strncmp_auto(*source, "rotatable ")  == 0) { component->rotatable  = to_r32(source); continue; }
		if (strncmp_auto(*source, "mass ")       == 0) { component->mass       = to_r32(source); continue; }
		if (strncmp_auto(*source, "elasticity ") == 0) { component->elasticity = to_r32(source); continue; }
		if (strncmp_auto(*source, "roughness ")  == 0) { component->roughness  = to_r32(source); continue; }
		if (strncmp_auto(*source, "stickiness ") == 0) { component->stickiness = to_r32(source); continue; }
		if (strncmp_auto(*source, "stillness ")  == 0) { component->stillness  = to_r32(source); continue; }
		if (strncmp_auto(*source, "shape ")      == 0) { component->shape      = to_r32(source); continue; }

		if (strncmp_auto(*source, "collider ") == 0) {
			cstring line_end = to_eol(source);
			u32 id = Asset::store_string(*source, (u32)(line_end - *source));
			component->mesh = Asset::add<Collider2d_Asset>(id);
			continue;
		}

		break;
	}
}

}}
