#include "engine/api/internal/component_types.h"
#include "engine/api/internal/asset_system.h"
#include "engine/api/internal/entity_system.h"
#include "engine/api/internal/asset_types.h"
#include "engine/impl/array.h"
#include "engine/impl/reference.h"
#include "engine/impl/math_linear.h"

#include "component_types.h"

// https://en.wikipedia.org/wiki/List_of_moments_of_inertia
// https://en.wikipedia.org/wiki/List_of_second_moments_of_area
// https://en.wikipedia.org/wiki/Polygon
// https://allenchou.net/game-physics-series/
// https://hyperphysics.phy-astr.gsu.edu/hbase/rke.html
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
// https://en.wikipedia.org/wiki/Stiction
// https://en.wikipedia.org/wiki/Angular_velocity
// https://en.wikipedia.org/wiki/Angular_acceleration
// https://stackoverflow.com/questions/31106438/calculate-moment-of-inertia-given-an-arbitrary-convex-2d-polygon
// https://stackoverflow.com/questions/41592034/computing-tensor-of-inertia-in-2d
// https://en.wikipedia.org/wiki/Moment_of_inertia
// https://en.wikipedia.org/wiki/Second_moment_of_area
// https://www.gdcvault.com/play/1017646/Physics-for-Game-Programmers:-The-Separating-Axis-Test-between-Convex-Polyhedra-by-Dirk Gregorius
// https://www.dyn4j.org/2010/01/sat/
// https://www.dyn4j.org/2010/04/gjk-gilbert-johnson-keerthi/
// https://www.dyn4j.org/2010/04/gjk-distance-closest-points/
// https://www.dyn4j.org/2010/05/epa-expanding-polytope-algorithm/
// https://www.dyn4j.org/2011/11/contact-points-using-clipping/

// @Note: from here I shorten `coefficient of restitution` to `restitution`,
//        the same goes for `friction` and `stiction`

// @Note: simplest collision response against a static object looks like
//        velocity = reflect(velocity, normal, 1 + restitution);
//        but general case is a tiniest bit more complex:
//        reflection factors depend on the plane [or line for 2d] of impact, masses, and relative velocity

// @Note: restitution; normal direction separation
//        masses:                   m1, m2; constant
//        linear velocities before: v1, v2
//        linear velocities after:  u1, u2
//        linear accelerations:     a1, a2
//        linear momentums:         P1, P2
//        linear forces:            F1, F2
//        ---- ---- ---- ----
//        shape: point; other shapes experience conversion of a portion of *impulses* into `angular momentum`;
//               only impact in the direction normal to the surfaces is accounted in these formulas, so that we
//               can compute separation impulses, disregarding tangential motion; or rather postponing its computation
//        ---- ---- ---- ----
//        P   = m*v
//        F   = m*a
//        Ekl = m*v*v/2
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
//        restitution = square_root(kinetic_energy_change)
//        restitution ~ |u| / |v|
//        ---- ---- ---- ----
//        restitution = restoration_impulse / deformation_impulse
//        restitution = (u1 - v_impact) / (v_impact - v1)
//        restitution = (v_impact - u2) / (v2 - v_impact)
//        ---- ---- ---- ----
//        restitution = (restoration_impulse + restoration_impulse) / (deformation_impulse + deformation_impulse)
//        restitution = ((u1 - v_impact) + (v_impact - u2)) / ((v_impact - v1) + (v2 - v_impact))
//        restitution * (v2 - v1) = (u1 - u2)
//        ---- ---- ---- ----
//        ---- ---- ---- ----
//     >> use the equations of `coefficient of restitution` and `momentum conservation`
//        u1 = (m1*v1 + m2*v2 - m2*u2) / m1
//        u2 = u1 + restitution*(v1 - v2)
//        ---- ---- ---- ----
//        u1 = (m1*v1 + m2*v2 - m2*restitution*(v1 - v2)) / (m1 + m2)
//        u1 = v1 +  m2 * (v2 - v1) * (1 + restitution) / (m1 + m2)
//        ---- ---- ---- ----
//        linear separation impulse = (v2 - v1) * (1 + restitution) / (m1 + m2)

