/*
These functions calculate the shortest distance to the surface of the shapes from a given point.
*/

inline float sdf_sphere(Vector3 point, float radius) {
	return magnitude(point) - radius;
}

inline float sdf_box(Vector3 point, Vector3 size) {
	Vector3 distance = absolute(point) - size;
	return max(distance);
}

inline float sdf_torus(Vector3 point, float radius, float thickness) {
	Vector2 distance = {magnitude(point.xy) - radius, point.z};
	return magnitude(distance) - thickness;
}

inline float sdf_cylinder(Vector3 point, float radius, float height) {
	Vector2 distance = {magnitude(point.xy) - radius, absolute(point.z) - height};
	return max(distance);
}

inline float sdf_plane(Vector3 point, Vector3 normal) {
	return dot_product(point, normal);
}
