#include "engine/api/entity_system.h"

namespace custom {
struct Bytecode;
}

struct mat3;
struct mat4;

struct Transform;
struct Transform2d;

namespace sandbox {
namespace renderer {

void init(custom::Bytecode * bytecode);
void update(Transform const & camera, mat4 const & projection);
void update2d(Transform2d const & camera, mat3 const & projection);

}}
