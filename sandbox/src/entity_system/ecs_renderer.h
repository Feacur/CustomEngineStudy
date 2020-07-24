#include "engine/api/internal/entity_system.h"

namespace custom {
struct Bytecode;
}

struct mat4;

struct Transform;

namespace sandbox {
namespace ecs_renderer {

void init(custom::Bytecode * bytecode);
void process(Transform const & camera, mat4 const & projection);

}}
