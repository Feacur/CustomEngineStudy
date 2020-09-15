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

void serialization_read_Entity_block(Entity & entity, cstring * source) {
	bool done = false;
	while (!done && *source) {
		skip_to_eol(source); parse_eol(source);
		switch ((parse_void(source), **source)) {
			case 'i': ++(*source); {
				bool is_instance = (bool)(parse_void(source), parse_u32(source));
				if (is_instance) { entity.promote_to_instance(); }
			} break;

			case 'o': ++(*source); {
				entity.serialization_read(source);
			} break;

			case 'p': ++(*source); {
				cstring line_end = (parse_void(source), *source); skip_to_eol(&line_end);
				u32 id = Asset::store_string(*source, (u32)(line_end - *source));
				Asset_RefT<Prefab_Asset> prefab_asset_ref = Asset::add<Prefab_Asset>(id);

				Prefab_Asset * prefab_asset = prefab_asset_ref.ref.get_fast();
				Entity source_entity = prefab_asset->entity;

				entity.override_with(source_entity);
			} break;

			case '#': break;
			default: done = true; break;
		}
	}
}

void serialization_read_Child_block(Entity & entity, cstring * source) {
	Entity last_child = {custom::empty_ref};

	bool is_instance = entity.is_instance();

	bool done = false;
	while (!done && *source) {
		skip_to_eol(source); parse_eol(source);
		switch ((parse_void(source), **source)) {
			case '!': ++(*source); {
				Entity child = Entity::create(is_instance);
				child.serialization_read(source);
				Hierarchy::set_parent(child, entity);
				last_child = child;
			} break;

			case 'p': ++(*source); {
				cstring line_end = (parse_void(source), *source); skip_to_eol(&line_end);
				u32 id = Asset::store_string(*source, (u32)(line_end - *source));
				Asset_RefT<Prefab_Asset> prefab_asset_ref = Asset::add<Prefab_Asset>(id);

				Prefab_Asset * prefab_asset = prefab_asset_ref.ref.get_fast();
				Entity child = prefab_asset->entity.copy(is_instance);

				Hierarchy::set_parent(child, entity);
				last_child = child;
			} break;

			case 'o': ++(*source); {
				Entity child = last_child;
				child.serialization_read(source);
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

template<> SERIALIZATION_READ_FUNC(component_pool_serialization_read<Transform>) {
	RefT<Transform> & refT = (RefT<Transform> &)ref;

	Transform * component = refT.get_fast();

	while (**source) {
		skip_to_eol(source); parse_eol(source);

		parse_void(source);
		if (**source == '#') { continue; }

		if (strncmp_auto(*source, "position ") == 0) { component->position = to_vec3(source); continue; }
		if (strncmp_auto(*source, "scale ")    == 0) { component->scale    = to_vec3(source); continue; }
		if (strncmp_auto(*source, "rotation ") == 0) { component->rotation = to_vec4(source); continue; }
		if (strncmp_auto(*source, "radians ")  == 0) { component->rotation = quat_from_radians(to_vec3(source)); continue; }
		if (strncmp_auto(*source, "degrees ")  == 0) { component->rotation = quat_from_radians(to_vec3(source) * deg_to_rad); continue; }

		break;
	}
}

}}

//
// Camera
//

namespace custom {
namespace serialization {

template<> SERIALIZATION_READ_FUNC(component_pool_serialization_read<Camera>) {
	RefT<Camera> & refT = (RefT<Camera> &)ref;

	Camera * component = refT.get_fast();

	while (**source) {
		skip_to_eol(source); parse_eol(source);

		parse_void(source);
		if (**source == '#') { continue; }

		// @Todo: parse enums
		if (strncmp_auto(*source, "near ")  == 0) { component->ncp   = to_r32(source); continue; }
		if (strncmp_auto(*source, "far ")   == 0) { component->fcp   = to_r32(source); continue; }
		if (strncmp_auto(*source, "scale ") == 0) { component->scale = to_r32(source); continue; }
		if (strncmp_auto(*source, "ortho ") == 0) { component->ortho = to_r32(source); continue; }
		if (strncmp_auto(*source, "clear ") == 0) { component->clear = (graphics::Clear_Flag)to_u32(source); continue; }
		if (strncmp_auto(*source, "layer ") == 0) { component->layer = (u8)to_u32(source); continue; }

		break;
	}
}

}}

//
// Hierarchy
//

namespace custom {
namespace serialization {

template<> SERIALIZATION_READ_FUNC(component_pool_serialization_read<Hierarchy>) {
	RefT<Hierarchy> & refT = (RefT<Hierarchy> &)ref;

	Hierarchy * component = refT.get_fast();
	component->parent = {custom::empty_ref};

	// @Todo: read direct parent reference
	CUSTOM_ASSERT(false, "is not implemented");
}

}}
