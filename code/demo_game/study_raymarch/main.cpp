// http://jamie-wong.com/2016/07/15/ray-marching-signed-distance-functions/
// http://iquilezles.org/www/articles/distfunctions/distfunctions.htm
// http://www.iquilezles.org/www/articles/rmshadows/rmshadows.htm
// http://www.iquilezles.org/www/articles/smin/smin.htm

#include "demo_game/main_common.h"
#include "demo_game/platform_globals.h"

#include "shared/math_shapes.h"

#include "data.h"
#include "code.h"

#include "sdf_shapes.h"
#include "sdf_operations.h"

#include "scene_1.h"
#include "scene_common_sdf.h"
#include "scene_common_raymarch.h"

Vector3 apply_light(Vector3 albedo, Ray3 ray, Vector3 point, Vector3 normal, r32 specular_power) {
	static Vector3 const ambient = {0.1f, 0.1f, 0.1f};
	static Vector3 const light = normalize(vec3(1.0f, 2.0f, 1.0f));
	static Vector3 const light_color = {1, 1, 1};
	static r32 const depth_limit = 1000;

	auto result = (albedo * ambient);
	
	r32 attenuation = raymarch_attenuation(point, normal, light, 16, depth_limit);

	r32 diffuse_lighting = max(0.0f, dot_product(normal, light)) * attenuation;
	result = result + (albedo * light_color) * diffuse_lighting;
	
	if (specular_power > 0) {
		auto reflection = reflect(ray.direction, normal, 2.0f);
		r32 specular_lighting_base = max(0.0f, dot_product(reflection, light));
		r32 specular_lighting = powf(specular_lighting_base, specular_power) * attenuation;
		result = result + light_color * specular_lighting;
	}

	return min(result, {1, 1, 1});
}

Vector4 raymarch_color(Ray3 ray) {
	static r32 const depth_limit = 1000;
	Raymarch_Result raymarch_result = raymarch_scene(ray, 256, depth_limit);

	Vector4 color = {0, 0, 0, 0};
	if (raymarch_result.depth < depth_limit) {
		Vector3 point = ray.origin + ray.direction * raymarch_result.depth;
		Vector3 normal = sdf_normal(point);
		Vector3 reflection = reflect(ray.direction, normal, 2.0f);

		if (normal.y > 0.99f && point.y < 0.2f) {
			r32 pattern = fmodf(floorf(point.x * 0.2f) + floorf(point.z * 0.2f), 2.0f);
			r32 brightness = (0.3f + pattern * 0.1f);
			color.xyz = {brightness, brightness, brightness};	
		}
		else {
			color.xyz = {1, 0, 0};
		}

		color.xyz = apply_light(color.xyz, ray, point, normal, 100);
		
		color.w = 1;
	}

	return color;
}

static r32 rotation_radians = 0;
extern "C" CUSTOM_DLL GAME_UPDATE(game_update) {
	globals::cache(platform_data);

	auto delta_time = globals::get_delta_seconds();

	rotation_radians += 0.25f * pi * delta_time;
	if (rotation_radians > tau) {
		rotation_radians -= tau;
	}

	static r32 fraction_direction = 1;
	fraction += fraction_direction * delta_time;
	if (fraction < 0) {
		fraction = 0;
		fraction_direction = 1;
	}
	else if (fraction > 1) {
		fraction = 1;
		fraction_direction = -1;
	}
	
	// request render buffer size
	platform_data->render_settings.target_size = {320, 180};
	platform_data->render_settings.size_mode = Render_Settings::Size_Mode::Game;
	platform_data->render_settings.stretch_mode = Render_Settings::Stretch_Mode::Fractional;
}

extern "C" CUSTOM_DLL GAME_RENDER(game_render) {
	auto image = globals::render_buffer_f;
	clear_buffer(image, {0, 0, 0, 0});

	Ray3 ray = {};

	Quaternion origin_rotation = quaternion_from_radians({0, rotation_radians, 0});
	ray.origin = quaternion_rotate_vector(origin_rotation, {0, 20, -25});
	
	Quaternion direction_rotation = quaternion_from_radians({pi/6, rotation_radians, 0});
	
	for (s32 y = 0; y < image.size.y; ++y) {
		auto destination_line = image.data + y * image.size.x;
		r32 offset_y = (r32)(y - image.size.y / 2) / image.size.y;
		for (s32 x = 0; x < image.size.x; ++x) {
			auto destination = destination_line + x;
			r32 offset_x = (r32)(x - image.size.x / 2) / image.size.y;
			
			Vector3 screen_point = {offset_x, offset_y, 1};
			ray.direction = quaternion_rotate_vector(
				direction_rotation,
				screen_point / magnitude(screen_point)
			);

			Vector4 color = raymarch_color(ray);
			*destination = color;
		}
	}
	platform_data->render_buffer_image_f.exposure = 1;
}

// extern "C" CUSTOM_DLL GAME_OUTPUT_SOUND(game_output_sound) {
// }
