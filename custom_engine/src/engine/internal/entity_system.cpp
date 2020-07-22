#include "custom_pch.h"

#include "engine/impl/entity_system.h"

namespace custom {

// @Note: instantiate static data
Gen_Pool Entity::pool;
Array<Entity> Entity::instances;
Array<Ref> Entity::components;
Array<void_ref_func *> Entity::component_destructors;

//
//
//


//
// pool
//

Gen_Pool::Gen_Pool() {
	// @Note: it's required for the current implementation where 0 is a default and invalid id
	Ref null_instance = create();
}

Ref Gen_Pool::create() {
	u32 id;
	if (gaps.count > 0) {
		id = gaps[gaps.count - 1];
		gaps.pop();
	}
	else {
		id = gens.count;
		gens.push();
	}
	return { id, gens.get(id) };
}

void Gen_Pool::destroy(Ref const & ref) {
	CUSTOM_ASSERT(ref.gen == gens.get(ref.id), "destroying null data");
	if (ref.id == gens.count - 1) {
		gens.pop();
	}
	else {
		gaps.push(ref.id);
	}
	++gens[ref.id];
}

//
//
//

Entity Entity::create() {
	Ref entity = Entity::pool.create();
	instances.push({entity.id, entity.gen});
	return {entity.id, entity.gen};
}

void Entity::destroy(Entity const & entity) {
	CUSTOM_ASSERT(entity.exists(), "entity doesn't exist");

	u32 index = UINT32_MAX;
	for (u32 i = 0; i < instances.count; ++i) {
		if (instances[i].id != entity.id) { continue; }
		if (instances[i].gen != entity.gen) { continue; }
		index = i; break;
	}

	if (index < instances.count) {
		instances.remove_at(index);
	}
	else {
		CUSTOM_WARNING("no entity instance for %d:%d", entity.id, entity.gen);
	}

	u32 entity_offset = entity.id * Entity::component_destructors.count;
	for (u32 i = 0; i < Entity::component_destructors.count; ++i) {
		Ref const & ref = Entity::components.get(entity_offset + i);
		(*Entity::component_destructors[i])(ref);
	}
	Entity::pool.destroy(entity);
}

}
