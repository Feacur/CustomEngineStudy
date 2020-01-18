/*
These functions mix or modify results of SDF shape functions.
*/

constexpr inline r32 sdf_intersection(r32 first, r32 second) {
	return max(first, second);
}

constexpr inline r32 sdf_union(r32 first, r32 second) {
	return min(first, second);
}

constexpr inline r32 sdf_difference(r32 first, r32 second) {
	return max(first, -second);
}

constexpr inline r32 sdf_interpolate(r32 first, r32 second, r32 fraction) {
	return interpolate(first, second, fraction);
}
