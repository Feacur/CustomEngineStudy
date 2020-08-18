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

void update() {
	
	custom::Entity camera_entity = {UINT32_MAX, 0};
	for (u32 i = 0; i < custom::Entity::instances.count; ++i) {
		custom::Entity entity = custom::Entity::instances[i];
		if (!entity.exists()) { continue; }
		if (!entity.has_component<Transform>()) { continue; }
		if (!entity.has_component<Camera>()) { continue; }
		camera_entity = entity;
		break;
	}

	if (!camera_entity.exists()) { return; }

	Transform * camera_transform = camera_entity.get_component<Transform>().get_fast();
	Camera * camera = camera_entity.get_component<Camera>().get_fast();

	ivec2 viewport_size = custom::application::get_viewport_size();
	r32 const aspect = (r32)viewport_size.x / (r32)viewport_size.y;

	mat4 camera_matrix = to_matrix(camera_transform->position, camera_transform->rotation, camera_transform->scale);
	camera_matrix = mat_product(
		mat_inverse_transform(camera_matrix),
		interpolate(
			mat_persp({camera->scale, camera->scale * aspect}, camera->near, camera->far),
			mat_ortho({camera->scale, camera->scale * aspect}, camera->near, camera->far),
			camera->ortho
		)
	);

	// @Todo: prefetch all relevant components into a contiguous array?
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

}}