// @Note: use inverse masses for computation:
//        the reason is presence of infinite mass objects; you do want to avoid doing `inf / inf`;
//        additionally, each object operates an impulse and its mass only
//        impulse = (v2 - v1) * (1 + restitution) / (1 / m1 + 1 / m2)
//        u1 = v1 + impulse * (1 / m1)
//        u2 = v2 - impulse * (1 / m2)

// @Note: friction, stiction; tangential direction separation
//        friction force:   Ff
//        stiction force:   Fs
//        normal force:     Fn
//        tangential force: Ft
//        ---- ---- ---- ----
//        ---- ---- ---- ----
//        - friction force is proportional to normal force and is directed opposite to the tangential motion vector
//        - ... which is `Ff = Fn * friction`
//        - also we can account Coulomb's law: if tangential force is lower than some portion of normal force, then
//        - ... no motion occurs in that direction until some threshold met

// @Note: application of angular momentum, etc.
//        moment of inertia:         I1, I2; constant; depends on the shape
//        angular velocities before: w1, w2
//        angular velocities after:  f1, f2
//        angular accelerations:     @1, @2
//        angular momentums:         L1, L2
//        angular forces:            T1, T2; or `torque`
//        some radii:                r1, r2
//        tangent vilocities:        V1, V2
//        ---- ---- ---- ----
//        L   = I*w
//        T   = I*@
//        Ekr = I*w*w/2
//        V   = w*r
//        ---- ---- ---- ----
//        ---- ---- ---- ----
//

struct Entity_Blob {
	custom::Entity   entity;
	Transform      * transform;
	Phys2d         * physical;
};

struct Physical_Blob {
	// Transform
	vec2 position, scale;
	complex rotation;
	// Phys2d
	r32 dynamic, inverse_mass, inverse_angular_mass;
	r32 elasticity, roughness, stickiness, stillness;
	custom::Collider2d_Asset * mesh;
	//
	vec2 velocity;
	vec2 acceleration;
	r32  angular_velocity;
	r32  angular_acceleration;

	void add_impulse(vec2 value, vec2 radius) {
		velocity += value * inverse_mass;
		angular_velocity += cross_product(radius, value) * inverse_angular_mass;
	}

	void add_force(vec2 value, vec2 radius) {
		acceleration += value * inverse_mass;
		angular_acceleration += cross_product(radius, value) * inverse_angular_mass;
	}
};

struct Points_Blob {
	u32 offset, count;
};

struct Collision {
	Physical_Blob * phys_a, * phys_b;
	vec2 contact;
	vec2 normal;
	r32  overlap;
};

struct Physics_Settings {
	u32  frequency           = 50;
	vec2 gravity             = {0, -9.81f};
	r32  separation_fraction = 0.5f;
	r32  separation_bias     = 0.01f;
};

struct Face { vec2 vertices[2]; };

static Physics_Settings settings;
static custom::Array<Points_Blob> transformed_points;
static custom::Array<vec2>        transformed_points_buffer;

static void ecs_update_physics_iteration(r32 dt, custom::Array<Physical_Blob> & physicals);

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
		blob->dynamic              = entity.physical->dynamic;
		blob->inverse_mass         = entity.physical->dynamic / entity.physical->mass;
		blob->inverse_angular_mass = entity.physical->dynamic / (entity.physical->mass * entity.physical->shape);
		blob->elasticity           = entity.physical->elasticity;
		blob->roughness            = entity.physical->roughness;
		blob->stickiness           = entity.physical->stickiness;
		blob->stillness            = entity.physical->stillness;
		//
		blob->mesh = entity.physical->mesh.ref.get_fast();
		//
		blob->velocity             = entity.physical->velocity;
		blob->acceleration         = entity.physical->acceleration;
		blob->angular_velocity     = entity.physical->angular_velocity;
		blob->angular_acceleration = entity.physical->angular_acceleration;
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
		entity.physical->acceleration = {0, 0};
		entity.physical->angular_velocity = physical.angular_velocity;
		entity.physical->angular_acceleration = 0;
	}
}

}

//
//
//

