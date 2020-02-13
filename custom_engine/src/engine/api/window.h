#pragma once
#include "engine/core/math_types.h"


namespace custom {
namespace window {

typedef struct Internal_Data Data; // @Note: an opaque pointer

typedef void viewport_func(Data * data, ivec2 size);

Data * create(void);
void destroy(Data * data);

void init_context(Data * data);

void update(Data * data);

void set_vsync(Data * data, s32 value);
bool check_vsync(Data * data);

void set_header(Data * data, cstring value);
ivec2 get_size(Data * data);
bool get_should_close(Data * data);

void set_viewport_callback(Data * data, viewport_func * callback);

}}
