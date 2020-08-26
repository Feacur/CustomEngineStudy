#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/api/internal/parsing.h"
#include "engine/api/internal/component_types.h"
#include "engine/impl/entity_system.h"
#include "engine/impl/math_linear.h"

//
// Transform
//

namespace custom {
namespace serialization {

template<> SERIALIZATION_READ_FUNC(component_pool_serialization_read<Transform>) {
	RefT<Transform> & refT = (RefT<Transform> &)ref;

	Transform * component = refT.get_fast();

	bool done = false;
	while (!done && **source) {
		skip_to_eol(source); parse_eol(source);
		parse_void(source);
		switch (**source) {
			case 'p': ++(*source); component->position = (parse_void(source), parse_vec3(source)); break;

			case 'r': ++(*source); switch (**source) {
				case ' ': component->rotation = (parse_void(source), parse_vec4(source)); break;
				case 'r': component->rotation = quat_from_radians((parse_void(source), parse_vec3(source))); break;
				case 'd': component->rotation = quat_from_radians((parse_void(source), parse_vec3(source)) * deg_to_rad); break;
			} break;

			case 's': ++(*source); component->scale    = (parse_void(source), parse_vec3(source)); break;
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

	bool done = false;
	while (!done && **source) {
		skip_to_eol(source); parse_eol(source);
		parse_void(source);
		switch (**source) {
			case 'n': ++(*source); component->near  = 0.1f; break;
			case 'f': ++(*source); component->far   = 100.0f; break;
			case 's': ++(*source); component->scale = 1.0f; break;
			case 'o': ++(*source); component->ortho = 0.0f; break;
			case 'c': ++(*source); component->clear = graphics::Clear_Flag::Depth; break;
			case 'l': ++(*source); component->layer = 0; break;
			default: done = true; break;
		}
	}
}

}}