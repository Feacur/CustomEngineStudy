inline r32 shape_get_distance(Shape shape, Ray3 ray) {
	if (shape.type == Shape::Type::Sphere) {
		return hit_sphere(shape.sphere, ray);
	}
	if (shape.type == Shape::Type::Aabb) {
		return hit_aabb(shape.aabb, ray);
	}
	return -1;
}

inline Vector3 shape_get_normal(Shape shape, Vector3 hit) {
	if (shape.type == Shape::Type::Sphere) {
		return get_sphere_normal(shape.sphere, hit);
	}
	if (shape.type == Shape::Type::Aabb) {
		return get_aabb_normal(shape.aabb, hit);
	}
	return {};
}

inline Aabb3 shape_get_aabb(Shape shape) {
	if (shape.type == Shape::Type::Sphere) {
		return {shape.sphere.xyz, vec3(2, 2, 2) * shape.sphere.w};
	}
	if (shape.type == Shape::Type::Aabb) {
		return shape.aabb;
	}
	return {};
}
