#include "engine/api/internal/component_types.h"
#include "engine/api/internal/asset_system.h"
#include "engine/api/internal/entity_system.h"
#include "engine/api/internal/asset_types.h"
#include "engine/impl/array.h"
#include "engine/impl/reference.h"
#include "engine/impl/math_linear.h"

#include "component_types.h"

// https://www.youtube.com/watch?v=7Ik2vowGcU0

struct Entity_Blob {
	custom::Entity   entity;
	Transform      * transform;
	Phys2d         * physical;
};

struct Physical_Blob {
	// Transform
	vec2 position;
	vec2 scale;
	complex rotation;
	// Phys2d
	custom::Collider2d_Asset * mesh;
	r32 dynamic;
	r32 mass;
	r32 restitution;
	vec2 velocity;
	vec2 acceleration;
};

struct Points_Blob {
	u32 offset, count;
};

struct Collision {
	Physical_Blob * phys_a, * phys_b;
	vec2 normal;
	r32 overlap;
};

struct Physics_Settings {
	u32  frequency = 50;
	vec2 gravity   = {0, -9.81f};
};

static Physics_Settings settings;
static custom::Array<Points_Blob> transformed_points;
static custom::Array<vec2>        transformed_points_buffer;

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

static void consume_config(void) {
	u32 version = custom::empty_index;

	custom::Config_Asset const * config = config_ref.ref.get_safe();
	CUSTOM_ASSERT(config, "no config");

	if (version == config->version) { return; }
	version = config->version;

	settings.frequency = config->get_value<u32>("physics_frequency", 50);
	settings.gravity   = {
		config->get_value<r32>("physics_gravity_x", 0),
		config->get_value<r32>("physics_gravity_y", -9.81f),
	};
}

