#include "engine/api/entity_system.h"

namespace custom {
struct Bytecode;
}

struct mat4;

namespace sandbox {
namespace renderer {

void init(custom::Bytecode * bytecode);
void update(mat4 const & cam);

}}
