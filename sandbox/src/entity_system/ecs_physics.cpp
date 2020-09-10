#include "engine/api/internal/component_types.h"
#include "engine/api/internal/asset_system.h"
#include "engine/api/internal/entity_system.h"
#include "engine/api/internal/asset_types.h"
#include "engine/impl/array.h"
#include "engine/impl/reference.h"
#include "engine/impl/math_linear.h"

#include "component_types.h"

// https://www.youtube.com/watch?v=7Ik2vowGcU0
// https://gafferongames.com/post/integration_basics/
// https://github.com/erincatto/box2d-lite
// https://github.com/RandyGaul/ImpulseEngine
// https://en.wikipedia.org/wiki/Coefficient_of_restitution
// https://www.youtube.com/watch?v=fdZfddO7YTs
// https://en.wikipedia.org/wiki/Conservation_of_momentum
// https://en.wikipedia.org/wiki/Conservation_of_energy
// https://en.wikipedia.org/wiki/Angular_momentum
// https://en.wikipedia.org/wiki/Momentum
// https://en.wikipedia.org/wiki/Friction

// @Note: simplest collision response against a static object looks like
//        velocity = reflect(velocity, normal, 1 + cor);
//        but general case is a tiniest bit more complex:
//        reflection factors depend on the plane of impact, masses, and relative velocity

// @Note: momentum conservation, energy conservation, etc.
//        masses:            m1, m2; constant
//        velocities before: v1, v2
//        velocities after:  u1, u2
//        shape:             point; other shapes experience conversion of a portion of *impulses* into `angular momentum`
//        ---- ---- ---- ----
//        ---- ---- ---- ----
//     >> express an equation for `energy conservation`
//        m1*v1*v1/2 + m2*v2*v2/2 + other_energies = m1*u1*u1/2 + m2*u2/2 + other_energies + kinetic_energy_change
//        ---- ---- ---- ----
//        kinetic_energy_change * (m*v*v/2) = (m*u*u/2)
//        kinetic_energy_change = (u*u) / (v*v)
//        ---- ---- ---- ----
//        ---- ---- ---- ----
//     >> express an equation for `momentum conservation`
//        m1*v1 + m2*v2 = m1*u1 + m2*u2
//        ---- ---- ---- ----
//        m1*v1 + deformation_impulse = m1*v_impact
//        m2*v2 - deformation_impulse = m2*v_impact
//        m1*v_impact + restoration_impulse = m1*u1
//        m2*v_impact - restoration_impulse = m2*u2
//        ---- ---- ---- ----
//     >> express an equation for `coefficient of restitution`
//        coefficient_of_restitution = square_root(kinetic_energy_change)
//        coefficient_of_restitution ~ |u| / |v|
//        ---- ---- ---- ----
//        coefficient_of_restitution = restoration_impulse / deformation_impulse
//        cor = (u1 - v_impact) / (v_impact - v1)
//        cor = (v_impact - u2) / (v2 - v_impact)
//        ---- ---- ---- ----
//        cor = (restoration_impulse + restoration_impulse) / (deformation_impulse + deformation_impulse)
//        cor = ((u1 - v_impact) + (v_impact - u2)) / ((v_impact - v1) + (v2 - v_impact))
//        cor * (v2 - v1) = (u1 - u2)
//        ---- ---- ---- ----
//        ---- ---- ---- ----
//     >> use the equations of `coefficient of restitution` and `momentum conservation`
//        u1 = (m1*v1 + m2*v2 - m2*u2) / m1
//        u2 = u1 + cor*(v1 - v2)
//        ---- ---- ---- ----
//        u1 = (m1*v1 + m2*v2 - m2*cor*(v1 - v2)) / (m1 + m2)
//        u1 = v1 +  m2 * (v2 - v1) * (1 + cor) / (m1 + m2)
//        ---- ---- ---- ----
//        impulse = (v2 - v1) * (1 + cor) / (m1 + m2)
//

// @Note: use inverse masses for computation:
//        the reason is presence of infinite mass objects; you do want to avoid doing `inf / inf`;
//        additionally, each object operates an impulse and its mass only
//        impulse = (v2 - v1) * (1 + cor) / (1 / m1 + 1 / m2)
//        u1 = v1 + impulse * (1 / m1)
//        u2 = v2 - impulse * (1 / m2)

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
	r32 dynamic, inverse_mass;
	r32 elasticity;
	r32 roughness;
	r32 stickiness;
	custom::Collider2d_Asset * mesh;
	//
	vec2 velocity;
	vec2 acceleration;

	void add_impulse(vec2 value) {
		velocity += value * inverse_mass;
	}

	void add_force(vec2 value) {
		acceleration += value * inverse_mass;
	}
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
	r32 separation_fraction = 0.5f;
	r32 separation_bias     = 0.01f;
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
	settings.separation_fraction = config->get_value<r32>("physics_separation_fraction", 0.8f);
	settings.separation_bias     = config->get_value<r32>("physics_separation_bias", 0.01f);
}

