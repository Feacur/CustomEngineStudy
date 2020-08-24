#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/impl/entity_system.h"

#include "component_types.h"

//
// Visual
//

namespace custom {
namespace serialization {

template<> VOID_DREF_CSTR_FUNC(component_pool_serialization_read<Visual>) {
}

}}

//
// Lua_Script
//

namespace custom {
namespace serialization {

template<> VOID_DREF_CSTR_FUNC(component_pool_serialization_read<Lua_Script>) {
}

}}
