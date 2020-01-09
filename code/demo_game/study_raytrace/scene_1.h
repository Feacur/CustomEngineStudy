static Plane const plane = {{0, 0, 0}, {0, 1, 0}};
static Sphere const sphere = {0, 6, 0, 10};

Raytrace_Result raytrace_scene(Ray3 ray) {
	auto raytrace_result_plane = raytrace_plane(plane, ray);
	raytrace_result_plane.id = 1;

	auto raytrace_result_sphere = raytrace_sphere(sphere, ray);
	raytrace_result_sphere.id = 2;

	Raytrace_Result raytrace_result = {};
	if (raytrace_result_plane.depth < raytrace_result_sphere.depth) {
		if (raytrace_result_plane.depth < too_far) {
			raytrace_result = raytrace_result_plane;
		}
	}
	else {
		if (raytrace_result_sphere.depth < too_far) {
			raytrace_result = raytrace_result_sphere;
		}
	}

	return raytrace_result;
}
