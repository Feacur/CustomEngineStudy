#pragma once
#include "engine/impl/math_linear.h"
#include "engine/impl/math_random.h"

inline vec2 random2_01(u32 * state) { return {random_01(state), random_01(state)}; }
inline vec3 random3_01(u32 * state) { return {random_01(state), random_01(state), random_01(state)}; }
inline vec4 random4_01(u32 * state) { return {random_01(state), random_01(state), random_01(state), random_01(state)}; }

#define RANDOM_RADIUS_IMPL(T, name)\
	inline T name##_radius01(u32 * state) {\
		T point;\
		do {\
			point = name##_01(state) * 2.0f - 1.0f;\
		} while (magnitude_squared(point) > 1.0f);\
		return point;\
	}\

RANDOM_RADIUS_IMPL(vec2, random2)
RANDOM_RADIUS_IMPL(vec3, random3)
RANDOM_RADIUS_IMPL(vec4, random4)
#undef RANDOM_RADIUS_IMPL
