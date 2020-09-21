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

template<> READ_FUNC(component_pool_read<Visual>) {
	RefT<Visual> & refT = (RefT<Visual> &)ref;

	static u32 const key_shader  = Entity::store_string("shader",  custom::empty_index);
	static u32 const key_texture = Entity::store_string("texture", custom::empty_index);
	static u32 const key_mesh    = Entity::store_string("mesh",    custom::empty_index);
	static u32 const key_layer   = Entity::store_string("layer",   custom::empty_index);

	Visual * component = refT.get_fast();

	CUSTOM_ASSERT(strncmp_auto(*source, "Visual") == 0, "");
	while ((to_next_line(source), **source)) {

		parse_void(source);
		if (**source == '#') { continue; }
		if (!IS_VALID_IDENTIFIER_START(**source)) { break; }

		u32 key_length = to_identifier_length(source);
		cstring key    = *source; skip_to_void(source);
		u32 key_id     = Entity::get_id(key, key_length);
		if (key_id == custom::empty_index) { *source = key; break; }

		if (key_id == key_shader) {
			u32 path_length = to_string_length(source);
			u32 path_id     = Asset::store_string(*source, path_length);
			component->shader = Asset::add<Shader_Asset>(path_id);
			continue;
		}

		if (key_id == key_texture) {
			u32 path_length = to_string_length(source);
			u32 path_id     = Asset::store_string(*source, path_length);
			component->texture = Asset::add<Texture_Asset>(path_id);
			continue;
		}

		if (key_id == key_mesh) {
			u32 path_length = to_string_length(source);
			u32 path_id     = Asset::store_string(*source, path_length);
			component->mesh = Asset::add<Mesh_Asset>(path_id);
			continue;
		}

		if (key_id == key_layer) { component->layer = (u8)to_u32(source); continue; }

		*source = key; break;
	}
}

}}

//
// Lua_Script
//

namespace custom {
namespace serialization {

template<> READ_FUNC(component_pool_read<Lua_Script>) {
	RefT<Lua_Script> & refT = (RefT<Lua_Script> &)ref;

	static u32 const key_asset  = Entity::store_string("asset",  custom::empty_index);
	static u32 const key_update = Entity::store_string("update", custom::empty_index);

	Lua_Script * component = refT.get_fast();

	CUSTOM_ASSERT(strncmp_auto(*source, "Lua_Script") == 0, "");
	while ((to_next_line(source), **source)) {

		parse_void(source);
		if (**source == '#') { continue; }
		if (!IS_VALID_IDENTIFIER_START(**source)) { break; }

		u32 key_length = to_identifier_length(source);
		cstring key    = *source; skip_to_void(source);
		u32 key_id     = Entity::get_id(key, key_length);
		if (key_id == custom::empty_index) { *source = key; break; }

		if (key_id == key_asset) {
			u32 path_length = to_string_length(source);
			u32 path_id     = Asset::store_string(*source, path_length);
			Asset::add<Lua_Asset>(path_id);
			continue;
		}

		if (key_id == key_update) {
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

template<> READ_FUNC(component_pool_read<Physical>) {
	RefT<Physical> & refT = (RefT<Physical> &)ref;

	Physical * component = refT.get_fast();

	CUSTOM_ASSERT(strncmp_auto(*source, "Physical") == 0, "");
	while ((to_next_line(source), **source)) {

		parse_void(source);
		if (**source == '#') { continue; }
		if (!IS_VALID_IDENTIFIER_START(**source)) { break; }

		u32 key_length = to_identifier_length(source);
		cstring key    = *source; skip_to_void(source);
		u32 key_id     = Entity::get_id(key, key_length);
		if (key_id == custom::empty_index) { *source = key; break; }

		*source = key; break;
	}
}

}}

//
// Phys2d
//

namespace custom {
namespace serialization {

template<> READ_FUNC(component_pool_read<Phys2d>) {
	RefT<Phys2d> & refT = (RefT<Phys2d> &)ref;

	static u32 const key_movable    = Entity::store_string("movable",    custom::empty_index);
	static u32 const key_rotatable  = Entity::store_string("rotatable",  custom::empty_index);
	static u32 const key_mass       = Entity::store_string("mass",       custom::empty_index);
	static u32 const key_elasticity = Entity::store_string("elasticity", custom::empty_index);
	static u32 const key_roughness  = Entity::store_string("roughness",  custom::empty_index);
	static u32 const key_stickiness = Entity::store_string("stickiness", custom::empty_index);
	static u32 const key_stillness  = Entity::store_string("stillness",  custom::empty_index);
	static u32 const key_shape      = Entity::store_string("shape",      custom::empty_index);
	static u32 const key_collider   = Entity::store_string("collider",   custom::empty_index);

	Phys2d * component = refT.get_fast();

	CUSTOM_ASSERT(strncmp_auto(*source, "Phys2d") == 0, "");
	while ((to_next_line(source), **source)) {

		parse_void(source);
		if (**source == '#') { continue; }
		if (!IS_VALID_IDENTIFIER_START(**source)) { break; }

		u32 key_length = to_identifier_length(source);
		cstring key    = *source; skip_to_void(source);
		u32 key_id     = Entity::get_id(key, key_length);
		if (key_id == custom::empty_index) { *source = key; break; }

		if (key_id == key_movable)    { component->movable    = to_r32(source); continue; }
		if (key_id == key_rotatable)  { component->rotatable  = to_r32(source); continue; }
		if (key_id == key_mass)       { component->mass       = to_r32(source); continue; }
		if (key_id == key_elasticity) { component->elasticity = to_r32(source); continue; }
		if (key_id == key_roughness)  { component->roughness  = to_r32(source); continue; }
		if (key_id == key_stickiness) { component->stickiness = to_r32(source); continue; }
		if (key_id == key_stillness)  { component->stillness  = to_r32(source); continue; }
		if (key_id == key_shape)      { component->shape      = to_r32(source); continue; }

		if (key_id == key_collider) {
			u32 path_length = to_string_length(source);
			u32 path_id     = Asset::store_string(*source, path_length);
			component->mesh = Asset::add<Collider2d_Asset>(path_id);
			continue;
		}

		*source = key; break;
	}
}

}}
