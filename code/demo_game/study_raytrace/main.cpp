// https://www.scratchapixel.com/
// https://blog.demofox.org/2017/01/09/raytracing-reflection-refraction-fresnel-total-internal-reflection-and-beers-law/
// https://blog.demofox.org/2016/01/16/making-a-ray-traced-snake-game-in-shadertoy/

// https://blog.demofox.org/2016/09/21/path-tracing-getting-started-with-diffuse-and-emissive/

/*
Let's lay out steps we need:

1) Trace a ray to a surface
2) Get albedo color

3) Trace reflections and refractions

4) Apply lighting
*/

#include "demo_game/main_common.h"
#include "demo_game/platform_globals.h"

#include "shared/shapes.h"

#include "data.h"
#include "code.h"

static float const too_far = 1000.0f;
struct Raytrace_Result {
	int32 id;
	float depth;
	Vector3 point;
	Vector3 normal;
};

#include "plane.h"
#include "disc.h"
#include "square.h"
#include "sphere.h"

inline float fresnel_schlick(Vector3 incident, Vector3 normal, float n1, float n2) {
	float incident_cosine = -dot_product(incident, normal);
	if (n1 > n2) {
		float factor = n1 / n2;
		float incident_sine_squared  = 1 - incident_cosine * incident_cosine;
		float refracted_sine_squared = incident_sine_squared * factor * factor;
		if (refracted_sine_squared > 1) {
			// total internal reflection
			return 1.0f;
		}
		incident_cosine = square_root(1 - refracted_sine_squared);
	}
	
	float zero_factor_sqrt = (n1 - n2) / (n1 + n2);
	float cosine_factor = 1 - incident_cosine;
	float fraction = cosine_factor * cosine_factor * cosine_factor * cosine_factor * cosine_factor;
	return interpolate(zero_factor_sqrt * zero_factor_sqrt, 1.0f, fraction);
}

static Vector3 const ambient = {0.1f, 0.1f, 0.1f};
static Vector3 const light = normalize(vec3(1.0f, 2.0f, 1.0f));
static Vector3 const light_color = {1, 1, 1};

static float const REFRACTIVE_INDEX_AIR = 1.00029f;
static float const REFRACTIVE_INDEX_GLASS = 1.125f;

static Vector3 const absorption_air = {0.004f, 0.003f, 0.002f};
static Vector3 const absorption_glass = {0.4f, 0.05f, 0.2f};

#include "scene_1.h"
#include "scene_common_raytrace.h"

inline Vector3 get_albedo_color(Raytrace_Result raytrace_result) {
	if (raytrace_result.id == 1) {
		float pattern = fmodf(floorf(raytrace_result.point.x * 0.2f) + floorf(raytrace_result.point.z * 0.2f), 2.0f);
		float brightness = (absolute(pattern) < 1) ? 1.0f : 0.4f;
		return {brightness, brightness, brightness};	
	}
	else if (raytrace_result.id == 2) {
		return {0.0f, 0.0f, 0.0f};
	}
	
	return {0.8f, 0.8f, 0.8f};
}

inline Vector3 apply_light(Vector3 albedo, Raytrace_Result raytrace_result, Vector3 view_direction) {
	auto result = (albedo * ambient);

	auto attenuation = raytrace_attenuation(raytrace_result.point, raytrace_result.normal, light);
	if (attenuation == 0) {
		return result;
	}
	
	float diffuse_lighting = max(0.0f, dot_product(raytrace_result.normal, light));
	result = result + (albedo * light_color) * diffuse_lighting;
	
	float specular_power = 100;
	if (specular_power > 0) {
		auto reflection = reflect(view_direction, raytrace_result.normal, 2.0f);
		float specular_lighting_base = max(0.0f, dot_product(reflection, light));
		float specular_lighting = powf(specular_lighting_base, specular_power);
		result = result + light_color * specular_lighting;
	}

	return result;
}

