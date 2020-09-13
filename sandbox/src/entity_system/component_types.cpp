#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/impl/math_linear.h"

#include "component_types.h"

//
// Phys2d
//

void Phys2d::add_impulse(vec2 value, vec2 radius) {
	velocity += value * (dynamic / mass);
	angular_velocity += cross_product(radius, value) * (dynamic / (mass * shape));
}

void Phys2d::add_force(vec2 value, vec2 radius) {
	acceleration += value * (dynamic / mass);
	angular_acceleration += cross_product(radius, value) * (dynamic / (mass * shape));
}
