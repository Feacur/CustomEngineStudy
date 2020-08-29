#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/api/internal/component_types.h"
#include "engine/impl/entity_system.h"

#include <new>

//
// Entity
//

namespace custom {

void entity_do_after_copy(Entity const & entity, bool force_instance) {
	Array<Hierarchy::Link> children;
	Hierarchy::fetch_children(entity, children);

	for (u32 i = 0; i < children.count; ++i) {
		Entity child = children[i].entity.copy(force_instance);
		Hierarchy::set_parent(child, entity);
	}
}

void entity_do_before_destroy(Entity & entity) {
	Array<Hierarchy::Link> children;
	Hierarchy::fetch_children(entity, children);

	for (u32 i = 0; i < children.count; ++i) {
		Hierarchy::remove_at(children[i].id);
	}

	for (u32 i = 0; i < children.count; ++i) {
		children[i].entity.destroy();
	}
}

}

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

	// @Todo: reparenting
	*toT.get_fast() = *fromT.get_fast();
}

template<> ENTITY_LOADING_FUNC(component_pool_clean<Hierarchy>) {
	RefT<Hierarchy> & refT = (RefT<Hierarchy> &)ref;

	Hierarchy * component = refT.get_fast();
	component->parent = {custom::empty_ref};

	// @Todo: destroy child entities
}

}
