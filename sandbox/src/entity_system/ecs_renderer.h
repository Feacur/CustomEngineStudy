#include "engine/api/internal/entity_system.h"

struct mat4;
struct Transform;

namespace sandbox {
namespace ecs_renderer {

void process(Transform const & camera, mat4 const & projection);

}}
