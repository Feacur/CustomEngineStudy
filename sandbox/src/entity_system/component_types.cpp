#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/impl/math_linear.h"

#include "component_types.h"

//
// Phys2d
//

void Phys2d::add_impulse(vec2 value, vec2 radius) {
	velocity += value * (movable / mass);
	angular_velocity += cross_product(radius, value) * (movable / (mass * shape));
}

void Phys2d::add_force(vec2 value, vec2 radius) {
	acceleration += value * (movable / mass);
	angular_acceleration += cross_product(radius, value) * (movable / (mass * shape));
}