void ecs_update_physics(r32 dt) {
	consume_config();

	CUSTOM_ASSERT(settings.frequency, "zero frequency");
	r32 period = 1.0f / settings.frequency;

	//
	custom::Array<Entity_Blob> entities(8);
	for (u32 i = 0; i < custom::Entity::instances.count; ++i) {
		custom::Entity entity = custom::Entity::instances[i];
		if (!entity.exists()) { continue; }

		Transform * transform = entity.get_component<Transform>().get_safe();
		if (!transform) { continue; }

		Phys2d * physical = entity.get_component<Phys2d>().get_safe();
		if (!physical) { continue; }
		if (!physical->mesh.exists()) { CUSTOM_ASSERT(false, "no mesh data"); continue; }

		CUSTOM_ASSERT(!quat_is_singularity(transform->rotation), "verify your code");
		entities.push({entity, transform, physical});
	}

	//
	custom::Array<Physical_Blob> physicals(entities.count);
	for (u32 i = 0; i < entities.count; ++i) {
		Entity_Blob const & entity = entities[i];

		physicals.push();
		Physical_Blob * blob = physicals.data + (physicals.count - 1);

		//
		blob->position = entity.transform->position.xy;
		blob->scale    = entity.transform->scale.xy;
		blob->rotation = complex_from_radians(
			quat_get_radians_z(entity.transform->rotation)
		);

		//
		blob->dynamic      = entity.physical->dynamic;
		blob->mass         = entity.physical->mass;
		blob->restitution  = entity.physical->restitution;
		blob->acceleration = entity.physical->acceleration;
		blob->velocity     = entity.physical->velocity;
		blob->mesh         = entity.physical->mesh.ref.get_fast();
	}

	static r32 elapsed = 0; elapsed += dt;
	while (elapsed >= period) {
		elapsed -= period;
		ecs_update_physics_iteration(period, physicals);
	}

	for (u32 i = 0; i < entities.count; ++i) {
		Physical_Blob const & physical = physicals[i];
		Entity_Blob         & entity   = entities[i];
		//
		entity.transform->position.xy = physical.position;
		entity.transform->rotation = quat_from_radians({
			0, 0, complex_get_radians(physical.rotation)
		});
		//
		entity.physical->acceleration = physical.acceleration;
		entity.physical->velocity     = physical.velocity;
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

static bool overlap_sat(u32 first_i, u32 second_i, r32 & overlap, vec2 & separator) {
	vec2 const * first_points = transformed_points_buffer.data + transformed_points[first_i].offset;
	u32 const    first_points_count = transformed_points[first_i].count;

	vec2 const * second_points = transformed_points_buffer.data + transformed_points[second_i].offset;
	u32 const    second_points_count = transformed_points[second_i].count;

	for (u32 axis_i = 0; axis_i < first_points_count; ++axis_i) {
		u32 axis_i_2 = (axis_i + 1) % first_points_count;

		// @Note: turn the edge 90 degrees clockwise
		vec2 axis = first_points[axis_i_2] - first_points[axis_i];
		axis = normalize(vec2{-axis.y, axis.x});

		r32 min1 = INFINITY, max1 = -INFINITY;
		for (u32 p = 0; p < first_points_count; ++p) {
			r32 projection = dot_product(axis, first_points[p]);
			min1 = min(min1, projection);
			max1 = max(max1, projection);
		}

		r32 min2 = INFINITY, max2 = -INFINITY;
		for (u32 p = 0; p < second_points_count; ++p) {
			r32 projection = dot_product(axis, second_points[p]);
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
	vec2 const global_gravity = settings.gravity;
	for (u32 i = 0; i < physicals.count; ++i) {
		Physical_Blob & phys = physicals[i];
		phys.velocity += (phys.acceleration + global_gravity) * dt;
	}

	for (u32 i = 0; i < physicals.count; ++i) {
		Physical_Blob & phys = physicals[i];
		phys.position += phys.velocity * (phys.dynamic * dt);
	}

	// @Todo: broad phase; at least, global AABB for the time being

	// @Todo: process in local space?
	//        - until then, apply whole hierarchy transform?
	//        - allow only local space?
	transformed_points.count = 0;
	transformed_points_buffer.count = 0;
	for (u32 i = 0; i < physicals.count; ++i) {
		Physical_Blob & phys = physicals[i];
		custom::Array<vec2> const & mesh_points = phys.mesh->points;

		transformed_points.push({
			transformed_points_buffer.count,
			mesh_points.count
		});

		for (u32 point_i = 0; point_i < mesh_points.count; ++point_i) {
			vec2 const p = complex_product(phys.rotation, mesh_points[point_i] * phys.scale) + phys.position;
			transformed_points_buffer.push(p);
		}
	}

	custom::Array<Collision> collisions(physicals.count);
	for (u32 ai = 0; ai < physicals.count; ++ai) {
		Physical_Blob & phys_a = physicals[ai];
		for (u32 bi = ai + 1; bi < physicals.count; ++bi) {
			Physical_Blob & phys_b = physicals[bi];
			r32 overlap = INFINITY; vec2 separator;
			if (!overlap_sat(ai, bi, overlap, separator)) { continue; }
			if (!overlap_sat(bi, ai, overlap, separator)) { continue; }
			if (overlap == 0) { continue; }
			// @Note: for late normalization
			// separator = normalize(separator);

			separator = separator * sign(dot_product(separator, phys_a.position - phys_b.position));
			collisions.push({&phys_a, &phys_b, separator, overlap});
		}
	}

	for (u32 i = 0; i < collisions.count; ++i) {
		vec2 separator = collisions[i].normal * collisions[i].overlap;
		collisions[i].phys_a->position += separator * collisions[i].phys_a->dynamic;
		collisions[i].phys_b->position -= separator * collisions[i].phys_b->dynamic;
	}

	for (u32 i = 0; i < collisions.count; ++i) {
		vec2 normal = collisions[i].normal;
		collisions[i].phys_a->velocity = reflect(collisions[i].phys_a->velocity, normal,  1 + collisions[i].phys_a->restitution);
		collisions[i].phys_b->velocity = reflect(collisions[i].phys_b->velocity, -normal, 1 + collisions[i].phys_b->restitution);
	}
}
