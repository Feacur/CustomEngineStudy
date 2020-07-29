#include "engine/api/internal/entity_system.h"

struct mat4;
struct Transform;

namespace sandbox {
namespace ecs_renderer {

void update(Transform const & camera_transform, mat4 const & projection);

}}
