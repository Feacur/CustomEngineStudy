#include "engine/api/internal/component_types.h"
#include "engine/api/internal/entity_system.h"
#include "engine/impl/array.h"
#include "engine/impl/math_linear.h"

#include "component_types.h"

// https://www.youtube.com/watch?v=7Ik2vowGcU0

constexpr inline static bool collide(aabb2 first, aabb2 second) {
	vec2 position = first.position - second.position;
	vec2 size = first.extents + second.extents;
	return position.x <= size.x && position.y <= size.y
	    && position.x >= -size.x && position.y >= -size.y;
}

namespace sandbox {

struct Physical_Blob {
	custom::Entity   entity;
	Phys2d         * physical;
	Transform      * transform;
};

void ecs_update_physics(r32 dt) {
	custom::Array<Physical_Blob> physicals(8);
	for (u32 i = 0; i < custom::Entity::instances.count; ++i) {
		custom::Entity entity = custom::Entity::instances[i];
		if (!entity.exists()) { continue; }

		Phys2d * physical = entity.get_component<Phys2d>().get_safe();
		if (!physical) { continue; }

		Transform * transform = entity.get_component<Transform>().get_safe();
		if (!transform) { continue; }

		physicals.push();
		Physical_Blob * blob = physicals.data + (physicals.count - 1);
		blob->entity = entity;
		blob->physical = physical;
		blob->transform = transform;
	}

	// @Todo: pass transform components data into physical components
	for (u32 i = 0; i < physicals.count; ++i) {
		Physical_Blob & physical = physicals[i];
		physical.physical->position = physical.transform->position.xy;
		physical.physical->transformed.count = 0;

		Phys2d * phys = physical.physical;
		for (u32 point_i = 0; point_i < phys->points.count; ++point_i) {
			phys->transformed.push(phys->points[point_i] + phys->position);
		}
	}

	// @Todo: process
	custom::Array<u32> collisions_count(physicals.count);
	for (u32 ai = 0; ai < physicals.count; ++ai) {
		collisions_count.push(0);
	}
	for (u32 ai = 0; ai < physicals.count; ++ai) {
		for (u32 bi = ai + 1; bi < physicals.count; ++bi) {
		}
	}
	vec2 gravity = {0, 9.81f};
	for (u32 i = 0; i < physicals.count; ++i) {
		if (physicals[i].physical->is_static) { continue; }
		if (collisions_count[i]) { continue; }
		physicals[i].physical->position -= gravity * dt;
	}

	// @Todo: pass physical components data into transform components
	for (u32 i = 0; i < physicals.count; ++i) {
		physicals[i].transform->position.xy = physicals[i].physical->position;
	}
}

}
