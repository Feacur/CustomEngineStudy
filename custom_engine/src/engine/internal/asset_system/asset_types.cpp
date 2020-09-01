#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/api/internal/asset_types.h"
#include "engine/impl/array.h"

//
// Mesh_Asset
//

namespace custom {

template struct Array<Mesh_Asset::Buffer>;

}

//
// Config_Asset
//

namespace custom {

template struct Array<Config_Asset::Entry>;
Strings_Storage Config_Asset::strings;

}
