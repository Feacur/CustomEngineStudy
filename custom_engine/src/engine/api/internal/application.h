#pragma once
#include "engine/core/math_types.h"
#include "engine/core/key_codes.h"

namespace custom {
	// @Forward
	struct Bytecode;
}

namespace custom {
namespace application {

void init(void);
void run(void);
void set_refresh_rate(u32 target, u32 debug, u32 failsafe, u32 vsync, bln as_display);

// data
ivec2 const & get_viewport_size(void);

// input
bool get_key(Key_Code key);
bool get_mouse_key(Mouse_Code key);
bool get_key_transition(Key_Code key, bool to_state);
bool get_mouse_key_transition(Mouse_Code key, bool to_state);
ivec2 const & get_mouse_pos(void);
ivec2 const & get_mouse_delta(void);
vec2 const & get_mouse_wheel(void);

// callbacks
typedef void init_func();
typedef void viewport_func(ivec2);
typedef void update_func(r32);

void set_init_callback(init_func * callback);
void set_viewport_callback(viewport_func * callback);
void set_update_callback(update_func * callback);

}}
