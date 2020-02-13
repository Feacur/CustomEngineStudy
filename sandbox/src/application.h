#include "engine/core/math_types.h"

namespace sandbox {
namespace application {

void run(void);

// callbacks
typedef void init_func();
typedef void viewport_func(ivec2);
typedef void update_func(r32);

void set_init_callback(init_func * callback);
void set_viewport_callback(viewport_func * callback);
void set_update_callback(update_func * callback);

}}
