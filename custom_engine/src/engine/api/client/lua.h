#pragma once

struct lua_State;

namespace custom {
namespace lua_client {

extern void init_entity(lua_State * L);
extern void init_components(lua_State * L);

}}
