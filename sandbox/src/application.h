#include "engine/core/math_types.h"

typedef void init_func();
typedef void viewport_func(ivec2);
typedef void update_func(r32);

namespace sandbox {
namespace application {

void set_init_callback(init_func * callback);
void set_viewport_callback(viewport_func * callback);
void set_update_callback(update_func * callback);
void run(void);

}}