void ecs_update_physics(r32 dt) {
	consume_config();

	CUSTOM_ASSERT(settings.frequency, "zero frequency");
	r32 period = 1.0f / settings.frequency;

	//
	custom::Array<Entity_Blob> entities(8);
	for (u32 i = 0; i < custom::Entity::state.instances.count; ++i) {
		custom::Entity entity = custom::Entity::state.instances[i];
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
		blob->inverse_mass = entity.physical->dynamic / entity.physical->mass;
		blob->elasticity   = entity.physical->elasticity;
		blob->roughness    = entity.physical->roughness;
		blob->stickiness   = entity.physical->stickiness;
		//
		blob->mesh = entity.physical->mesh.ref.get_fast();
		//
		blob->velocity     = entity.physical->velocity;
		blob->acceleration = entity.physical->acceleration;
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
		entity.physical->velocity = physical.velocity;
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
		phys.velocity += (global_gravity + phys.acceleration) * (phys.dynamic * dt);
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

			if (phys_a.dynamic == 0 && phys_b.dynamic == 0) { continue; }

			r32 overlap = INFINITY; vec2 separator;
			if (!overlap_sat(ai, bi, overlap, separator)) { continue; }
			if (!overlap_sat(bi, ai, overlap, separator)) { continue; }
			if (overlap == 0) { continue; }

			separator = separator * sign(dot_product(separator, phys_a.position - phys_b.position));
			collisions.push({&phys_a, &phys_b, separator, overlap});
		}
	}

	for (u32 i = 0; i < collisions.count; ++i) {
		Physical_Blob * phys_a = collisions[i].phys_a;
		Physical_Blob * phys_b = collisions[i].phys_b;

		// @Todo: account angular motion
		vec2 contact_velocity = phys_b->velocity - phys_a->velocity;

		// @Note: do not collide separating objects
		vec2 const normal = collisions[i].normal;
		r32 contact_velocity_normal = dot_product(contact_velocity, normal);
		if (contact_velocity_normal <= 0) { continue; }

		// common data
		r32 const contact_mass = 1 / (phys_a->inverse_mass + phys_b->inverse_mass);

		// normal impulse
		r32 const  restitution           = square_root(phys_a->elasticity * phys_b->elasticity);
		r32 const  normal_impulse        = contact_velocity_normal * contact_mass * (1 + restitution);
		vec2 const normal_impulse_vector = normal * normal_impulse;
		phys_a->add_impulse(normal_impulse_vector);
		phys_b->add_impulse(-normal_impulse_vector);

		// tangent impulse
		vec2 const tangent_velocity       = contact_velocity - normal * contact_velocity_normal;
		r32 const  tangent_axis_magnitude = magnitude_squared(tangent_velocity);
		if (tangent_axis_magnitude == 0) { continue; }

		r32 const contact_velocity_tangent = square_root(tangent_axis_magnitude);
		vec2 tangent_axis = tangent_velocity / contact_velocity_tangent;

		r32 const friction  = square_root(phys_a->roughness * phys_b->roughness);
		r32 const stiction  = square_root(phys_a->stickiness * phys_b->stickiness);
		r32 tangent_impulse = contact_velocity_tangent * contact_mass;
		tangent_impulse = (tangent_impulse < normal_impulse * stiction)
			? tangent_impulse * stiction
			: normal_impulse  * friction;
		vec2 const tangent_impulse_vector = tangent_axis * tangent_impulse;
		phys_a->add_impulse(tangent_impulse_vector);
		phys_b->add_impulse(-tangent_impulse_vector);
	}

	for (u32 i = 0; i < collisions.count; ++i) {
		if (collisions[i].overlap < settings.separation_bias) { continue; }
		vec2 separator = collisions[i].normal * collisions[i].overlap;

		Physical_Blob * phys_a = collisions[i].phys_a;
		Physical_Blob * phys_b = collisions[i].phys_b;

		r32 mass = settings.separation_fraction / (phys_a->inverse_mass + phys_b->inverse_mass);
		phys_a->position += separator * (phys_a->inverse_mass * mass);
		phys_b->position -= separator * (phys_b->inverse_mass * mass);
	}
}
