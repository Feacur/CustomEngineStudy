#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/api/internal/component_types.h"
#include "engine/api/internal/entity_system.h"
#include "engine/impl/array.h"

//
// Hierarchy
//

template struct custom::Array<Hierarchy::Link>;
custom::Array<Hierarchy::Link> Hierarchy::links;

void Hierarchy::fetch_children(custom::Entity const & entity, custom::Array<Hierarchy::Link> & buffer) {
	for (u32 i = 0; i < Hierarchy::links.count; ++i) {
		if (Hierarchy::links[i].id != entity.id) { continue; }
		buffer.push({i, Hierarchy::links[i].entity});
	}
}

void Hierarchy::set_parent(custom::Entity & child, custom::Entity const & entity) {
	Hierarchy::links.push({entity.id, child});

	custom::RefT<Hierarchy> hierarchy_refT = child.add_component<Hierarchy>();
	Hierarchy * hierarchy = hierarchy_refT.get_fast();
	hierarchy->parent = entity;
}

void Hierarchy::rem_parent(custom::Entity & child, custom::Entity const & entity) {
	for (u32 i = 0; i < Hierarchy::links.count; ++i) {
		if (Hierarchy::links[i].id != entity.id) { continue; }
		if (Hierarchy::links[i].entity != child) { continue; }
		Hierarchy::links.remove_at(i);
		break;
	}
}

void Hierarchy::remove_at(u32 id) {
	Hierarchy::links.remove_at(id);
}
