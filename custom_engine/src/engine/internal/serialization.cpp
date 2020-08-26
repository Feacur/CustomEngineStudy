#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/api/internal/component_types.h"
#include "engine/impl/entity_system.h"

//
// Transform
//

namespace custom {
namespace serialization {

template<> SERIALIZATION_READ_FUNC(component_pool_serialization_read<Transform>) {
	RefT<Transform> & refT = (RefT<Transform> &)ref;

	Transform * component = refT.get_fast();

	// @Todo
	(*component) = {
		{0,1,0}, {0,0,0,1}, {1,1,1}
	};
}

}}

//
// Camera
//

namespace custom {
namespace serialization {

template<> SERIALIZATION_READ_FUNC(component_pool_serialization_read<Camera>) {
	RefT<Camera> & refT = (RefT<Camera> &)ref;

	Camera * component = refT.get_fast();

	// (*component) = {
	// 	0.1f, 100.0f, 1.0f, 0.0f,
	// 	graphics::Clear_Flag::Depth,
	// 	2,
	// };
}

}}