inline Vector3 raytrace_scene_color(Ray3 ray, int id, int reflection_limit, int refraction_limit) {
	auto raytrace_result = raytrace_scene(ray);
	auto albedo = get_albedo_color(raytrace_result);
	if (raytrace_result.id == 0) { return albedo; }

	bool from_outside = (id == 0);
	float refractive_index_from = from_outside ? REFRACTIVE_INDEX_AIR : REFRACTIVE_INDEX_GLASS;
	float refractive_index_to = from_outside ? REFRACTIVE_INDEX_GLASS : REFRACTIVE_INDEX_AIR;
	
	auto absorption_power_base = from_outside ? absorption_air : absorption_glass;
	auto absorption_power = absorption_power_base * magnitude(raytrace_result.point - ray.origin);
	Vector3 absorption = {
		(float)exp(-absorption_power.x),
		(float)exp(-absorption_power.y),
		(float)exp(-absorption_power.z)
	};

	float fresnel = fresnel_schlick(
		ray.direction, raytrace_result.normal,
		refractive_index_from, refractive_index_to
	);

	Vector3 result = {0, 0, 0};
	if (reflection_limit > 0) {
		float minReflectivity = (raytrace_result.id == 1) ? 0.1f : 0.01f;
		float maxReflectivity = (raytrace_result.id == 1) ? 0.2f : 1.0f;
		float reflectivity = interpolate(minReflectivity, maxReflectivity, fresnel);
		if (reflectivity > 0.001f) {
			auto reflection_point = raytrace_result.point + raytrace_result.normal * 0.1f;
			auto reflection_direction = reflect(ray.direction, raytrace_result.normal, 2.0f);
			
			auto reflection_color = raytrace_scene_color(
				{reflection_point, reflection_direction},
				id,
				reflection_limit - 1,
				refraction_limit
			) * reflectivity;
			
			result = result + reflection_color;
		}
	}

	if (refraction_limit > 0) {
		float minTransmittance = (raytrace_result.id == 1) ? 0.0f : 0.0f;
		float maxTransmittance = (raytrace_result.id == 1) ? 0.0f : 0.99f;
		float transmittance = interpolate(maxTransmittance, minTransmittance, fresnel);
		if (transmittance > 0.001f) {
			auto refraction_point = raytrace_result.point - raytrace_result.normal * 0.1f;
			auto refraction_direction = refract(
				ray.direction, raytrace_result.normal,
				refractive_index_from / refractive_index_to
			);
			
			auto refraction_color = raytrace_scene_color(
				{refraction_point, refraction_direction},
				raytrace_result.id,
				reflection_limit,
				refraction_limit - 1
			) * transmittance;
			
			result = result + refraction_color;
		}
	}

	auto color = apply_light(albedo, raytrace_result, ray.direction);
	color = (color * absorption);

	return result + color;
}

static Vector3 const axis = {cosine(pi / 3), sine(pi / 3), 0};
static float rotation_radians = 0;
API_C API_DLL GAME_UPDATE(game_update) {
	globals::cache(platform_data);

	auto delta_time = globals::get_delta_seconds();
	rotation_radians += 0.25f * pi * delta_time;
	if (rotation_radians > tau) {
		rotation_radians -= tau;
	}
	
	// request render buffer size
	platform_data->render_settings.target_size = {320, 180};
	platform_data->render_settings.size_mode = Render_Settings::Size_Mode::Game;
	platform_data->render_settings.stretch_mode = Render_Settings::Stretch_Mode::Fractional;
}

API_C API_DLL GAME_RENDER(game_render) {
	auto image = globals::render_buffer_f;
	clear_buffer(image, {0, 0, 0, 0});

	Ray3 ray = {};

	Quaternion origin_rotation = quaternion_from_radians({0, rotation_radians, 0});
	ray.origin = quaternion_rotate_vector(origin_rotation, {0, 20, -25});
	
	Quaternion direction_rotation = quaternion_from_radians({pi/6, rotation_radians, 0});

	for (int32 y = 0; y < image.size.y; ++y) {
		auto destination_line = image.data + y * image.size.x;
		float offset_y = (float)(y - image.size.y / 2) / image.size.y;
		for (int32 x = 0; x < image.size.x; ++x) {
			auto destination = destination_line + x;
			float offset_x = (float)(x - image.size.x / 2) / image.size.y;
			
			Vector3 screen_point = {offset_x, offset_y, 1};
			ray.direction = quaternion_rotate_vector(
				direction_rotation,
				screen_point / magnitude(screen_point)
			);

			auto color3 = raytrace_scene_color(ray, 0, 4, 4);
			color3 = min(color3, {1, 1, 1});
			*destination = {color3, 1};
		}
	}
	platform_data->render_buffer_image_f.exposure = 1;
}

// API_C API_DLL GAME_OUTPUT_SOUND(game_output_sound) {
// }