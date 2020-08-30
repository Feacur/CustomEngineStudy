#include "engine/api/internal/bytecode.h"
#include "engine/api/internal/renderer.h"
#include "engine/api/internal/component_types.h"
#include "engine/api/internal/entity_system.h"
#include "engine/api/internal/application.h"
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

struct Renderer_Blob {
	custom::Entity    entity;
	Transform const * transform;
	Camera const    * camera;
};

struct Renderable_Blob {
	custom::Entity    entity;
	Transform const * transform;
	Visual const    * visual;
};

void ecs_update_renderer(void) {
	// @Todo: global shaders data
	// custom::renderer::set_uniform(shader, (u32)sandbox::Uniform::resolution, viewport_size);

	ivec2 viewport_size = custom::application::get_viewport_size();
	r32 const aspect = (r32)viewport_size.x / (r32)viewport_size.y;

	// custom::Array<Hierarchy::Link> children(4);

	// @Note: fetch renderers
	u32 renderer_ids_limit = 0;
	custom::Array<Renderer_Blob> renderers(8);
	for (u32 i = 0; i < custom::Entity::instances.count; ++i) {
		custom::Entity entity = custom::Entity::instances[i];
		if (!entity.exists()) { continue; }

		Transform const * transform = entity.get_component<Transform>().get_safe();
		if (!transform) { continue; }

		Camera const * camera = entity.get_component<Camera>().get_safe();
		if (!camera) { continue; }

		renderers.push({entity, transform, camera});
		renderer_ids_limit = max(renderer_ids_limit, entity.id);
	}

	// @Todo: revisit nesting; optimize matrices cache
	custom::Array<mat4> renderer_locals(renderer_ids_limit + 1);
	for (u32 i = 0; i < renderers.count; ++i) {
		Renderer_Blob & renderer = renderers[i];
		renderer_locals.get(renderer.entity.id) = to_matrix(renderer.transform->position, renderer.transform->rotation, renderer.transform->scale);
	}

	// for (u32 i = 0; i < renderers.count; ++i) {
	// 	Renderer_Blob & renderer = renderers[i];
	//
	// 	children.count = 0;
	// 	Hierarchy::fetch_children(renderer.entity, children);
	// 	if (children.count == 0) { continue; }
	//
	// 	mat4 const & mat4_parent = renderer_locals.get(renderer.entity.id);
	// 	for (u32 ci = 0; ci < children.count; ++ci) {
	// 		custom::Entity child = children[ci].entity;
	// 		renderer_locals.get(child.id) = mat_product(renderer_locals.get(child.id), mat4_parent);
	// 	}
	// }

	for (u32 i = 0; i < renderers.count; ++i) {
		Renderer_Blob const & renderer = renderers[i];

		Hierarchy const * hierarchy = renderer.entity.get_component<Hierarchy>().get_safe();
		if (!hierarchy) { continue; }
		if (!hierarchy->parent.exists()) { continue; }
		if (!hierarchy->parent.has_component<Transform>()) { continue; }

		mat4 & renderer_local = renderer_locals.get(renderer.entity.id);
		mat4 const & renderer_parent = renderer_locals.get(hierarchy->parent.id);
		renderer_local = mat_product(renderer_local, renderer_parent);
	}

	// @Todo: revisit sorting
	qsort(renderers.data, renderers.count, sizeof(*renderers.data), [](void const * va, void const * vb) {
		Renderer_Blob const * a = (Renderer_Blob const *)va;
		Renderer_Blob const * b = (Renderer_Blob const *)vb;
		return (a->camera->layer > b->camera->layer) - (a->camera->layer < b->camera->layer);
	});

	// @Note: fetch renderables
	u32 renderable_ids_limit = 0;
	custom::Array<Renderable_Blob> renderables(custom::Entity::instances.count);
	for (u32 i = 0; i < custom::Entity::instances.count; ++i) {
		custom::Entity entity = custom::Entity::instances[i];
		if (!entity.exists()) { continue; }

		Transform const * transform = entity.get_component<Transform>().get_safe();
		if (!transform) { continue; }

		Visual const * visual = entity.get_component<Visual>().get_safe();
		if (!visual) { continue; }

		renderables.push({entity, transform, visual});
		renderable_ids_limit = max(renderable_ids_limit, entity.id);
	}

	// @Todo: revisit nesting; optimize matrices cache
	custom::Array<mat4> renderable_locals(renderable_ids_limit + 1);
	for (u32 i = 0; i < renderables.count; ++i) {
		Renderable_Blob & renderable = renderables[i];
		renderable_locals.get(renderable.entity.id) = to_matrix(renderable.transform->position, renderable.transform->rotation, renderable.transform->scale);
	}

	// for (u32 i = 0; i < renderables.count; ++i) {
	// 	Renderable_Blob & renderable = renderables[i];
	//
	// 	children.count = 0;
	// 	Hierarchy::fetch_children(renderable.entity, children);
	// 	if (children.count == 0) { continue; }
	//
	// 	mat4 const & mat4_parent = renderable_locals.get(renderable.entity.id);
	// 	for (u32 ci = 0; ci < children.count; ++ci) {
	// 		custom::Entity child = children[ci].entity;
	// 		renderable_locals.get(child.id) = mat_product(renderable_locals.get(child.id), mat4_parent);
	// 	}
	// }

	for (u32 i = 0; i < renderables.count; ++i) {
		Renderable_Blob const & renderable = renderables[i];

		Hierarchy const * hierarchy = renderable.entity.get_component<Hierarchy>().get_safe();
		if (!hierarchy) { continue; }
		if (!hierarchy->parent.exists()) { continue; }
		if (!hierarchy->parent.has_component<Transform>()) { continue; }

		mat4 & renderable_local = renderable_locals.get(renderable.entity.id);
		mat4 const & renderable_parent = renderable_locals.get(hierarchy->parent.id);
		renderable_local = mat_product(renderable_local, renderable_parent);
	}

	// @Todo: revisit sorting
	qsort(renderables.data, renderables.count, sizeof(*renderables.data), [](void const * va, void const * vb) {
		Renderable_Blob const * a = (Renderable_Blob const *)va;
		Renderable_Blob const * b = (Renderable_Blob const *)vb;
		return (a->visual->layer > b->visual->layer) - (a->visual->layer < b->visual->layer);
	});

	// @Note: iterate renderers and renderables
	u32 renderable_i = 0;
	custom::Array<Renderable_Blob> relevant_renderables(renderables.count);
	for (u32 camera_i = 0; camera_i < renderers.count; ++camera_i) {
		Renderer_Blob const & renderer = renderers[camera_i];

		mat4 const camera_matrix = mat_product(
			mat_inverse_transform(renderer_locals.get(renderer.entity.id)),
			interpolate(
				mat_persp({renderer.camera->scale, renderer.camera->scale * aspect}, renderer.camera->near, renderer.camera->far),
				mat_ortho({renderer.camera->scale, renderer.camera->scale * aspect}, renderer.camera->near, renderer.camera->far),
				renderer.camera->ortho
			)
		);

		u32 last_renderable_i = renderable_i;
		for (; last_renderable_i < renderables.count; ++last_renderable_i) {
			Renderable_Blob const & renderable = renderables[last_renderable_i];
			if (renderable.visual->layer != renderer.camera->layer) { break; }
		}

		// @Todo: revisit sorting;
		//        - account distance
		//        - account mesh, too?
		qsort(renderables.data + renderable_i, (last_renderable_i - renderable_i), sizeof(*renderables.data), [](void const * va, void const * vb) {
			Renderable_Blob const * a = (Renderable_Blob const *)va;
			Renderable_Blob const * b = (Renderable_Blob const *)vb;
			return (a->visual->shader.ref.id > b->visual->shader.ref.id) - (a->visual->shader.ref.id < b->visual->shader.ref.id);
		});

		custom::renderer::clear(renderer.camera->clear);

		u32 shader_id = custom::empty_ref.id;
		for (; renderable_i < last_renderable_i; ++renderable_i) {
			Renderable_Blob const & renderable = renderables[renderable_i];

			mat4 const transform_matrix = renderable_locals.get(renderable.entity.id);

			if (shader_id != renderable.visual->shader.ref.id) {
				shader_id = renderable.visual->shader.ref.id;
				custom::renderer::set_shader(renderable.visual->shader.ref);
				custom::renderer::set_uniform(renderable.visual->shader.ref, (u32)sandbox::Uniform::View_Projection, camera_matrix);
			}

			custom::graphics::unit_id unit = custom::renderer::make_unit(renderable.visual->texture.ref);
			custom::renderer::set_uniform(renderable.visual->shader.ref, (u32)sandbox::Uniform::Texture, unit);
			custom::renderer::set_uniform(renderable.visual->shader.ref, (u32)sandbox::Uniform::Transform, transform_matrix);
			custom::renderer::set_mesh(renderable.visual->mesh.ref);
			custom::renderer::draw();
		}
	}
}

}