// for (u32 i = 0; i < transformed_points.count; ++i) {
// 	vec2 const * points = transformed_points_buffer.data + transformed_points[i].offset;
// 	u32 const    points_count = transformed_points[i].count;
// 
// 	r32 area = 0;
// 	for (u32 point_a = 0; point_a < points_count; ++point_a) {
// 		u32 point_b = (point_a + 1) % points_count;
// 		area += cross_product(points[point_b], points[point_a]);
// 	}
// 	area /= 2;
// }

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

	for (u32 face_i = 0; face_i < first_points_count; ++face_i) {
		u32 face_i_2 = (face_i + 1) % first_points_count;
		vec2 normal = normalize(
			cross_product(first_points[face_i_2] - first_points[face_i], 1.0f)
		);

		r32 min1 = INFINITY, max1 = -INFINITY;
		for (u32 p = 0; p < first_points_count; ++p) {
			r32 projection = dot_product(normal, first_points[p]);
			min1 = min(min1, projection);
			max1 = max(max1, projection);
		}

		r32 min2 = INFINITY, max2 = -INFINITY;
		for (u32 p = 0; p < second_points_count; ++p) {
			r32 projection = dot_product(normal, second_points[p]);
			min2 = min(min2, projection);
			max2 = max(max2, projection);
		}

		if (min1 > max2) { return false; }
		if (min2 > max2) { return false; }

		r32 projection_overlap = min(max1, max2) - max(min1, min2);
		if (overlap > projection_overlap) {
			overlap = projection_overlap;
			separator = normal;
		}
	}
	return true;
}

static u32 find_closest_face(u32 phys_i, vec2 reference_normal) {
	vec2 const * points = transformed_points_buffer.data + transformed_points[phys_i].offset;
	u32 const    points_count = transformed_points[phys_i].count;

	u32 face     = custom::empty_index;
	r32 face_dot = -INFINITY;
	for (u32 face_i = 0; face_i < points_count; ++face_i) {
		u32 face_i_2 = (face_i + 1) % points_count;
		vec2 normal = normalize(
			cross_product(points[face_i_2] - points[face_i], 1.0f)
		);

		r32 dot = dot_product(normal, reference_normal);
		if (face_dot < dot) {
			face_dot = dot;
			face = face_i;
		}
	}
	return face;
}

static void fill_face(u32 phys_i, u32 face_i, Face & face) {
	vec2 const * points = transformed_points_buffer.data + transformed_points[phys_i].offset;
	u32 const    points_count = transformed_points[phys_i].count;

	u32 face_i_2 = (face_i + 1) % points_count;
	face.vertices[0] = points[face_i];
	face.vertices[1] = points[face_i_2];
}

