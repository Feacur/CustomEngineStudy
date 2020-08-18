#include "ecs_renderer.h"

#include "engine/api/internal/bytecode.h"
#include "engine/api/internal/renderer.h"
#include "engine/api/internal/component_types.h"
#include "engine/api/internal/application.h"
#include "engine/api/graphics_params.h"
#include "engine/impl/array.h"
#include "engine/impl/math_linear.h"

#include "../asset_system/uniform_ids.h"
#include "component_types.h"

// @Note: thoughts on factoring code for a renderer
//        - batch together materials/objects with the same shaders
//        - proactively bind textures pending to rendering
//        - batch and rebind if running out of slots/units

namespace sandbox {
namespace ecs_renderer {

struct Camera_Blob {
	custom::Entity entity;
	Transform transform;
	Camera cam;
};

void update() {
	// @Todo: sort cameras
	custom::Array<Camera_Blob> cameras;
	for (u32 i = 0; i < custom::Entity::instances.count; ++i) {
		custom::Entity entity = custom::Entity::instances[i];
		if (!entity.exists()) { continue; }

		Transform * transform = entity.get_component<Transform>().get_safe();
		if (!transform) { continue; }

		Camera * camera = entity.get_component<Camera>().get_safe();
		if (!camera) { continue; }

		cameras.push({entity, *transform, *camera});
	}

	// @Todo: prefetch all relevant components into a contiguous array?
	for (u32 camera_i = 0; camera_i < cameras.count; ++camera_i) {
		Camera_Blob const & camera = cameras[camera_i];

		ivec2 viewport_size = custom::application::get_viewport_size();
		r32 const aspect = (r32)viewport_size.x / (r32)viewport_size.y;

		mat4 camera_matrix = to_matrix(camera.transform.position, camera.transform.rotation, camera.transform.scale);
		camera_matrix = mat_product(
			mat_inverse_transform(camera_matrix),
			interpolate(
				mat_persp({camera.cam.scale, camera.cam.scale * aspect}, camera.cam.near, camera.cam.far),
				mat_ortho({camera.cam.scale, camera.cam.scale * aspect}, camera.cam.near, camera.cam.far),
				camera.cam.ortho
			)
		);

		custom::renderer::clear(camera.cam.clear);
		for (u32 i = 0; i < custom::Entity::instances.count; ++i) {
			custom::Entity entity = custom::Entity::instances[i];
			if (!entity.exists()) { continue; }

			Visual * visual = entity.get_component<Visual>().get_safe();
			if (!visual) { continue; }

			Transform * transform = entity.get_component<Transform>().get_safe();
			if (!transform) { continue; }

			mat4 transform_matrix = to_matrix(
				transform->position, transform->rotation, transform->scale
			);

			//
			custom::renderer::set_shader(visual->shader);
			custom::renderer::set_uniform(visual->shader, (u32)sandbox::Uniform::Texture, visual->texture);
			custom::renderer::set_uniform(visual->shader, (u32)sandbox::Uniform::View_Projection, camera_matrix);
			custom::renderer::set_uniform(visual->shader, (u32)sandbox::Uniform::Transform, transform_matrix);
			// custom::renderer::set_uniform(visual->shader, (u32)sandbox::Uniform::resolution, viewport_size);
			custom::renderer::set_mesh(visual->mesh);
			custom::renderer::draw();
		}
	}
}

}}
