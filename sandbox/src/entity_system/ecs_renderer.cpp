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

struct Renderer_Blob {
	custom::Entity entity;
	Transform transform;
	Camera camera;
};

struct Renderable_Blob {
	custom::Entity entity;
	Transform transform;
	Visual visual;
};

void update() {
	// @Todo: global shaders data
	// custom::renderer::set_uniform(shader, (u32)sandbox::Uniform::resolution, viewport_size);

	// @Todo: sort renderers by depth
	custom::Array<Renderer_Blob> cameras;
	for (u32 i = 0; i < custom::Entity::instances.count; ++i) {
		custom::Entity entity = custom::Entity::instances[i];
		if (!entity.exists()) { continue; }

		Transform * transform = entity.get_component<Transform>().get_safe();
		if (!transform) { continue; }

		Camera * camera = entity.get_component<Camera>().get_safe();
		if (!camera) { continue; }

		cameras.push({entity, *transform, *camera});
	}

	// @Change: sort renderables by layer instead of prefetching them into an array of relevant ones?
	custom::Array<Renderable_Blob> all_renderables;
	for (u32 i = 0; i < custom::Entity::instances.count; ++i) {
		custom::Entity entity = custom::Entity::instances[i];
		if (!entity.exists()) { continue; }

		Transform * transform = entity.get_component<Transform>().get_safe();
		if (!transform) { continue; }

		Visual * visual = entity.get_component<Visual>().get_safe();
		if (!visual) { continue; }

		all_renderables.push({entity, *transform, *visual});
	}

	custom::Array<Renderable_Blob> relevant_renderables(all_renderables.count);
	for (u32 camera_i = 0; camera_i < cameras.count; ++camera_i) {
		Renderer_Blob const & renderer = cameras[camera_i];

		ivec2 viewport_size = custom::application::get_viewport_size();
		r32 const aspect = (r32)viewport_size.x / (r32)viewport_size.y;

		mat4 camera_matrix = to_matrix(renderer.transform.position, renderer.transform.rotation, renderer.transform.scale);
		camera_matrix = mat_product(
			mat_inverse_transform(camera_matrix),
			interpolate(
				mat_persp({renderer.camera.scale, renderer.camera.scale * aspect}, renderer.camera.near, renderer.camera.far),
				mat_ortho({renderer.camera.scale, renderer.camera.scale * aspect}, renderer.camera.near, renderer.camera.far),
				renderer.camera.ortho
			)
		);

		// prefetch
		relevant_renderables.count = 0;
		for (u32 i = 0; i < all_renderables.count; ++i) {
			Renderable_Blob const & renderable = all_renderables[i];
			if (renderer.camera.layer == renderable.visual.layer) {
				relevant_renderables.push(renderable);
			}
		}

		custom::renderer::clear(renderer.camera.clear);

		// @Todo: sort by material; additionally by mesh, too (?)
		for (u32 i = 0; i < relevant_renderables.count; ++i) {
			Renderable_Blob const & renderable = relevant_renderables[i];

			mat4 transform_matrix = to_matrix(
				renderable.transform.position, renderable.transform.rotation, renderable.transform.scale
			);

			custom::renderer::set_shader(renderable.visual.shader);
			custom::renderer::set_uniform(renderable.visual.shader, (u32)sandbox::Uniform::Texture, renderable.visual.texture);
			custom::renderer::set_uniform(renderable.visual.shader, (u32)sandbox::Uniform::View_Projection, camera_matrix);
			custom::renderer::set_uniform(renderable.visual.shader, (u32)sandbox::Uniform::Transform, transform_matrix);
			custom::renderer::set_mesh(renderable.visual.mesh);
			custom::renderer::draw();
		}
	}
}

}}
