/*
These functions calculate the shortest distance to the surface of the shapes from a given point.
*/

inline r32 sdf_sphere(Vector3 point, r32 radius) {
	return magnitude(point) - radius;
}

inline r32 sdf_box(Vector3 point, Vector3 size) {
	Vector3 distance = absolute(point) - size;
	return max(distance);
}

inline r32 sdf_torus(Vector3 point, r32 radius, r32 thickness) {
	Vector2 distance = {magnitude(point.xy) - radius, point.z};
	return magnitude(distance) - thickness;
}

inline r32 sdf_cylinder(Vector3 point, r32 radius, r32 height) {
	Vector2 distance = {magnitude(point.xy) - radius, absolute(point.z) - height};
	return max(distance);
}

inline r32 sdf_plane(Vector3 point, Vector3 normal) {
	return dot_product(point, normal);
}
