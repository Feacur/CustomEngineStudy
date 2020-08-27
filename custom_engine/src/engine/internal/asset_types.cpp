#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/api/internal/asset_types.h"

//
// Prefab_Asset
//

namespace custom {

void Prefab_Asset::promote_to_instance(void) {
	// @Todo: optimize or modify this check?
	for (u32 i = 0; i < instances.count; ++i) {
		if (instances[i] != *this) { continue; }
		CUSTOM_ASSERT(false, "prefab is an instance already");
		return;
	}

	instances.push(*this);
}

}
