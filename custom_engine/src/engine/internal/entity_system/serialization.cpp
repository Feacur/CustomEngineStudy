#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/api/internal/parsing.h"
#include "engine/api/internal/component_types.h"
#include "engine/api/internal/asset_types.h"
#include "engine/impl/entity_system.h"
#include "engine/impl/asset_system.h"
#include "engine/impl/math_linear.h"

#include <new>

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

			case 'p': ++(*source); {
				cstring line_end = (parse_void(source), *source); skip_to_eol(&line_end);
				u32 id = Asset::store_string(*source, (u32)(line_end - *source));
				Asset_RefT<Prefab_Asset> prefab_asset = Asset::add<Prefab_Asset>(id);

				custom::Entity const prefab = *prefab_asset.ref.get_fast();
				entity.override(prefab);
			} break;

			case 'o': ++(*source); {
				entity.override(source);
			} break;

			case '#': break;
			default: done = true; break;
		}
	}
}

void serialization_read_Child_block(Entity & entity, cstring * source) {
	bool done = false;
	while (!done && *source) {
		skip_to_eol(source); parse_eol(source);
		switch ((parse_void(source), **source)) {
			case '!': ++(*source); {
				Entity child = Entity::serialization_read(source);

				RefT<Hierarchy> hierarchy_refT = child.add_component<Hierarchy>();
				Hierarchy * hierarchy = hierarchy_refT.get_fast();
				hierarchy->parent = entity;
			} break;

			case 'p': ++(*source); {
				cstring line_end = (parse_void(source), *source); skip_to_eol(&line_end);
				u32 id = Asset::store_string(*source, (u32)(line_end - *source));
				Asset_RefT<Prefab_Asset> prefab_asset = Asset::add<Prefab_Asset>(id);

				Entity child = prefab_asset.ref.get_fast()->copy(entity.is_instance);

				RefT<Hierarchy> hierarchy_refT = child.add_component<Hierarchy>();
				Hierarchy * hierarchy = hierarchy_refT.get_fast();
				hierarchy->parent = entity;
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
	// new (component) Transform;

	bool done = false;
	while (!done && **source) {
		skip_to_eol(source); parse_eol(source);
		switch ((parse_void(source), **source)) {
			case 'p': ++(*source); {
				component->position = (parse_void(source), parse_vec3(source));
			} break;

			case 'r': ++(*source); switch (**source) {
				case ' ': component->rotation = (parse_void(source), parse_vec4(source)); break;
				case 'r': component->rotation = quat_from_radians((parse_void(source), parse_vec3(source))); break;
				case 'd': component->rotation = quat_from_radians((parse_void(source), parse_vec3(source)) * deg_to_rad); break;
			} break;

			case 's': ++(*source); {
				component->scale = (parse_void(source), parse_vec3(source));
			} break;

			case '#': break;
			default: done = true; break;
		}
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
	// new (component) Camera;

	bool done = false;
	while (!done && **source) {
		skip_to_eol(source); parse_eol(source);
		switch ((parse_void(source), **source)) {
			case 'n': ++(*source); {
				component->near = (parse_void(source), parse_r32(source));
			} break;

			case 'f': ++(*source); {
				component->far = (parse_void(source), parse_r32(source));
			} break;

			case 's': ++(*source); {
				component->scale = (parse_void(source), parse_r32(source));
			} break;

			case 'o': ++(*source); {
				component->ortho = (parse_void(source), parse_r32(source));
			} break;

			case 'c': ++(*source); {
				// @Todo: parse enums
				component->clear = (graphics::Clear_Flag)(parse_void(source), parse_u32(source));
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
// Hierarchy
//

namespace custom {
namespace serialization {

template<> SERIALIZATION_READ_FUNC(component_pool_serialization_read<Hierarchy>) {
	RefT<Hierarchy> & refT = (RefT<Hierarchy> &)ref;

	Hierarchy * component = refT.get_fast();
	new (component) Hierarchy;

	// @Todo: read direct parent reference

	bool done = false;
	while (!done && **source) {
		skip_to_eol(source); parse_eol(source);
		switch ((parse_void(source), **source)) {
			case 'e': ++(*source); {
				CUSTOM_TRACE("> inline prefab");

				// @Note: can potentially reallocate memory; ping ref pool once more afterwards
				Entity child = Entity::serialization_read(source);
				component = refT.get_fast();

				// component->link(entity, child);
				// component = refT.get_fast();
			} break;

			case 'p': ++(*source); {
				cstring line_end = (parse_void(source), *source); skip_to_eol(&line_end);
				u32 id = Asset::store_string(*source, (u32)(line_end - *source));
				CUSTOM_TRACE("> nested prefab: '%s'", Asset::get_string(id));
				Asset_RefT<Prefab_Asset> prefab = Asset::add<Prefab_Asset>(id);

				// @Note: can potentially reallocate memory; ping ref pool once more afterwards
				Entity child = prefab.ref.get_fast()->copy(false);
				component = refT.get_fast();

				// @Note: can potentially reallocate memory; ping ref pool once more afterwards
				// component->link(entity, child);
				// component = refT.get_fast();
			} break;

			case 'o': ++(*source); {
				// Entity child = component->children[component->children.count - 1];

				// @Note: can potentially reallocate memory; ping ref pool once more afterwards
				// child.override(source);
				// component = refT.get_fast();
			} break;

			case '#': break;
			default: done = true; break;
		}
	}
}

}}

	// custom::RefT<Hierarchy>   child_hierarchy_ref = child.get_component<Hierarchy>();
	// Hierarchy               * child_hierarchy     = child_hierarchy_ref.get_safe();
	// if (!child_hierarchy) {
	// 	child_hierarchy_ref = child.add_component<Hierarchy>();
	// 	child_hierarchy     = child_hierarchy_ref.get_safe();
	// 	new (child_hierarchy) Hierarchy;
	// }

	// child_hierarchy->parent = entity;