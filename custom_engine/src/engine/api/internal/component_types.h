#pragma once
#include "engine/core/math_types.h"
#include "engine/api/graphics_params.h"
#include "engine/api/internal/entity_system.h"

namespace custom {

}

struct Transform
{
	vec3 position = {0, 0, 0};
	vec3 scale    = {1, 1, 1};
	quat rotation = {0, 0, 0, 1};
};

struct Camera
{
	typedef custom::graphics::Clear_Flag Clear_Flag;
	r32 ncp   = 0.1f;
	r32 fcp   = 100;
	r32 scale = 1;
	r32 ortho = 0;
	Clear_Flag clear = Clear_Flag::Color | Clear_Flag::Depth;
	u8 layer = 0;
};

struct Hierarchy {
	custom::Entity parent = {custom::empty_ref};

	struct Link { u32 id; custom::Entity entity; };
	static custom::Array<Link> links;

	static void fetch_children(custom::Entity const & entity, custom::Array<Hierarchy::Link> & buffer);
	static void set_parent(custom::Entity & child, custom::Entity const & entity);
	static void rem_parent(custom::Entity & child, custom::Entity const & entity);
	static void remove_at(u32 id);
};

// struct Transform2d
// {
// 	vec2    position = {0, 0};
// 	vec2    scale    = {1, 1};
// 	complex rotation = {1, 0};
// };
