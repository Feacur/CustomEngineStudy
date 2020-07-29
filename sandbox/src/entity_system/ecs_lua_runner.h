#include "engine/api/internal/entity_system.h"

struct lua_State;

namespace sandbox {
namespace ecs_lua_runner {

void lua_function(lua_State * L, cstring name);
void update(lua_State * L);

}}
