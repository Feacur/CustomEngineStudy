#include "renderer.h"
#include "../assets/ids.h"
#include "components.h"
#include "engine/api/graphics_params.h"
#include "engine/api/loader.h"
#include "engine/impl/math_linear.h"
#include "engine/impl/bytecode.h"

// @Note: thoughts on factoring code for a renderer
//        - batch together materials/objects with the same shaders
//        - proactively bind textures pending to rendering
//        - batch and rebind if running out of slots/units

namespace sandbox {
namespace renderer {

static custom::Bytecode * bc = NULL;

void init(custom::Bytecode * bytecode) {
	bc = bytecode;
}

void update(mat4 const & cam) {
	// @Todo: prefetch all relevant components into a contiguous array?
	for (u32 i = 0; i < custom::Entity::pool.instances.count; ++i) {
		if (!custom::Entity::pool.check_active(i)) { continue; }
		custom::Entity * e = &custom::Entity::pool.instances[i];

		Visual * visual = e->get_component<Visual>().operator->();
		if (!visual) { continue; }
		if (visual->shader == empty_id) { continue; }

		Transform * transform = e->get_component<Transform>().operator->();
		if (!transform) { continue; }

		bc->write(custom::graphics::Instruction::Use_Shader);
		bc->write(visual->shader);

		if (visual->texture != empty_id) {
			bc->write(custom::graphics::Instruction::Use_Texture);
			bc->write(visual->texture);

			bc->write(custom::graphics::Instruction::Load_Uniform);
			bc->write(visual->shader);
			bc->write((u32)sandbox::Uniform::texture);
			bc->write(custom::graphics::Data_Type::texture_unit);
			bc->write((u32)1); bc->write(visual->texture);
		}

		mat4 mat; // = {};
		quat_get_axes(transform->rotation, mat.x.xyz, mat.y.xyz, mat.z.xyz);
		mat.x.xyz *= transform->scale.x; mat.x.w = 0;
		mat.y.xyz *= transform->scale.y; mat.y.w = 0;
		mat.z.xyz *= transform->scale.z; mat.z.w = 0;
		mat.w = {transform->position, 1};

		bc->write(custom::graphics::Instruction::Load_Uniform);
		bc->write(visual->shader);
		bc->write((u32)sandbox::Uniform::view_proj);
		bc->write(custom::graphics::Data_Type::mat4);
		bc->write((u32)1); bc->write(cam);

		bc->write(custom::graphics::Instruction::Load_Uniform);
		bc->write(visual->shader);
		bc->write((u32)sandbox::Uniform::transform);
		bc->write(custom::graphics::Data_Type::mat4);
		bc->write((u32)1); bc->write(mat);

		if (visual->mesh != empty_id) {
			bc->write(custom::graphics::Instruction::Use_Mesh);
			bc->write(visual->mesh);
		}

		bc->write(custom::graphics::Instruction::Draw);
	}
}

void update2d(mat3 const & cam) {
	// @Todo: prefetch all relevant components into a contiguous array?
	for (u32 i = 0; i < custom::Entity::pool.instances.count; ++i) {
		if (!custom::Entity::pool.check_active(i)) { continue; }
		custom::Entity * e = &custom::Entity::pool.instances[i];

		Visual * visual = e->get_component<Visual>().operator->();
		if (!visual) { continue; }
		if (visual->shader == empty_id) { continue; }

		Transform2d * transform = e->get_component<Transform2d>().operator->();
		if (!transform) { continue; }

		bc->write(custom::graphics::Instruction::Use_Shader);
		bc->write(visual->shader);

		if (visual->texture != empty_id) {
			bc->write(custom::graphics::Instruction::Use_Texture);
			bc->write(visual->texture);

			bc->write(custom::graphics::Instruction::Load_Uniform);
			bc->write(visual->shader);
			bc->write((u32)sandbox::Uniform::texture);
			bc->write(custom::graphics::Data_Type::texture_unit);
			bc->write((u32)1); bc->write(visual->texture);
		}

		mat3 mat; // = {};
		mat.x.xy = transform->rotation;
		mat.y.xy = {-transform->rotation.y, transform->rotation.x};
		mat.x.xy *= transform->scale.x; mat.x.z = 0;
		mat.y.xy *= transform->scale.y; mat.y.z = 0;
		mat.z = {transform->position, 1};

		bc->write(custom::graphics::Instruction::Load_Uniform);
		bc->write(visual->shader);
		bc->write((u32)sandbox::Uniform::view_proj);
		bc->write(custom::graphics::Data_Type::mat3);
		bc->write((u32)1); bc->write(cam);

		bc->write(custom::graphics::Instruction::Load_Uniform);
		bc->write(visual->shader);
		bc->write((u32)sandbox::Uniform::transform);
		bc->write(custom::graphics::Data_Type::mat3);
		bc->write((u32)1); bc->write(mat);

		if (visual->mesh != empty_id) {
			bc->write(custom::graphics::Instruction::Use_Mesh);
			bc->write(visual->mesh);
		}

		bc->write(custom::graphics::Instruction::Draw);
	}
}

}}
