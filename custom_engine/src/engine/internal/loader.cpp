#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/core/collection_types.h"
#include "engine/core/math_types.h"
#include "engine/debug/log.h"
#include "engine/api/platform/file.h"
#include "engine/api/platform/graphics_resource.h"
#include "engine/api/client/asset_lookup.h"
#include "engine/api/graphics_params.h"
#include "engine/impl/array_fixed.h"
#include "engine/impl/bytecode.h"

namespace custom {
namespace loader {

static Bytecode * bc = NULL;

void init(Bytecode * bytecode) {
	bc = bytecode;
}

}}

#include "loader_lua.h"
#include "loader_shader.h"
#include "loader_image.h"
#include "loader_obj.h"
