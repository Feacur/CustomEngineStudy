#pragma once
#include "engine/core/math_types.h"
#include "engine/core/key_codes.h"

namespace custom {
namespace window {

struct Internal_Data; // @Note: an opaque pointer

Internal_Data * create(void);
void destroy(Internal_Data * data);

void init_context(Internal_Data * data);

void update(Internal_Data * data);

void set_vsync(Internal_Data * data, u8 value);
bool check_vsync(Internal_Data * data);
u16 get_refresh_rate(Internal_Data * data, u16 default_value);
void toggle_borderless_fullscreen(Internal_Data * data);

void set_header(Internal_Data * data, cstring value);
ivec2 const & get_size(Internal_Data * data);
bool get_is_active(Internal_Data * data);

// input
bool get_key(Internal_Data * data, Key_Code key);
bool get_mouse_key(Internal_Data * data, Mouse_Code key);
bool get_key_transition(Internal_Data * data, Key_Code key, bool to_state);
bool get_mouse_key_transition(Internal_Data * data, Mouse_Code key, bool to_state);
ivec2 const & get_mouse_pos(Internal_Data * data);
ivec2 const & get_mouse_delta(Internal_Data * data);
vec2 const & get_mouse_wheel(Internal_Data * data);

// callbacks
typedef void viewport_func(Internal_Data * data, ivec2 size);
typedef void close_func(Internal_Data * data);

void set_viewport_callback(Internal_Data * data, viewport_func * callback);
void set_close_callback(Internal_Data * data, close_func * callback);

}}
