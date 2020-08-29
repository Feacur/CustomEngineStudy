#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/api/internal/component_types.h"
#include "engine/impl/entity_system.h"

#include <new>

//
// Transform
//

namespace custom {

template<> ENTITY_FROM_TO_FUNC(component_pool_copy<Transform>) {
	RefT<Transform> const & fromT = (RefT<Transform> const &)from;
	RefT<Transform> & toT = (RefT<Transform> &)to;

	*toT.get_fast() = *fromT.get_fast();
}

template<> ENTITY_LOADING_FUNC(component_pool_clean<Transform>) {
	// RefT<Transform> & refT = (RefT<Transform> &)ref;
}

}

//
// Camera
//

namespace custom {

template<> ENTITY_FROM_TO_FUNC(component_pool_copy<Camera>) {
	RefT<Camera> const & fromT = (RefT<Camera> const &)from;
	RefT<Camera> & toT = (RefT<Camera> &)to;

	*toT.get_fast() = *fromT.get_fast();
}

template<> ENTITY_LOADING_FUNC(component_pool_clean<Camera>) {
	// RefT<Camera> & refT = (RefT<Camera> &)ref;
}

}

//
// Hierarchy
//

namespace custom {

template<> ENTITY_FROM_TO_FUNC(component_pool_copy<Hierarchy>) {
	RefT<Hierarchy> const & fromT = (RefT<Hierarchy> const &)from;
	RefT<Hierarchy> & toT = (RefT<Hierarchy> &)to;

	Hierarchy const * from_component = fromT.get_fast();
	Hierarchy * to_component = toT.get_fast();

	new (to_component) Hierarchy;

	for (u32 i = 0; i < from_component->children.count; ++i) {
		to_component->children.push(from_component->children[i]);
	}

	for (u32 i = 0; i < to_component->children.count; ++i) {
		custom::Entity child = to_component->children[i];
		// @Note: can potentially reallocate memory; ping ref pool once more afterwards
		child = child.copy(entity.is_instance);
		to_component = toT.get_fast();
		to_component->children[i] = child;
	}

	for (u32 i = 0; i < to_component->children.count; ++i) {
		custom::Entity    child               = to_component->children[i];
		RefT<Hierarchy>   child_hierarchy_ref = child.get_component<Hierarchy>();
		Hierarchy       * child_hierarchy     = child_hierarchy_ref.get_fast();
		child_hierarchy->parent = entity;
	}
}

template<> ENTITY_LOADING_FUNC(component_pool_clean<Hierarchy>) {
	RefT<Hierarchy> & refT = (RefT<Hierarchy> &)ref;

	Hierarchy * component = refT.get_fast();

	if (component->parent.exists()) {
		RefT<Hierarchy>   parent_hierarchy_ref = component->parent.get_component<Hierarchy>();
		Hierarchy       * parent_hierarchy     = parent_hierarchy_ref.get_fast();
		for (u32 i = 0; i < parent_hierarchy->children.count; ++i) {
			if (parent_hierarchy->children[i].ref == entity.ref) {
				parent_hierarchy->children.remove_at(i);
				break;
			}
		}
	}

	if (entity_will_be_destroyed) {
		for (u32 i = 0; i < component->children.count; ++i) {
			// @Note: can potentially reallocate memory; ping ref pool once more afterwards
			component->children[i].destroy();
			component = refT.get_fast();
		}
	}
	else {
		for (u32 i = 0; i < component->children.count; ++i) {
			RefT<Hierarchy>   child_hierarchy_ref = component->children[i].get_component<Hierarchy>();
			Hierarchy       * child_hierarchy     = child_hierarchy_ref.get_fast();
			child_hierarchy->parent = {custom::empty_ref, false};
		}
	}

	component = refT.get_fast();
	component->parent = {custom::empty_ref, false};
	component->children.~Array();
}

}