static void ecs_update_physics_iteration(r32 dt, custom::Array<Physical_Blob> & physicals) {
	vec2 const global_gravity = settings.gravity;
	for (u32 i = 0; i < physicals.count; ++i) {
		Physical_Blob & phys = physicals[i];

		phys.velocity += (global_gravity + phys.acceleration) * (phys.dynamic * dt);
		phys.angular_velocity += (phys.angular_acceleration) * (phys.dynamic * dt);

		phys.position += phys.velocity * (phys.dynamic * dt);
		phys.rotation = complex_product(
			phys.rotation,
			complex_from_radians(phys.angular_velocity * (phys.dynamic * dt))
		);
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

			//
			r32 overlap = INFINITY; vec2 separator;
			if (!overlap_sat(ai, bi, overlap, separator)) { continue; }
			if (!overlap_sat(bi, ai, overlap, separator)) { continue; }

			// force separation to be ralative to `ai`
			separator = separator * sign(dot_product(separator, phys_b.position - phys_a.position));

			// collision happened, find contact
			Face faces[2];
			fill_face(ai, find_closest_face(ai, separator), faces[0]);
			fill_face(bi, find_closest_face(bi, -separator), faces[1]);

			u32 face_reference =
				absolute(dot_product(faces[0].vertices[1] - faces[0].vertices[0], separator)) >=
				absolute(dot_product(faces[1].vertices[1] - faces[1].vertices[0], separator));
			u32 face_incident  = 1 - face_reference;

			// @Todo:
			vec2 contact = (phys_a.position + phys_b.position) / 2.0f;

			//
			collisions.push({&phys_a, &phys_b, contact, separator, overlap});
		}
	}
	

	for (u32 i = 0; i < collisions.count; ++i) {
		vec2 const normal = collisions[i].normal;

		Physical_Blob * phys_a = collisions[i].phys_a;
		Physical_Blob * phys_b = collisions[i].phys_b;

		vec2 const radius_a = {0, 0};//collisions[i].contact - phys_a->position;
		vec2 const radius_b = {0, 0};//collisions[i].contact - phys_b->position;

		vec2 contact_velocity = (phys_a->velocity - phys_b->velocity) + (
			cross_product(phys_a->angular_velocity, radius_a) -
			cross_product(phys_b->angular_velocity, radius_b)
		);
		r32 contact_velocity_normal = dot_product(contact_velocity, normal);

		// do not collide separating contacts
		if (contact_velocity_normal <= 0) { continue; }

		// r32 const radius_normal_a = dot_product(radius_a, normal);
		// r32 const radius_normal_b = dot_product(radius_b, normal);
		// r32 const inertia_a = dot_product(radius_a, radius_a) - radius_normal_a * radius_normal_a;
		// r32 const inertia_b = dot_product(radius_b, radius_b) - radius_normal_b * radius_normal_b;

		r32 const inertia_a = dot_product(cross_product(cross_product(radius_a, normal), radius_a), normal);
		r32 const inertia_b = dot_product(cross_product(cross_product(radius_b, normal), radius_b), normal);
		r32 const contact_mass_inverse = (phys_a->inverse_mass + phys_b->inverse_mass) + (
			inertia_a * phys_a->inverse_angular_mass +
			inertia_b * phys_b->inverse_angular_mass
		);
		r32 const contact_mass = 1 / contact_mass_inverse;

		r32 const restitution    = square_root(phys_a->elasticity * phys_b->elasticity);
		r32 const normal_impulse = contact_velocity_normal * contact_mass * (1 + restitution);

		{
			vec2 const normal_impulse_vector = normal * normal_impulse;
			phys_a->add_impulse(-normal_impulse_vector, radius_a);
			phys_b->add_impulse(normal_impulse_vector, radius_b);
		}

		// @Note: it is important due to angular motion
		contact_velocity = (phys_a->velocity - phys_b->velocity) + (
			cross_product(phys_a->angular_velocity, radius_a) -
			cross_product(phys_b->angular_velocity, radius_b)
		);
		contact_velocity_normal = dot_product(contact_velocity, normal);

		vec2 const contact_velocity_tangent_vector  = contact_velocity - normal * contact_velocity_normal;
		r32 const  contact_velocity_tangent_squared = magnitude_squared(contact_velocity_tangent_vector);

		// do not collide tangentially still contacts
		if (contact_velocity_tangent_squared == 0) { continue; }

		r32 const friction  = square_root(phys_a->roughness * phys_b->roughness);
		r32 const stiction  = square_root(phys_a->stickiness * phys_b->stickiness);
		r32 const stillness = square_root(phys_a->stillness * phys_b->stillness);

		r32 const contact_velocity_tangent = square_root(contact_velocity_tangent_squared);
		vec2 tangent_axis = contact_velocity_tangent_vector / contact_velocity_tangent;

		r32 tangent_impulse = contact_velocity_tangent * contact_mass;
		tangent_impulse = (tangent_impulse < normal_impulse * stiction)
			? tangent_impulse * stillness // this is not particularly correct, though
			: normal_impulse  * friction;

		{
			vec2 const tangent_impulse_vector = tangent_axis * tangent_impulse;
			phys_a->add_impulse(-tangent_impulse_vector, radius_a);
			phys_b->add_impulse(tangent_impulse_vector, radius_b);
		}
	}

	for (u32 i = 0; i < collisions.count; ++i) {
		if (collisions[i].overlap < settings.separation_bias) { continue; }
		vec2 separator = collisions[i].normal * collisions[i].overlap;

		Physical_Blob * phys_a = collisions[i].phys_a;
		Physical_Blob * phys_b = collisions[i].phys_b;

		r32 mass = settings.separation_fraction / (phys_a->inverse_mass + phys_b->inverse_mass);
		phys_a->position -= separator * (phys_a->inverse_mass * mass);
		phys_b->position += separator * (phys_b->inverse_mass * mass);
	}
}

