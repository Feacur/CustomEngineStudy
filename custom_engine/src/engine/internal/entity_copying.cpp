#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/api/internal/component_types.h"
#include "engine/impl/entity_system.h"

#include <new>

//
// Transform
//

namespace custom {

template<> FROM_TO_FUNC(ref_pool_copy<Transform>) {
	RefT<Transform> const & fromT = (RefT<Transform> const &)from;
	RefT<Transform> & toT = (RefT<Transform> &)to;

	*toT.get_fast() = *fromT.get_fast();
}

}

//
// Camera
//

namespace custom {

template<> FROM_TO_FUNC(ref_pool_copy<Camera>) {
	RefT<Camera> const & fromT = (RefT<Camera> const &)from;
	RefT<Camera> & toT = (RefT<Camera> &)to;

	*toT.get_fast() = *fromT.get_fast();
}

}

//
// Hierarchy
//

namespace custom {

template<> FROM_TO_FUNC(ref_pool_copy<Hierarchy>) {
	RefT<Hierarchy> const & fromT = (RefT<Hierarchy> const &)from;
	RefT<Hierarchy> & toT = (RefT<Hierarchy> &)to;

	Hierarchy const * from_component = fromT.get_fast();
	Hierarchy * to_component = toT.get_fast();

	new (to_component) Hierarchy;

	for (u32 i = 0; i < from_component->children.count; ++i) {
		to_component->children.push(from_component->children[i]);
	}

	for (u32 i = 0; i < to_component->children.count; ++i) {
		to_component->children[i] = to_component->children[i].copy();
	}
}

}
