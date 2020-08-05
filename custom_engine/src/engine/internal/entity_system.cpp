#include "custom_pch.h"

#include "engine/impl/entity_system.h"

namespace custom {

// @Note: instantiate static data
Gen_Pool               Entity::generations;
Array<Entity>          Entity::instances;
Array<void_ref_func *> Entity::component_destructors;
Array<Ref>             Entity::components;

//
// pool
//

Ref Gen_Pool::create(void) {
	u32 id;
	if (gaps.count > 0) {
		id = gaps[gaps.count - 1];
		gaps.pop();
	}
	else {
		id = gens.count;
		gens.push(0);
	}
	return { id, gens.get(id) };
}

void Gen_Pool::destroy(Ref const & ref) {
	// @Change: ignore, but warn, if data doesn't exist?
	CUSTOM_ASSERT(ref.gen == gens.get(ref.id), "destroying null data");
	++gens[ref.id];
	if (ref.id == gens.count - 1) {
		gens.pop();
	}
	else {
		gaps.push(ref.id);
	}
}

//
//
//

Entity Entity::create(void) {
	Ref ref = Entity::generations.create();
	instances.push({ref.id, ref.gen});
	return {ref.id, ref.gen};
}

void Entity::destroy(Entity const & entity) {
	CUSTOM_ASSERT(entity.exists(), "entity doesn't exist");

	u32 entity_offset = entity.id * Entity::component_destructors.count;
	for (u32 i = 0; i < Entity::component_destructors.count; ++i) {
		Ref const & ref = Entity::components.get(entity_offset + i);
		(*Entity::component_destructors[i])(ref);
	}
	Entity::generations.destroy(entity);

	u32 instance_index = UINT32_MAX;
	for (u32 i = 0; i < instances.count; ++i) {
		if (instances[i].id != entity.id) { continue; }
		if (instances[i].gen != entity.gen) { continue; }
		instance_index = i; break;
	}

	if(instance_index < instances.count) {
		instances.remove_at(instance_index);
	}
	else {
		CUSTOM_WARNING("entity instance doesn't exist");
	}
}

}
