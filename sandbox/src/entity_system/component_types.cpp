#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/impl/math_linear.h"

#include "component_types.h"

//
// Phys2d
//

void Phys2d::add_impulse(vec2 value) {
	velocity += value * (dynamic / mass);
}

void Phys2d::add_force(vec2 value) {
	acceleration += value * (dynamic / mass);
}
