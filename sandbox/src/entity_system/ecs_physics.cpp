#include "engine/api/internal/component_types.h"
#include "engine/api/internal/asset_system.h"
#include "engine/api/internal/entity_system.h"
#include "engine/api/internal/asset_types.h"
#include "engine/impl/array.h"
#include "engine/impl/reference.h"
#include "engine/impl/math_linear.h"

#include "component_types.h"

// https://www.youtube.com/watch?v=7Ik2vowGcU0

// constexpr static bool collide(aabb2 first, aabb2 second) {
// 	vec2 position = first.position - second.position;
// 	vec2 size = first.extents + second.extents;
// 	return position.x <= size.x && position.y <= size.y
// 	    && position.x >= -size.x && position.y >= -size.y;
// }

struct Physical_Blob {
	custom::Entity   entity;
	Phys2d         * physical;
	Transform      * transform;
};

void ecs_update_physics_iteration(r32 dt, custom::Array<Physical_Blob> & physicals);

//
//
//

struct Phys2d_Internal {
	vec2 position;
	vec2 scale;
	complex rotation;
	custom::Array<vec2> buffer;
};
template<> struct custom::RefT<Phys2d_Internal>;

custom::Ref phys2d_add_data(void) {
	return custom::empty_ref;
}

void phys2d_rem_data(custom::Ref ref) {

}

//
//
//

namespace sandbox {

static custom::Asset_RefT<custom::Config_Asset> config_ref = {custom::empty_ref, custom::empty_index};
void ecs_init_physics(void) {
	u32 config_id = custom::Asset::get_resource("assets/configs/client.cfg", custom::empty_index);
	config_ref = custom::Asset::add<custom::Config_Asset>(config_id);
}

static u32 physics_rate_target   = 50;
static void consume_config(void) {
	static u32 version = custom::empty_index;

	custom::Config_Asset const * config = config_ref.ref.get_safe();
	CUSTOM_ASSERT(config, "no config");

	if (version == config->version) { return; }
	version = config->version;

	physics_rate_target = config->get_value<u32>("physics_rate_target", 50);
}

void ecs_update_physics(r32 dt) {
	consume_config();

	CUSTOM_ASSERT(physics_rate_target, "zero frequency");
	r32 period = 1.0f / physics_rate_target;

	//
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

	//
	for (u32 i = 0; i < physicals.count; ++i) {
		Physical_Blob & physical = physicals[i];
		physical.physical->position = physical.transform->position.xy;
		physical.physical->scale = physical.transform->scale.xy;
		CUSTOM_ASSERT(!quat_is_singularity(physical.transform->rotation), "verify your code");
		physical.physical->rotation = complex_from_radians(
			quat_get_radians_z(physical.transform->rotation)
		);
	}

	static r32 elapsed = 0; elapsed += dt;
	while (elapsed >= period) {
		elapsed -= period;
		ecs_update_physics_iteration(period, physicals);
	}

	for (u32 i = 0; i < physicals.count; ++i) {
		Physical_Blob & physical = physicals[i];
		physical.transform->position.xy = physical.physical->position;
		physical.transform->rotation = quat_from_radians({
			0, 0, complex_get_radians(physical.physical->rotation)
		});
	}
}

}

//
//
//

static bool overlap_sat(Phys2d const & first, Phys2d const & second, r32 & overlap, vec2 & separator) {
	for (u32 axis_i = 0; axis_i < first.transformed.count; ++axis_i) {
		u32 axis_i_2 = (axis_i + 1) % first.transformed.count;
		vec2 axis = first.transformed[axis_i_2] - first.transformed[axis_i];

		// @Note: turn 90 degrees clockwise
		axis = {-axis.y, axis.x};

		// @Note: for early normalization
		axis = normalize(axis);

		r32 min1 = INFINITY, max1 = -INFINITY;
		for (u32 p = 0; p < first.transformed.count; ++p) {
			r32 projection = dot_product(axis, first.transformed[p]);
			min1 = min(min1, projection);
			max1 = max(max1, projection);
		}

		r32 min2 = INFINITY, max2 = -INFINITY;
		for (u32 p = 0; p < second.transformed.count; ++p) {
			r32 projection = dot_product(axis, second.transformed[p]);
			min2 = min(min2, projection);
			max2 = max(max2, projection);
		}

		if (min1 > max2) { return false; }
		if (min2 > max2) { return false; }

		r32 projection_overlap = min(max1, max2) - max(min1, min2);
		if (overlap > projection_overlap) {
			overlap = projection_overlap;
			separator = axis;
		}
	}
	return true;
}

void ecs_update_physics_iteration(r32 dt, custom::Array<Physical_Blob> & physicals) {
	// @Todo: broad phase; at least, global AABB for the time being

	// @Todo: process in local space?
	//        - until then, apply whole hierarchy transform?
	//        - allow only local space?
	for (u32 i = 0; i < physicals.count; ++i) {
		Phys2d * phys = physicals[i].physical;
		custom::Collider2d_Asset * mesh = phys->mesh.ref.get_safe();

		phys->transformed.count = 0;
		for (u32 point_i = 0; point_i < mesh->buffer.count; ++point_i) {
			vec2 const p = complex_product(phys->rotation, mesh->buffer[point_i] * phys->scale) + phys->position;
			phys->transformed.push(p);
		}
	}

	vec2 gravity = {0, 9.81f};
	for (u32 i = 0; i < physicals.count; ++i) {
		physicals[i].physical->position -= gravity * (physicals[i].physical->movable * dt);
	}

	struct Collision { Phys2d * phys_a, * phys_b; vec2 separator; };
	custom::Array<Collision> collisions(physicals.count);
	for (u32 ai = 0; ai < physicals.count; ++ai) {
		Phys2d & phys_a = *physicals[ai].physical;
		for (u32 bi = ai + 1; bi < physicals.count; ++bi) {
			Phys2d & phys_b = *physicals[bi].physical;
			r32 overlap = INFINITY; vec2 separator;
			if (!overlap_sat(phys_a, phys_b, overlap, separator)) { continue; }
			if (!overlap_sat(phys_b, phys_a, overlap, separator)) { continue; }
			if (overlap == 0) { continue; }
			// @Note: for late normalization
			// separator = normalize(separator);

			separator = separator * sign(dot_product(separator, phys_a.position - phys_b.position));
			collisions.push({&phys_a, &phys_b, separator * overlap});
		}
	}

	for (u32 i = 0; i < collisions.count; ++i) {
		collisions[i].phys_a->position += collisions[i].separator * collisions[i].phys_a->movable;
		collisions[i].phys_b->position -= collisions[i].separator * collisions[i].phys_b->movable;
	}
}
