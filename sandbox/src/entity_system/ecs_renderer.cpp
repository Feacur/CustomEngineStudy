#include "engine/api/internal/names_lookup.h"
#include "engine/api/internal/bytecode.h"
#include "engine/api/internal/renderer.h"
#include "engine/api/internal/component_types.h"
#include "engine/api/internal/entity_system.h"
#include "engine/api/internal/application.h"
#include "engine/impl/array.h"
#include "engine/impl/math_linear.h"

#include "component_types.h"

#include <stdlib.h>

// @Note: thoughts on factoring code for a renderer
//        - batch together materials/objects with the same shaders
//        - proactively bind textures pending to rendering
//        - batch and rebind if running out of slots/units

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

static void ecs_update_renderer_internal(custom::Array<Renderer_Blob> & renderers, custom::Array<Renderable_Blob> & renderables);

//
//
//

namespace sandbox {

void ecs_update_renderer(void) {
	custom::Array<Renderer_Blob> renderers(8);
	for (u32 i = 0; i < custom::Entity::state.instances.count; ++i) {
		custom::Entity entity = custom::Entity::state.instances[i];
		if (!entity.exists()) { continue; }

		Transform const * transform = entity.get_component<Transform>().get_safe();
		if (!transform) { continue; }

		Camera const * camera = entity.get_component<Camera>().get_safe();
		if (!camera) { continue; }

		renderers.push({entity, transform, camera});
	}

	custom::Array<Renderable_Blob> renderables(custom::Entity::state.instances.count);
	for (u32 i = 0; i < custom::Entity::state.instances.count; ++i) {
		custom::Entity entity = custom::Entity::state.instances[i];
		if (!entity.exists()) { continue; }

		Transform const * transform = entity.get_component<Transform>().get_safe();
		if (!transform) { continue; }

		Visual const * visual = entity.get_component<Visual>().get_safe();
		if (!visual) { continue; }

		renderables.push({entity, transform, visual});
	}

	ecs_update_renderer_internal(renderers, renderables);
}

}

//
//
//

static void ecs_update_renderer_internal(custom::Array<Renderer_Blob> & renderers, custom::Array<Renderable_Blob> & renderables) {
	static u32 const u_Resolution      = custom::uniform_names.store_string("u_Resolution", custom::empty_index);
	static u32 const u_View_Projection = custom::uniform_names.store_string("u_View_Projection", custom::empty_index);
	static u32 const u_Transform       = custom::uniform_names.store_string("u_Transform", custom::empty_index);
	static u32 const u_Texture         = custom::uniform_names.store_string("u_Texture", custom::empty_index);
	static u32 const u_Color           = custom::uniform_names.store_string("u_Color", custom::empty_index);
	static u32 const u_Z               = custom::uniform_names.store_string("u_Z", custom::empty_index);

	ivec2 viewport_size = custom::application::get_viewport_size();
	r32 const aspect = (r32)viewport_size.x / (r32)viewport_size.y;

	// @Todo: global shaders data
	// custom::renderer::set_uniform(shader, u_Resolution, viewport_size);

	//
	// prepare renderers
	//

	u32 renderer_ids_limit = 0;
	for (u32 i = 0; i < renderers.count; ++i) {
		renderer_ids_limit = max(renderer_ids_limit, renderers[i].entity.id);
	}

	custom::Array<Transform> renderer_locals(renderer_ids_limit + 1);
	for (u32 i = 0; i < renderers.count; ++i) {
		Renderer_Blob & renderer = renderers[i];
		renderer_locals.get(renderer.entity.id) = *renderer.transform;
	}

	// @Todo: revisit nesting
	for (u32 i = 0; i < renderers.count; ++i) {
		Renderer_Blob const & renderer = renderers[i];

		Hierarchy const * hierarchy = renderer.entity.get_component<Hierarchy>().get_safe();
		if (!hierarchy) { continue; }
		if (!hierarchy->parent.exists()) { continue; }
		if (!hierarchy->parent.has_component<Transform>()) { continue; }

		Transform & renderer_local = renderer_locals.get(renderer.entity.id);
		Transform const & renderer_parent = renderer_locals.get(hierarchy->parent.id);
		renderer_local = renderer_parent.transform(renderer_local);
	}

	// @Todo: revisit sorting
	qsort(renderers.data, renderers.count, sizeof(*renderers.data), [](void const * va, void const * vb) {
		Renderer_Blob const * a = (Renderer_Blob const *)va;
		Renderer_Blob const * b = (Renderer_Blob const *)vb;
		return (a->camera->layer > b->camera->layer) - (a->camera->layer < b->camera->layer);
	});

	//
	// prepare renderables
	//

	u32 renderable_ids_limit = 0;
	for (u32 i = 0; i < renderables.count; ++i) {
		renderable_ids_limit = max(renderable_ids_limit, renderables[i].entity.id);
	}

	custom::Array<Transform> renderable_locals(renderable_ids_limit + 1);
	for (u32 i = 0; i < renderables.count; ++i) {
		Renderable_Blob & renderable = renderables[i];
		renderable_locals.get(renderable.entity.id) = *renderable.transform;
	}

	// @Todo: revisit nesting
	for (u32 i = 0; i < renderables.count; ++i) {
		Renderable_Blob const & renderable = renderables[i];

		Hierarchy const * hierarchy = renderable.entity.get_component<Hierarchy>().get_safe();
		if (!hierarchy) { continue; }
		if (!hierarchy->parent.exists()) { continue; }
		if (!hierarchy->parent.has_component<Transform>()) { continue; }

		Transform & renderable_local = renderable_locals.get(renderable.entity.id);
		Transform const & renderable_parent = renderable_locals.get(hierarchy->parent.id);
		renderable_local = renderable_parent.transform(renderable_local);
	}

	// @Todo: revisit sorting
	qsort(renderables.data, renderables.count, sizeof(*renderables.data), [](void const * va, void const * vb) {
		Renderable_Blob const * a = (Renderable_Blob const *)va;
		Renderable_Blob const * b = (Renderable_Blob const *)vb;
		return (a->visual->layer > b->visual->layer) - (a->visual->layer < b->visual->layer);
	});

	//
	// render
	//

	u32 renderable_i = 0;
	custom::Array<Renderable_Blob> relevant_renderables(renderables.count);
	for (u32 camera_i = 0; camera_i < renderers.count; ++camera_i) {
		Renderer_Blob const & renderer = renderers[camera_i];

		mat4 const camera_matrix = mat_product(
			renderer.camera->to_matrix(aspect),
			mat_inverse_transform(renderer_locals.get(renderer.entity.id).to_matrix())
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

			mat4 const transform_matrix = renderable_locals.get(renderable.entity.id).to_matrix();

			if (shader_id != renderable.visual->shader.ref.id) {
				shader_id = renderable.visual->shader.ref.id;
				custom::renderer::set_shader(renderable.visual->shader.ref);
				custom::renderer::set_uniform(renderable.visual->shader.ref, u_View_Projection, camera_matrix);
			}

			custom::graphics::unit_id unit = custom::renderer::make_unit(renderable.visual->texture.ref);
			custom::renderer::set_uniform(renderable.visual->shader.ref, u_Texture, unit);
			custom::renderer::set_uniform(renderable.visual->shader.ref, u_Transform, transform_matrix);
			custom::renderer::set_mesh(renderable.visual->mesh.ref);
			custom::renderer::draw();
		}
	}
}
