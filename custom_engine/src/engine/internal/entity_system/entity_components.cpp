#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/api/internal/component_types.h"
#include "engine/impl/entity_system.h"

//
// Entity
//

namespace custom {

void entity_do_after_copy(Entity const & from, Entity & to, bool force_instance) {
	// @Todo: estimate if capacity reservation is better here
	Array<Hierarchy::Link> children;
	Hierarchy::fetch_children(from, children);

	for (u32 i = 0; i < children.count; ++i) {
		Entity child = children[i].entity.copy(force_instance);
		Hierarchy::set_parent(child, to);
	}
}

void entity_do_before_destroy(Entity & entity) {
	// @Todo: estimate if capacity reservation is better here
	Array<Hierarchy::Link> children;
	Hierarchy::fetch_children(entity, children);

	// @Note: might be passed to `component_pool_unload_Hierarchy`,
	//        but now for now it's batch-processed here;
	//        it's not pretty, but for this purpose unloaders
	//        recieve a flag `only_component`, which tells, if a component
	//        is being removed alone or as a part of entity destruction
	for (u32 i = children.count; i > 0; --i) {
		Hierarchy::remove_at(children[i - 1].id);
	}

	for (u32 i = 0; i < children.count; ++i) {
		if (!children[i].entity.exists()) { continue; }
		children[i].entity.destroy();
	}
}

void entity_do_before_reset_system(void) {
	// @Note: do not traverse hierarchy upon state reset
	Hierarchy::links.count = 0;
}

}

//
// Transform
//

namespace custom {

ENTITY_FROM_TO_FUNC(component_pool_copy_Transform) {
	RefT<Transform> const & fromT = (RefT<Transform> const &)from;
	RefT<Transform> & toT = (RefT<Transform> &)to;

	*toT.get_fast() = *fromT.get_fast();
}

ENTITY_LOADING_FUNC(component_pool_load_Transform) {
	RefT<Transform> & refT = (RefT<Transform> &)ref;
	Transform * component = refT.get_fast();

	component->position = {0, 0, 0};
	component->scale    = {1, 1, 1};
	component->rotation = {0, 0, 0, 1};
}

ENTITY_LOADING_FUNC(component_pool_unload_Transform) {
	// RefT<Transform> & refT = (RefT<Transform> &)ref;
	// Transform * component = refT.get_fast();
}

}

//
// Camera
//

namespace custom {

ENTITY_FROM_TO_FUNC(component_pool_copy_Camera) {
	RefT<Camera> const & fromT = (RefT<Camera> const &)from;
	RefT<Camera> & toT = (RefT<Camera> &)to;

	*toT.get_fast() = *fromT.get_fast();
}

ENTITY_LOADING_FUNC(component_pool_load_Camera) {
	RefT<Camera> & refT = (RefT<Camera> &)ref;
	Camera * component = refT.get_fast();

	component->ncp  = 0.1f;
	component->fcp   = 100;
	component->scale = 1;
	component->ortho = 0;
	component->clear = Camera::Clear_Flag::Color | Camera::Clear_Flag::Depth;
	component->layer = 0;
}

ENTITY_LOADING_FUNC(component_pool_unload_Camera) {
	// RefT<Camera> & refT = (RefT<Camera> &)ref;
	// Camera * component = refT.get_fast();
}

}

//
// Hierarchy
//

namespace custom {

ENTITY_FROM_TO_FUNC(component_pool_copy_Hierarchy) {
	/*pass the responsibility to `entity_do_after_copy`*/
	RefT<Hierarchy> & refT = (RefT<Hierarchy> &)to;
	Hierarchy * component = refT.get_fast();

	// @Todo: does this need an implementation whatsoever?
	//        for now, just drop the parent reference.
	//        - removing the component is possible, but isn't necessary
	//        - might keep it in case it's an instance
	//        - ... but prefab is a no go here definitely
	component->parent = {custom::empty_ref};
}

ENTITY_LOADING_FUNC(component_pool_load_Hierarchy) {
	RefT<Hierarchy> & refT = (RefT<Hierarchy> &)ref;
	Hierarchy * component = refT.get_fast();

	component->parent = {custom::empty_ref};
}

ENTITY_LOADING_FUNC(component_pool_unload_Hierarchy) {
	if (!only_component) { /*pass the responsibility to `entity_do_before_destroy`*/ return; }
	RefT<Hierarchy> & refT = (RefT<Hierarchy> &)ref;
	Hierarchy * component = refT.get_fast();

	Hierarchy::rem_parent(entity, component->parent);
}

}
