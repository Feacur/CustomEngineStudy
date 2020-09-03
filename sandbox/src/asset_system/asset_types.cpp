#include "asset_types.h"

//
// Shader_Asset
//

void Lua_Asset::update(custom::Array<u8> & file) {
	// file.push('\0'); --file.count;
	source.data     = file.data;     file.data     = NULL;
	source.capacity = file.capacity; file.capacity = 0;
	source.count    = file.count;    file.count    = 0;
}
