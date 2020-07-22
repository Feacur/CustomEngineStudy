#include "custom_pch.h"

#include "engine/impl/entity_system.h"

namespace custom {

// @Note: instantiate static data
Ref_Pool<Entity> Entity::pool;
Array<Plain_Ref> Entity::entities;
Array<Plain_Ref> Entity::components;
Array<void_ref_func *> Entity::component_destructors;

//
//
//

Ref<Entity> Entity::create() {
	auto entity = Entity::pool.create();
	entities.push({entity.id, entity.gen});
	return entity;
}

void Entity::destroy(Ref<Entity> ref) {
	u32 index = UINT32_MAX;
	for (u32 i = 0; i < entities.count; ++i) {
		if (entities[i].id != ref.id) { continue; }
		if (entities[i].gen != ref.gen) { continue; }
		index = i; break;
	}
	CUSTOM_ASSERT(index < entities.count, "no entity %d:%d", ref.id, ref.gen);
	entities.remove_at(index);

	u32 entity_offset = ref.id * Entity::component_destructors.count;
	for (u32 i = 0; i < Entity::component_destructors.count; ++i) {
		Plain_Ref & c_ref = Entity::components.get(entity_offset + i);
		(*Entity::component_destructors[i])(c_ref.id, c_ref.gen);
	}
	Entity::pool.destroy(ref);
}

}
