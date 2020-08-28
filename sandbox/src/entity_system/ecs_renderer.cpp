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

#include <stdlib.h>

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

	ivec2 viewport_size = custom::application::get_viewport_size();
	r32 const aspect = (r32)viewport_size.x / (r32)viewport_size.y;

	// @Todo: sort renderers by depth
	custom::Array<Renderer_Blob> renderers;
	for (u32 i = 0; i < custom::Entity::instances.count; ++i) {
		custom::Entity entity = custom::Entity::instances[i];
		if (!entity.exists()) { continue; }

		Transform * transform = entity.get_component<Transform>().get_safe();
		if (!transform) { continue; }

		Camera * camera = entity.get_component<Camera>().get_safe();
		if (!camera) { continue; }

		renderers.push({entity, *transform, *camera});
	}

	// @Todo: revisit sorting
	qsort(renderers.data, renderers.count, sizeof(*renderers.data), [](void const * va, void const * vb) {
		Renderer_Blob const * a = (Renderer_Blob const *)va;
		Renderer_Blob const * b = (Renderer_Blob const *)vb;
		return (a->camera.layer > b->camera.layer) - (a->camera.layer < b->camera.layer);
	});

	// @Change: sort renderables by layer instead of prefetching them into an array of relevant ones?
	custom::Array<Renderable_Blob> renderables;
	for (u32 i = 0; i < custom::Entity::instances.count; ++i) {
		custom::Entity entity = custom::Entity::instances[i];
		if (!entity.exists()) { continue; }

		Transform * transform = entity.get_component<Transform>().get_safe();
		if (!transform) { continue; }

		Visual * visual = entity.get_component<Visual>().get_safe();
		if (!visual) { continue; }

		renderables.push({entity, *transform, *visual});
	}

	// @Todo: revisit sorting
	qsort(renderables.data, renderables.count, sizeof(*renderables.data), [](void const * va, void const * vb) {
		Renderable_Blob const * a = (Renderable_Blob const *)va;
		Renderable_Blob const * b = (Renderable_Blob const *)vb;
		return (a->visual.layer > b->visual.layer) - (a->visual.layer < b->visual.layer);
	});

	u32 renderable_i = 0;
	custom::Array<Renderable_Blob> relevant_renderables(renderables.count);
	for (u32 camera_i = 0; camera_i < renderers.count; ++camera_i) {
		Renderer_Blob const & renderer = renderers[camera_i];

		mat4 camera_matrix = to_matrix(renderer.transform.position, renderer.transform.rotation, renderer.transform.scale);
		camera_matrix = mat_product(
			mat_inverse_transform(camera_matrix),
			interpolate(
				mat_persp({renderer.camera.scale, renderer.camera.scale * aspect}, renderer.camera.near, renderer.camera.far),
				mat_ortho({renderer.camera.scale, renderer.camera.scale * aspect}, renderer.camera.near, renderer.camera.far),
				renderer.camera.ortho
			)
		);

		u32 last_renderable_i = renderable_i;
		for (; last_renderable_i < renderables.count; ++last_renderable_i) {
			Renderable_Blob const & renderable = renderables[last_renderable_i];
			if (renderable.visual.layer != renderer.camera.layer) { break; }
		}

		// @Todo: revisit sorting;
		//        - account distance
		//        - account mesh, too?
		qsort(renderables.data + renderable_i, (last_renderable_i - renderable_i), sizeof(*renderables.data), [](void const * va, void const * vb) {
			Renderable_Blob const * a = (Renderable_Blob const *)va;
			Renderable_Blob const * b = (Renderable_Blob const *)vb;
			return (a->visual.shader.ref.id > b->visual.shader.ref.id) - (a->visual.shader.ref.id < b->visual.shader.ref.id);
		});

		custom::renderer::clear(renderer.camera.clear);

		u32 shader_id = custom::empty_ref.id;
		for (; renderable_i < last_renderable_i; ++renderable_i) {
			Renderable_Blob const & renderable = renderables[renderable_i];

			mat4 transform_matrix = to_matrix(
				renderable.transform.position, renderable.transform.rotation, renderable.transform.scale
			);

			if (shader_id != renderable.visual.shader.ref.id) {
				shader_id = renderable.visual.shader.ref.id;
				custom::renderer::set_shader(renderable.visual.shader.ref);
				custom::renderer::set_uniform(renderable.visual.shader.ref, (u32)sandbox::Uniform::View_Projection, camera_matrix);
			}

			custom::graphics::unit_id unit = custom::renderer::make_unit(renderable.visual.texture.ref);
			custom::renderer::set_uniform(renderable.visual.shader.ref, (u32)sandbox::Uniform::Texture, unit);
			custom::renderer::set_uniform(renderable.visual.shader.ref, (u32)sandbox::Uniform::Transform, transform_matrix);
			custom::renderer::set_mesh(renderable.visual.mesh.ref);
			custom::renderer::draw();
		}
	}
}

}}
