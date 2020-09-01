#include "engine/api/internal/component_types.h"
#include "engine/api/internal/entity_system.h"
#include "engine/impl/array.h"
#include "engine/impl/math_linear.h"

#include "component_types.h"

// https://www.youtube.com/watch?v=7Ik2vowGcU0

namespace sandbox {

struct Physical_Blob {
	custom::Entity   entity;
	Phys2d         * physical;
};

struct Transformable_Blob {
	custom::Entity   entity;
	Transform      * transform;
	Phys2d         * physical;
};

void ecs_update_physics(r32 dt) {
	custom::Array<Physical_Blob> physicals(8);
	custom::Array<Transformable_Blob> transformables(8);
	for (u32 i = 0; i < custom::Entity::instances.count; ++i) {
		custom::Entity entity = custom::Entity::instances[i];
		if (!entity.exists()) { continue; }

		Phys2d * physical = entity.get_component<Phys2d>().get_safe();
		if (!physical) { continue; }

		physicals.push({entity, physical});

		Transform * transform = entity.get_component<Transform>().get_safe();
		if (!transform) { continue; }

		transformables.push({entity, transform, physical});
	}

	// @Todo: pass transform components data into physical components
	for (u32 i = 0; i < transformables.count; ++i) {
		transformables[i].physical->position = transformables[i].transform->position.xy;
	}

	// @Todo: process
	vec2 gravity = {0, 9.81f};
	for (u32 i = 0; i < physicals.count; ++i) {
		if (physicals[i].physical->is_static) { continue; }
		physicals[i].physical->position -= gravity * dt;
	}

	// @Todo: pass physical components data into transform components
	for (u32 i = 0; i < transformables.count; ++i) {
		transformables[i].transform->position.xy = transformables[i].physical->position;
	}
}

}
