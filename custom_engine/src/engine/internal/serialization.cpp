#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/api/internal/parsing.h"
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

	bool done = false;
	while (!done && *source < end) {
		parse_eol(source, end); parse_void(source);
		switch (**source) {
			case 'p': component->position = {0,1,0};   break;
			case 'r': component->rotation = {0,0,0,1}; break;
			case 's': component->scale    = {1,1,1};   break;
			default: done = true; break;
		}
	}
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

	// @Todo
	CUSTOM_ASSERT(false, "todo");
	// (*component) = {
	// 	0.1f, 100.0f, 1.0f, 0.0f,
	// 	graphics::Clear_Flag::Depth,
	// 	2,
	// };
}

}}
