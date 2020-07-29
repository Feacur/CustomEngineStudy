#include "ecs_renderer.h"

#include "engine/api/internal/bytecode.h"
#include "engine/api/internal/loader.h"
#include "engine/api/internal/renderer.h"
#include "engine/api/graphics_params.h"
#include "engine/impl/array.h"
#include "engine/impl/math_linear.h"

#include "../assets/ids.h"
#include "components.h"

// @Note: thoughts on factoring code for a renderer
//        - batch together materials/objects with the same shaders
//        - proactively bind textures pending to rendering
//        - batch and rebind if running out of slots/units

namespace sandbox {
namespace ecs_renderer {

void update(Transform const & camera_transform, mat4 const & projection) {
	mat4 camera_matrix = to_matrix(camera_transform.position, camera_transform.rotation, camera_transform.scale);
	camera_matrix = mat_product(mat_inverse_transform(camera_matrix), projection);

	custom::loader::uniforms();

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
		custom::loader::shader(visual->shader);
		custom::loader::image(visual->texture);
		custom::loader::mesh(visual->mesh);

		//
		custom::renderer::set_shader(visual->shader);
		custom::renderer::set_texture(visual->shader, (u32)sandbox::Uniform::texture, visual->texture);
		custom::renderer::set_matrix(visual->shader, (u32)sandbox::Uniform::view_proj, camera_matrix);
		custom::renderer::set_matrix(visual->shader, (u32)sandbox::Uniform::transform, transform_matrix);
		custom::renderer::set_mesh(visual->mesh);
		custom::renderer::draw();
	}
}

}}
