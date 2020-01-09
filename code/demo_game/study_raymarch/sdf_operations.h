/*
These functions mix or modify results of SDF shape functions.
*/

constexpr inline float sdf_intersection(float first, float second) {
	return max(first, second);
}

constexpr inline float sdf_union(float first, float second) {
	return min(first, second);
}

constexpr inline float sdf_difference(float first, float second) {
	return max(first, -second);
}

constexpr inline float sdf_interpolate(float first, float second, float fraction) {
	return interpolate(first, second, fraction);
}
