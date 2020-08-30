#pragma once

struct lua_State;

namespace sandbox {

void lua_function(lua_State * L, cstring name);
void ecs_update_lua(lua_State * L, r32 dt);

}
