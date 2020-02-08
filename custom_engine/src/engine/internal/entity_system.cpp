#include "custom_pch.h"

#include "engine/entity_system/world.h"

namespace custom {

Ref_Pool<Entity> Entity::pool;
u32 Entity::component_types_count;
Array<Plain_Ref> Entity::components;

Array<Ref_Pool_Base *> World::component_pools;

Ref<Entity> World::create() {
	return Entity::pool.create();
}

void World::destroy(Ref<Entity> ref) {
	u32 entity_offset = ref.id * Entity::component_types_count;
	for (u32 i = 0; i < component_pools.count; ++i) {
		Plain_Ref & c_ref = Entity::components[entity_offset + i];
		if (c_ref.id) {
			component_pools[i]->destroy(c_ref.id, c_ref.gen);
		}
	}
	Entity::pool.destroy(ref);
}

}
