#include "engine/api/internal/names_lookup.h"
#include "engine/api/internal/bytecode.h"
#include "engine/api/internal/renderer.h"
#include "engine/api/internal/component_types.h"
#include "engine/api/internal/entity_system.h"
#include "engine/api/internal/application.h"
#include "engine/impl/array.h"
#include "engine/impl/math_linear.h"

#include "../entity_system/component_types.h"

#include <stdlib.h>

// @Note: thoughts on factoring code for a renderer
//        - batch together materials/objects with the same shaders
//        - proactively bind textures pending to rendering
//        - batch and rebind if running out of slots/units

namespace {

struct Transforms_Blob {
	custom::Entity    entity;
	Transform const * transform;
};

struct Renderer_Blob {
	u32            id;
	Camera const * camera;
};

struct Renderable_Blob {
	u32            id;
	Visual const * visual;
};

}

static void build_transforms_map(custom::Array<Transform> & id_to_transform);
static void ecs_update_renderer_internal(custom::Array<Transform> const & id_to_transform, custom::Array<Renderer_Blob> const & renderers, custom::Array<Renderable_Blob> const & renderables);

//
//
//

namespace sandbox {

void ecs_update_renderer(void) {
	custom::Array<Transform> id_to_transform;
	build_transforms_map(id_to_transform);

	custom::Array<Renderer_Blob> renderers(8);
	for (u32 i = 0; i < custom::Entity::state.instances.count; ++i) {
		custom::Entity const entity = custom::Entity::state.instances[i];
		if (!entity.exists()) { continue; }
		if (!entity.has_component<Transform>()) { continue; }

		Camera const * camera = entity.get_component<Camera>().get_safe();
		if (!camera) { continue; }

		renderers.push({entity.id, camera});
	}

	custom::Array<Renderable_Blob> renderables(custom::Entity::state.instances.count);
	for (u32 i = 0; i < custom::Entity::state.instances.count; ++i) {
		custom::Entity const entity = custom::Entity::state.instances[i];
		if (!entity.exists()) { continue; }
		if (!entity.has_component<Transform>()) { continue; }

		Visual const * visual = entity.get_component<Visual>().get_safe();
		if (!visual) { continue; }

		renderables.push({entity.id, visual});
	}

	ecs_update_renderer_internal(id_to_transform, renderers, renderables);
}

}

//
//
//

static void build_transforms_map(custom::Array<Transform> & id_to_transform) {
	custom::Array<Transforms_Blob> transforms(custom::Entity::state.instances.count);
	for (u32 i = 0; i < custom::Entity::state.instances.count; ++i) {
		custom::Entity const entity = custom::Entity::state.instances[i];
		if (!entity.exists()) { continue; }

		Transform const * transform = entity.get_component<Transform>().get_safe();
		if (!transform) { continue; }

		transforms.push({entity, transform});
	}
	// transforms.set_capacity(transforms.count);

	// @Todo: use actual map structure?
	//        be more optimal fetching parents?
	u32 transform_ids_limit = 0;
	for (u32 i = 0; i < transforms.count; ++i) {
		transform_ids_limit = max(transform_ids_limit, transforms[i].entity.id);
	}

	id_to_transform.set_capacity(transform_ids_limit + 1);
	for (u32 i = 0; i < transforms.count; ++i) {
		Transforms_Blob const & transform = transforms[i];
		id_to_transform.get(transform.entity.id) = *transform.transform;
	}

	for (u32 i = 0; i < transforms.count; ++i) {
		Transforms_Blob const & transform = transforms[i];

		Hierarchy const * hierarchy = transform.entity.get_component<Hierarchy>().get_safe();
		if (!hierarchy) { continue; }
		if (!hierarchy->parent.exists()) { continue; }
		if (!hierarchy->parent.has_component<Transform>()) { continue; }

		Transform       & child  = id_to_transform.get(transform.entity.id);
		Transform const & parent = id_to_transform.get(hierarchy->parent.id);
		child = parent.transform(child);
	}
}

static void ecs_update_renderer_internal(custom::Array<Transform> const & id_to_transform, custom::Array<Renderer_Blob> const & renderers, custom::Array<Renderable_Blob> const & renderables) {
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

	qsort(renderers.data, renderers.count, sizeof(*renderers.data), [](void const * va, void const * vb) {
		Renderer_Blob const * a = (Renderer_Blob const *)va;
		Renderer_Blob const * b = (Renderer_Blob const *)vb;
		return (a->camera->layer > b->camera->layer) - (a->camera->layer < b->camera->layer);
	});

	qsort(renderables.data, renderables.count, sizeof(*renderables.data), [](void const * va, void const * vb) {
		Renderable_Blob const * a = (Renderable_Blob const *)va;
		Renderable_Blob const * b = (Renderable_Blob const *)vb;
		return (a->visual->layer > b->visual->layer) - (a->visual->layer < b->visual->layer);
	});

	u32 renderable_i = 0;
	custom::Array<Renderable_Blob> relevant_renderables(renderables.count);
	for (u32 camera_i = 0; camera_i < renderers.count; ++camera_i) {
		Renderer_Blob const & renderer = renderers[camera_i];

		mat4 const camera_matrix = mat_product(
			renderer.camera->to_matrix(aspect),
			mat_inverse_transform(id_to_transform.get(renderer.id).to_matrix())
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

			mat4 const transform_matrix = id_to_transform.get(renderable.id).to_matrix();

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
