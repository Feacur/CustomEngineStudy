#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/impl/entity_system.h"

#include "component_types.h"

// #include <new>

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
	// RefT<Visual> & refT = (RefT<Visual> &)ref;
}

template<> ENTITY_LOADING_FUNC(component_pool_unload<Visual>) {
	RefT<Visual> & refT = (RefT<Visual> &)ref;
	Visual * visual = refT.get_fast();
	visual->shader  = {custom::empty_ref, custom::empty_index};
	visual->texture = {custom::empty_ref, custom::empty_index};
	visual->mesh    = {custom::empty_ref, custom::empty_index};
}

}

//
// Camera
//

namespace custom {

template<> ENTITY_FROM_TO_FUNC(component_pool_copy<Lua_Script>) {
	RefT<Lua_Script> const & fromT = (RefT<Lua_Script> const &)from;
	RefT<Lua_Script> & toT = (RefT<Lua_Script> &)to;

	*toT.get_fast() = *fromT.get_fast();
}

template<> ENTITY_LOADING_FUNC(component_pool_load<Lua_Script>) {
	// RefT<Lua_Script> & refT = (RefT<Lua_Script> &)ref;
}

template<> ENTITY_LOADING_FUNC(component_pool_unload<Lua_Script>) {
	// RefT<Lua_Script> & refT = (RefT<Lua_Script> &)ref;
}

}
