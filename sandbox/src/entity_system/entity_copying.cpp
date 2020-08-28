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

template<> FROM_TO_FUNC(ref_pool_copy<Visual>) {
	RefT<Visual> const & fromT = (RefT<Visual> const &)from;
	RefT<Visual> & toT = (RefT<Visual> &)to;

	*toT.get_fast() = *fromT.get_fast();
}

template<> COMPONENT_LOADING_FUNC(component_pool_clean<Visual>) {
	// RefT<Visual> & refT = (RefT<Visual> &)ref;
}

}

//
// Camera
//

namespace custom {

template<> FROM_TO_FUNC(ref_pool_copy<Lua_Script>) {
	RefT<Lua_Script> const & fromT = (RefT<Lua_Script> const &)from;
	RefT<Lua_Script> & toT = (RefT<Lua_Script> &)to;

	*toT.get_fast() = *fromT.get_fast();
}

template<> COMPONENT_LOADING_FUNC(component_pool_clean<Lua_Script>) {
	// RefT<Lua_Script> & refT = (RefT<Lua_Script> &)ref;
}

}
