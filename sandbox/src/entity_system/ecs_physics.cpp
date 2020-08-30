#include "engine/api/internal/component_types.h"
#include "engine/api/internal/entity_system.h"
#include "engine/impl/array.h"
#include "engine/impl/math_linear.h"

#include "component_types.h"

namespace sandbox {

struct Physical_Blob {
	custom::Entity   entity;
	Physical       * physical;
};

struct Transformable_Blob {
	custom::Entity   entity;
	Transform      * transform;
	Physical       * physical;
};

void ecs_update_physics(void) {
	custom::Array<Physical_Blob> physicals(8);
	custom::Array<Transformable_Blob> transformables(8);
	for (u32 i = 0; i < custom::Entity::instances.count; ++i) {
		custom::Entity entity = custom::Entity::instances[i];
		if (!entity.exists()) { continue; }

		Physical * physical = entity.get_component<Physical>().get_safe();
		if (!physical) { continue; }

		physicals.push({entity, physical});

		Transform * transform = entity.get_component<Transform>().get_safe();
		if (!transform) { continue; }

		transformables.push({entity, transform, physical});
	}

	// @Todo: pass transform components data into physical components
	for (u32 i = 0; i < transformables.count; ++i) {
	}

	// @Todo: process
	for (u32 i = 0; i < physicals.count; ++i) {
	}

	// @Todo: pass physical components data into transform components
	for (u32 i = 0; i < transformables.count; ++i) {
	}
}

}
