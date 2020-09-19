#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/impl/entity_system.h"

#include "component_types.h"

//
// Visual
//

namespace custom {

template<> ENTITY_FROM_TO_FUNC(component_pool_copy<Visual>) {
	RefT<Visual> const & fromT = (RefT<Visual> const &)from;
	RefT<Visual> & toT = (RefT<Visual> &)to;

	*toT.get_fast() = *fromT.get_fast();
}

template<> ENTITY_LOADING_FUNC(component_pool_load<Visual>) {
	RefT<Visual> & refT = (RefT<Visual> &)ref;
	Visual * component = refT.get_fast();

	component->shader  = {custom::empty_ref, custom::empty_index};
	component->texture = {custom::empty_ref, custom::empty_index};
	component->mesh    = {custom::empty_ref, custom::empty_index};
}

template<> ENTITY_LOADING_FUNC(component_pool_unload<Visual>) {
	// RefT<Visual> & refT = (RefT<Visual> &)ref;
	// Visual * component = refT.get_fast();
}

}

//
// Lua_Script
//

namespace custom {

template<> ENTITY_FROM_TO_FUNC(component_pool_copy<Lua_Script>) {
	RefT<Lua_Script> const & fromT = (RefT<Lua_Script> const &)from;
	RefT<Lua_Script> & toT = (RefT<Lua_Script> &)to;

	*toT.get_fast() = *fromT.get_fast();
}

template<> ENTITY_LOADING_FUNC(component_pool_load<Lua_Script>) {
	RefT<Lua_Script> & refT = (RefT<Lua_Script> &)ref;
	Lua_Script * component = refT.get_fast();

	component->update_string_id = custom::empty_index;
}

template<> ENTITY_LOADING_FUNC(component_pool_unload<Lua_Script>) {
	// RefT<Lua_Script> & refT = (RefT<Lua_Script> &)ref;
	// Lua_Script * component = refT.get_fast();
}

}

//
// Physical
//

namespace custom {

template<> ENTITY_FROM_TO_FUNC(component_pool_copy<Physical>) {
	RefT<Physical> const & fromT = (RefT<Physical> const &)from;
	RefT<Physical> & toT = (RefT<Physical> &)to;

	*toT.get_fast() = *fromT.get_fast();
}

template<> ENTITY_LOADING_FUNC(component_pool_load<Physical>) {
	// RefT<Physical> & refT = (RefT<Physical> &)ref;
}

template<> ENTITY_LOADING_FUNC(component_pool_unload<Physical>) {
	// RefT<Physical> & refT = (RefT<Physical> &)ref;
}

}

//
// Phys2d
//

namespace custom {

template<> ENTITY_FROM_TO_FUNC(component_pool_copy<Phys2d>) {
	RefT<Phys2d> const & fromT = (RefT<Phys2d> const &)from;
	RefT<Phys2d> & toT = (RefT<Phys2d> &)to;

	*toT.get_fast() = *fromT.get_fast();
}

template<> ENTITY_LOADING_FUNC(component_pool_load<Phys2d>) {
	RefT<Phys2d> & refT = (RefT<Phys2d> &)ref;
	Phys2d * component = refT.get_fast();

	// meta
	component->movable   = 1;
	component->rotatable = 1;
	// material
	component->mass       = 1;
	component->elasticity = 0.5;
	component->roughness  = 0.3;
	component->stickiness = 0.6;
	component->stillness  = 0.8;
	// shape
	component->shape = 1;
	component->mesh = {custom::empty_ref, custom::empty_index};
	// state
	component->velocity             = {0, 0};
	component->acceleration         = {0, 0};
	component->angular_velocity     = 0;
	component->angular_acceleration = 0;
}

template<> ENTITY_LOADING_FUNC(component_pool_unload<Phys2d>) {
	// RefT<Phys2d> & refT = (RefT<Phys2d> &)ref;
	// Phys2d * component = refT.get_fast();
}

}
