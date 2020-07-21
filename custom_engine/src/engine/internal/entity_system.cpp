#include "custom_pch.h"

#include "engine/impl/entity_system.h"

namespace custom {

// @Note: instantiate static data
Ref_Pool<Entity> Entity::pool;
Array<Plain_Ref> Entity::entities;
Array<Plain_Ref> Entity::components;
Array<Ref_Pool_Base *> Entity::component_pools;

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
		if (entities[i].id == ref.id) { index = i; break; }
	}
	CUSTOM_ASSERT(index < entities.count, "no entity %d:%d", ref.id, ref.gen);
	u32 entity_offset = ref.id * Entity::component_pools.count;
	for (u32 i = 0; i < Entity::component_pools.count; ++i) {
		Plain_Ref & c_ref = Entity::components[entity_offset + i];
		Entity::component_pools[i]->destroy_safe(c_ref.id, c_ref.gen);
	}
	Entity::pool.destroy(ref);
	entities.remove_at(index);
}

}
