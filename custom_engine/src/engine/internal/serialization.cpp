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

template<> VOID_DREF_CSTR_FUNC(component_pool_serialization_read<Transform>) {
}

}}

//
// Camera
//

namespace custom {
namespace serialization {

template<> VOID_DREF_CSTR_FUNC(component_pool_serialization_read<Camera>) {
}

}}
