#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/api/internal/component_types.h"
#include "engine/api/internal/entity_system.h"
#include "engine/impl/array.h"

//
//
//

template struct custom::Array<Hierarchy::Link>;
custom::Array<Hierarchy::Link> Hierarchy::links;

void Hierarchy::set_parent(custom::Entity & child, custom::Entity const & entity) {
	Hierarchy::links.push({entity.id, child});

	custom::RefT<Hierarchy> hierarchy_refT = child.add_component<Hierarchy>();
	Hierarchy * hierarchy = hierarchy_refT.get_fast();
	hierarchy->parent = entity;
}
