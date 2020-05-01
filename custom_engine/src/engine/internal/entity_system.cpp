#include "custom_pch.h"

#include "engine/impl/entity_system.h"

namespace custom {

// @Note: instantiate static data
Ref_Pool<Entity> Entity::pool;
Array<Plain_Ref> Entity::components;
Array<Ref_Pool_Base *> Entity::component_pools;

//
//
//

Ref<Entity> Entity::create() {
	return Entity::pool.create();
}

void Entity::destroy(Ref<Entity> ref) {
	u32 entity_offset = ref.id * Entity::component_pools.count;
	for (u32 i = 0; i < Entity::component_pools.count; ++i) {
		Plain_Ref & c_ref = Entity::components[entity_offset + i];
		Entity::component_pools[i]->destroy_safe(c_ref.id, c_ref.gen);
	}
	Entity::pool.destroy(ref);
}

}
