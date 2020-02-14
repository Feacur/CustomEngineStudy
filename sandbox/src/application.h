#include "engine/core/math_types.h"
#include "engine/core/key_codes.h"

namespace sandbox {
namespace application {

void run(void);

// input
custom::Key_State get_key(custom::Key_Code key);
custom::Key_State get_mouse_key(custom::Mouse_Code key);
ivec2 const & get_mouse_pos();
ivec2 const & get_mouse_delta();
vec2 const & get_mouse_wheel();

// callbacks
typedef void init_func();
typedef void viewport_func(ivec2);
typedef void update_func(r32);

void set_init_callback(init_func * callback);
void set_viewport_callback(viewport_func * callback);
void set_update_callback(update_func * callback);

}}
