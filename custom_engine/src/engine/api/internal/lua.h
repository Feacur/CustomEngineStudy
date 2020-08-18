#pragma once
struct lua_State;

namespace custom {
namespace lua {

void init_input(lua_State * L);
void init_math_linear(lua_State * L);
void init_asset_system(lua_State * L);
void init_entity_system(lua_State * L);

}}
