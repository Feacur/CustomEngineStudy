Vector3 texture_sky_emission(Ray3 ray) {
	static Vector3 const sky_color_1 = {1, 1, 1};
	static Vector3 const sky_color_2 = {0.5f, 0.7f, 1};
	float fraction = (ray.direction.y + 1) / 2;
	return interpolate(sky_color_1, sky_color_2, fraction);
}
