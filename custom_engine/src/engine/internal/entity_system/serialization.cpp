#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/api/internal/parsing.h"
#include "engine/api/internal/component_types.h"
#include "engine/api/internal/asset_types.h"
#include "engine/impl/entity_system.h"
#include "engine/impl/asset_system.h"
#include "engine/impl/math_linear.h"
#include "engine/impl/parsing.h"

//
// Entity
//

namespace custom {
namespace serialization {

void read_Entity_block(Entity & entity, cstring * source) {
	bool done = false;

	CUSTOM_ASSERT(**source == '!', "");
	while (!done && *source) {
		to_next_line(source);
		switch ((parse_void(source), **source)) {
			case 'i': ++(*source); {
				bool is_instance = (bool)(parse_void(source), parse_u32(source));
				if (is_instance) { entity.promote_to_instance(); }
			} break;

			case 'o': ++(*source); {
				entity.read(source);
			} break;

			case 'p': ++(*source); {
				u32 path_length = to_string_length(source);
				u32 path_id     = Asset::store_string(*source, path_length);
				Asset_RefT<Prefab_Asset> prefab_asset_ref = Asset::add<Prefab_Asset>(path_id);

				Prefab_Asset * prefab_asset = prefab_asset_ref.ref.get_fast();
				Entity source_entity = prefab_asset->entity;

				entity.override_with(source_entity);
			} break;

			case '#': break;
			default: done = true; break;
		}
	}
}

void read_Child_block(Entity & entity, cstring * source) {
	Entity last_child = {custom::empty_ref};

	bool is_instance = entity.is_instance();

	bool done = false;

	CUSTOM_ASSERT(**source == '>', "");
	while (!done && *source) {
		to_next_line(source);
		switch ((parse_void(source), **source)) {
			case '!': ++(*source); {
				Entity child = Entity::create(is_instance);
				child.read(source);
				Hierarchy::set_parent(child, entity);
				last_child = child;
			} break;

			case 'p': ++(*source); {
				u32 path_length = to_string_length(source);
				u32 path_id     = Asset::store_string(*source, path_length);
				Asset_RefT<Prefab_Asset> prefab_asset_ref = Asset::add<Prefab_Asset>(path_id);

				Prefab_Asset * prefab_asset = prefab_asset_ref.ref.get_fast();
				Entity child = prefab_asset->entity.copy(is_instance);

				Hierarchy::set_parent(child, entity);
				last_child = child;
			} break;

			case 'o': ++(*source); {
				Entity child = last_child;
				child.read(source);
			} break;

			case '#': break;
			default: done = true; break;
		}
	}
}

}}

//
// Transform
//

namespace custom {
namespace serialization {

template<> READ_FUNC(component_pool_read<Transform>) {
	RefT<Transform> & refT = (RefT<Transform> &)ref;

	static u32 const key_position = Entity::store_string("position", custom::empty_index);
	static u32 const key_scale    = Entity::store_string("scale",    custom::empty_index);
	static u32 const key_rotation = Entity::store_string("rotation", custom::empty_index);
	static u32 const key_radians  = Entity::store_string("radians",  custom::empty_index);
	static u32 const key_degrees  = Entity::store_string("degrees",  custom::empty_index);

	Transform * component = refT.get_fast();

	CUSTOM_ASSERT(strncmp_auto(*source, "Transform") == 0, "");
	while ((to_next_line(source), **source)) {

		parse_void(source);
		if (**source == '#') { continue; }
		if (!IS_VALID_IDENTIFIER_START(**source)) { break; }

		u32 key_length = to_identifier_length(source);
		cstring key    = *source; skip_to_void(source);
		u32 key_id     = Entity::get_id(key, key_length);
		if (key_id == custom::empty_index) { *source = key; break; }

		if (key_id == key_position) { component->position = to_vec3(source); continue; }
		if (key_id == key_scale)    { component->scale    = to_vec3(source); continue; }
		if (key_id == key_rotation) { component->rotation = to_vec4(source); continue; }
		if (key_id == key_radians)  { component->rotation = quat_from_radians(to_vec3(source)); continue; }
		if (key_id == key_degrees)  { component->rotation = quat_from_radians(to_vec3(source) * deg_to_rad); continue; }

		*source = key; break;
	}
}

}}

//
// Camera
//

namespace custom {
namespace serialization {

template<> READ_FUNC(component_pool_read<Camera>) {
	RefT<Camera> & refT = (RefT<Camera> &)ref;

	static u32 const key_near  = Entity::store_string("near",  custom::empty_index);
	static u32 const key_far   = Entity::store_string("far",   custom::empty_index);
	static u32 const key_scale = Entity::store_string("scale", custom::empty_index);
	static u32 const key_ortho = Entity::store_string("ortho", custom::empty_index);
	static u32 const key_clear = Entity::store_string("clear", custom::empty_index);
	static u32 const key_layer = Entity::store_string("layer", custom::empty_index);

	Camera * component = refT.get_fast();

	CUSTOM_ASSERT(strncmp_auto(*source, "Camera") == 0, "");
	while ((to_next_line(source), **source)) {
		parse_void(source);
		if (**source == '#') { continue; }
		if (!IS_VALID_IDENTIFIER_START(**source)) { break; }

		u32 key_length = to_identifier_length(source);
		cstring key    = *source; skip_to_void(source);
		u32 key_id     = Entity::get_id(key, key_length);
		if (key_id == custom::empty_index) { *source = key; break; }

		if (key_id == key_near)  { component->ncp   = to_r32(source); continue; }
		if (key_id == key_far)   { component->fcp   = to_r32(source); continue; }
		if (key_id == key_scale) { component->scale = to_r32(source); continue; }
		if (key_id == key_ortho) { component->ortho = to_r32(source); continue; }
		if (key_id == key_clear) { component->clear = (graphics::Clear_Flag)to_u32(source); continue; }
		if (key_id == key_layer) { component->layer = (u8)to_u32(source); continue; }

		*source = key; break;
	}
}

}}

//
// Hierarchy
//

namespace custom {
namespace serialization {

template<> READ_FUNC(component_pool_read<Hierarchy>) {
	RefT<Hierarchy> & refT = (RefT<Hierarchy> &)ref;

	Hierarchy * component = refT.get_fast();
	component->parent = {custom::empty_ref};

	// @Todo: read direct parent reference
	CUSTOM_ASSERT(false, "is not implemented");
}

}}
