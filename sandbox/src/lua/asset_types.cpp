#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/api/lua.h"
#include "engine/impl/asset_system.h"

#include "../asset_system/asset_types.h"

#include <lua.hpp>
// #include <lstate.h>

//
// None
//

namespace custom {
namespace lua {

void init_client_asset_types(lua_State * L) {
	#define COMPONENT_IMPL(T) LUA_META_IMPL(T)
	#include "../asset_system/assets_registry_impl.h"
}

}}
