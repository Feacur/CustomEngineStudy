#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/api/internal/component_types.h"
#include "engine/api/internal/entity_system.h"

#include <new>

//
// Hierarchy
//

// @Todo: this looks error-prone; needs more design
void Hierarchy::link(custom::Entity & entity, custom::Entity & child) {
	children.push(child);

	custom::RefT<Hierarchy>   child_hierarchy_ref = child.get_component<Hierarchy>();
	Hierarchy               * child_hierarchy     = child_hierarchy_ref.get_safe();
	if (!child_hierarchy) {
		child_hierarchy_ref = child.add_component<Hierarchy>();
		child_hierarchy     = child_hierarchy_ref.get_safe();
		new (child_hierarchy) Hierarchy;
	}

	child_hierarchy->parent = entity;
}
