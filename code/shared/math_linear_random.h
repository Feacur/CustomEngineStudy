#define SHARED_MATH_LINEAR

#if !defined(SHARED_MATH_RANDOM)
	#error include "shared/math_random.h"
#endif

inline Vector2 random2_01(u32 * state) { return {random_01(state), random_01(state)}; }
inline Vector3 random3_01(u32 * state) { return {random_01(state), random_01(state), random_01(state)}; }
inline Vector4 random4_01(u32 * state) { return {random_01(state), random_01(state), random_01(state), random_01(state)}; }

#define RANDOM_RADIUS_IMPL(T, name)\
inline T name##_radius01(u32 * state) {\
	T point;\
	do {\
		point = name##_01(state) * 2 - 1;\
	} while (magnitude_squared(point) > 1);\
	return point;\
}

RANDOM_RADIUS_IMPL(Vector2, random2)
RANDOM_RADIUS_IMPL(Vector3, random3)
RANDOM_RADIUS_IMPL(Vector4, random4)
#undef RANDOM_RADIUS_IMPL
