#include "engine/api/internal/component_types.h"
#include "engine/api/internal/asset_system.h"
#include "engine/api/internal/entity_system.h"
#include "engine/api/internal/asset_types.h"
#include "engine/impl/array.h"
#include "engine/impl/reference.h"
#include "engine/impl/math_linear.h"

#include "component_types.h"

// https://www.youtube.com/watch?v=7Ik2vowGcU0

//
//
//

struct Phys2d_Internal {
	vec2 position;
	vec2 scale;
	complex rotation;
	custom::Collider2d_Asset * mesh;
	r32 movable;
	custom::Array<vec2> buffer;
};
custom::Ref_PoolT<Phys2d_Internal> custom::RefT<Phys2d_Internal>::pool;

custom::Ref phys2d_add_data(void) {
	typedef custom::RefT<Phys2d_Internal> RefT;
	RefT refT = RefT::pool.create();
	Phys2d_Internal * internal = refT.get_fast();

	internal->buffer.data     = NULL;
	internal->buffer.capacity = 0;
	internal->buffer.count    = 0;

	return refT;
}

void phys2d_rem_data(custom::Ref ref) {
	typedef custom::RefT<Phys2d_Internal> RefT;
	RefT refT = {ref};
	Phys2d_Internal * internal = refT.get_fast();

	internal->buffer.~Array();

	RefT::pool.destroy(ref);
}

struct Physical_Blob {
	custom::Entity    entity;
	Phys2d          * phys2d;
	Transform       * transform;
	Phys2d_Internal * internal;
};

void ecs_update_physics_iteration(r32 dt, custom::Array<Physical_Blob> & physicals);

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
	typedef custom::RefT<Phys2d_Internal> RefT;
	consume_config();

	CUSTOM_ASSERT(physics_rate_target, "zero frequency");
	r32 period = 1.0f / physics_rate_target;

	//
	custom::Array<Physical_Blob> physicals(8);
	for (u32 i = 0; i < custom::Entity::instances.count; ++i) {
		custom::Entity entity = custom::Entity::instances[i];
		if (!entity.exists()) { continue; }

		Phys2d * phys2d = entity.get_component<Phys2d>().get_safe();
		if (!phys2d) { continue; }
		if (!phys2d->mesh.exists()) { CUSTOM_ASSERT(false, "no mesh data"); continue; }

		RefT refT = {phys2d->internal_data};
		Phys2d_Internal * internal = refT.get_safe();
		if (!internal) { CUSTOM_ASSERT(false, "no internal data"); continue; }

		Transform * transform = entity.get_component<Transform>().get_safe();
		if (!transform) { continue; }

		physicals.push();
		Physical_Blob * blob = physicals.data + (physicals.count - 1);
		*blob = {entity, phys2d, transform, internal};
	}

	//
	for (u32 i = 0; i < physicals.count; ++i) {
		Physical_Blob & physical = physicals[i];
		physical.internal->position = physical.transform->position.xy;
		physical.internal->scale = physical.transform->scale.xy;
		CUSTOM_ASSERT(!quat_is_singularity(physical.transform->rotation), "verify your code");
		physical.internal->rotation = complex_from_radians(
			quat_get_radians_z(physical.transform->rotation)
		);
		physical.internal->mesh = physical.phys2d->mesh.ref.get_fast();
		physical.internal->movable = physical.phys2d->movable;
	}

	static r32 elapsed = 0; elapsed += dt;
	while (elapsed >= period) {
		elapsed -= period;
		ecs_update_physics_iteration(period, physicals);
	}

	for (u32 i = 0; i < physicals.count; ++i) {
		Physical_Blob & physical = physicals[i];
		physical.transform->position.xy = physical.internal->position;
		physical.transform->rotation = quat_from_radians({
			0, 0, complex_get_radians(physical.internal->rotation)
		});
	}
}

}

//
//
//

// constexpr static bool collide(aabb2 first, aabb2 second) {
// 	vec2 position = first.position - second.position;
// 	vec2 size = first.extents + second.extents;
// 	return position.x <= size.x && position.y <= size.y
// 	    && position.x >= -size.x && position.y >= -size.y;
// }

static bool overlap_sat(Phys2d_Internal const & first, Phys2d_Internal const & second, r32 & overlap, vec2 & separator) {
	for (u32 axis_i = 0; axis_i < first.buffer.count; ++axis_i) {
		u32 axis_i_2 = (axis_i + 1) % first.buffer.count;
		vec2 axis = first.buffer[axis_i_2] - first.buffer[axis_i];

		// @Note: turn 90 degrees clockwise
		axis = {-axis.y, axis.x};

		// @Note: for early normalization
		axis = normalize(axis);

		r32 min1 = INFINITY, max1 = -INFINITY;
		for (u32 p = 0; p < first.buffer.count; ++p) {
			r32 projection = dot_product(axis, first.buffer[p]);
			min1 = min(min1, projection);
			max1 = max(max1, projection);
		}

		r32 min2 = INFINITY, max2 = -INFINITY;
		for (u32 p = 0; p < second.buffer.count; ++p) {
			r32 projection = dot_product(axis, second.buffer[p]);
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
		Phys2d_Internal * phys = physicals[i].internal;
		custom::Array<vec2> & mesh_points = phys->mesh->buffer;

		phys->buffer.count = 0;
		for (u32 point_i = 0; point_i < mesh_points.count; ++point_i) {
			vec2 const p = complex_product(phys->rotation, mesh_points[point_i] * phys->scale) + phys->position;
			phys->buffer.push(p);
		}
	}

	vec2 gravity = {0, 9.81f};
	for (u32 i = 0; i < physicals.count; ++i) {
		Phys2d_Internal * phys = physicals[i].internal;
		phys->position -= gravity * (physicals[i].internal->movable * dt);
	}

	struct Collision { Phys2d_Internal * phys_a, * phys_b; vec2 separator; };
	custom::Array<Collision> collisions(physicals.count);
	for (u32 ai = 0; ai < physicals.count; ++ai) {
		Phys2d_Internal & phys_a = *physicals[ai].internal;
		for (u32 bi = ai + 1; bi < physicals.count; ++bi) {
			Phys2d_Internal & phys_b = *physicals[bi].internal;
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
