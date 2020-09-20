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

	CUSTOM_ASSERT(strncmp_auto(*source, "Visual") == 0, "");
	while ((to_next_line(source), **source)) {

		parse_void(source);
		if (**source == '#') { continue; }
		if (!IS_VALID_IDENTIFIER_START(**source)) { break; }

		u32 key_length = to_identifier_length(source);
		cstring key    = *source;
		skip_to_void(source);

		if (strncmp_auto(key, "shader ") == 0) {
			u32 path_length = to_string_length(source);
			u32 path_id     = Asset::store_string(*source, path_length);
			component->shader = Asset::add<Shader_Asset>(path_id);
			continue;
		}

		if (strncmp_auto(key, "texture ") == 0) {
			u32 path_length = to_string_length(source);
			u32 path_id     = Asset::store_string(*source, path_length);
			component->texture = Asset::add<Texture_Asset>(path_id);
			continue;
		}

		if (strncmp_auto(key, "mesh ") == 0) {
			u32 path_length = to_string_length(source);
			u32 path_id     = Asset::store_string(*source, path_length);
			component->mesh = Asset::add<Mesh_Asset>(path_id);
			continue;
		}

		if (strncmp_auto(key, "layer ") == 0) { component->layer = (u8)to_u32(source); continue; }

		*source = key; break;
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

	CUSTOM_ASSERT(strncmp_auto(*source, "Lua_Script") == 0, "");
	while ((to_next_line(source), **source)) {

		parse_void(source);
		if (**source == '#') { continue; }
		if (!IS_VALID_IDENTIFIER_START(**source)) { break; }

		u32 key_length = to_identifier_length(source);
		cstring key    = *source;
		skip_to_void(source);

		if (strncmp_auto(key, "asset ") == 0) {
			u32 path_length = to_string_length(source);
			u32 path_id     = Asset::store_string(*source, path_length);
			Asset::add<Lua_Asset>(path_id);
			continue;
		}

		if (strncmp_auto(key, "update ") == 0) {
			u32 value_length = to_string_length(source);
			component->update_string_id = Entity::store_string(*source, value_length);
			continue;
		}

		*source = key; break;
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

	CUSTOM_ASSERT(strncmp_auto(*source, "Physical") == 0, "");
	while ((to_next_line(source), **source)) {

		parse_void(source);
		if (**source == '#') { continue; }
		if (!IS_VALID_IDENTIFIER_START(**source)) { break; }

		u32 key_length = to_identifier_length(source);
		cstring key    = *source;
		skip_to_void(source);

		*source = key; break;
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

	CUSTOM_ASSERT(strncmp_auto(*source, "Phys2d") == 0, "");
	while ((to_next_line(source), **source)) {

		parse_void(source);
		if (**source == '#') { continue; }
		if (!IS_VALID_IDENTIFIER_START(**source)) { break; }

		u32 key_length = to_identifier_length(source);
		cstring key    = *source;
		skip_to_void(source);

		if (strncmp_auto(key, "movable ")    == 0) { component->movable    = to_r32(source); continue; }
		if (strncmp_auto(key, "rotatable ")  == 0) { component->rotatable  = to_r32(source); continue; }
		if (strncmp_auto(key, "mass ")       == 0) { component->mass       = to_r32(source); continue; }
		if (strncmp_auto(key, "elasticity ") == 0) { component->elasticity = to_r32(source); continue; }
		if (strncmp_auto(key, "roughness ")  == 0) { component->roughness  = to_r32(source); continue; }
		if (strncmp_auto(key, "stickiness ") == 0) { component->stickiness = to_r32(source); continue; }
		if (strncmp_auto(key, "stillness ")  == 0) { component->stillness  = to_r32(source); continue; }
		if (strncmp_auto(key, "shape ")      == 0) { component->shape      = to_r32(source); continue; }

		if (strncmp_auto(key, "collider ") == 0) {
			u32 path_length = to_string_length(source);
			u32 path_id     = Asset::store_string(*source, path_length);
			component->mesh = Asset::add<Collider2d_Asset>(path_id);
			continue;
		}

		*source = key; break;
	}
}

}}