/*
% Combined total area of all triangles
total_area = SUM( area(i), i=1:n )
total_mass = SUM( mass(i), i=1:n )
% Combined centroid (center of mass) coordinates
combined_centroid_x = SUM( mass(i)*centroid_x(i), i=1:n)/total_mass
combined_centroid_y = SUM( mass(i)*centroid_y(i), i=1:n)/total_mass
% Each distance to triangle (squared)
centroid_distance_sq(i) = centroid_x(i)*centroid_x(i)+centroid_y(i)*centroid_y(i)
% Combined mass moment of inertia
combined_mmoi = SUM(mmoi(i)+mass(i)*centroid_distance_sq(i), i=1:n)
*/

/*
public static RigidBody2 FromShape(double mass, params Vector2[] polygon)
{
    double area = 0;
    Vector2 center = Vector2.Zero;
    double mmoi = 0;

    int prev = polygon.Length-1;
    for (int index = 0; index < polygon.Length; index++)
    {
        var a = polygon[prev];
        var b = polygon[index];

        var area_step = Vector2.Cross(a, b)/2;
        var center_step = (a+b)/3;
        var mmoi_step = area_step*(Vector2.Dot(a, a)+Vector2.Dot(b, b)+Vector2.Dot(a, b))/6;

        center = (center*area + center_step * area_step)/(area + area_step);
        area += area_step;
        mmoi += mmoi_step;

        prev = index;
    }

    double density = mass/area;
    mmoi *= density;
    mmoi -= mass * Vector2.Dot(center, center);

    return new RigidBody2(mass, mmoi, center);
}
*/

/*
polygon(Vector[] points, double depth, double density)
{
    // Accumulate the following values
    double area = 0.0;
    double mass = 0.0;
    Vector center = [0.0, 0.0];
    double mmoi = 0.0;

    // Take each vertex pair starting from the last-first vertex
    // in order to consider all sides.
    int count = points.Length;
    int prev = count - 1;
    for(int index=0; index<count; index++)
    {
        Vector a = points[prev];
        Vector b = points[index];

        double area_step = TriangleArea(a,b);
        double mass_step = density * area_step * depth;
        Vector center_step = TriangleCenter(a,b);
        double mmoi_step = TriangleMmoi(a,b, mass_step);

        area += area_step;
        center = (mass*center + mass_step*center_step)/(mass+mass_step);
        mass += mass_step;
        mmoi += mmoi_step;

        prev = index;
    }

    // Transfer mass moment of inertia from the origin to the center of mass
    mmoi -= mass*dot(center,center);

    // use area, mass, center and mmoi
}

double TriangleArea(Vector a, Vector b)
{
    return cross(a,b)/2;
}
double TriangleCenter(Vector a, Vector b)
{
    return (a+b)/3;
{
double TriangleMmoi(Vector a, Vector b, double triangleMass)
{
    return triangleMass/6*(dot(a,a)+dot(b.b)+dot(a.b));
}
*/

/*
for (u32 i = 0; i < transformed_points.count; ++i) {
	vec2 const * points = transformed_points_buffer.data + transformed_points[i].offset;
	u32 const    points_count = transformed_points[i].count;

	vec2 centre            = {0, 0};
	r32  area              = 0;
	r32  moment_of_inertia = 0;

	for (u32 point_a = 0; point_a < points_count; ++point_a) {
		u32 point_b = (point_a + 1) % points_count;
		vec2 a = points[point_a];
		vec2 b = points[point_b];

		vec2 triangle_centre            = (a + b) / 3.0f;
		r32  triangle_area              = cross_product(b, a) / 2;
		r32  triangle_moment_of_inertia = triangle_area * (
			dot_product(a, a) + dot_product(b, b) + dot_product(a, b)
		) / 6;

		centre            += (centre * area + triangle_centre * triangle_area) / (area + triangle_area);
		area              += triangle_area;
		moment_of_inertia += triangle_moment_of_inertia;
	}

	Physical_Blob & phys = physicals[i];

	moment_of_inertia *= (1 - area * dot_product(centre, centre)) / (area * phys.inverse_mass);
}
*/
