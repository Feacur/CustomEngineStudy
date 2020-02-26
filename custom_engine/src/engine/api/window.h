#pragma once
#include "engine/core/math_types.h"
#include "engine/core/key_codes.h"

namespace custom {
namespace window {

typedef struct Internal_Data Data; // @Note: an opaque pointer

Data * create(void);
void destroy(Data * data);

void init_context(Data * data);

void update(Data * data);

void set_vsync(Data * data, s32 value);
bool check_vsync(Data * data);

void set_header(Data * data, cstring value);
ivec2 const & get_size(Data * data);
bool get_should_close(Data * data);

// input
bool get_key(Internal_Data * data, Key_Code key);
bool get_mouse_key(Internal_Data * data, Mouse_Code key);
bool get_key_transition(Internal_Data * data, Key_Code key, bool to_state);
bool get_mouse_key_transition(Internal_Data * data, Mouse_Code key, bool to_state);
ivec2 const & get_mouse_pos(Internal_Data * data);
ivec2 const & get_mouse_delta(Internal_Data * data);
vec2 const & get_mouse_wheel(Internal_Data * data);

// callbacks
typedef void viewport_func(Data * data, ivec2 size);

void set_viewport_callback(Data * data, viewport_func * callback);

}}
