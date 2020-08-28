#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/debug/log.h"

#include "engine/impl/array.h"
#include "engine/impl/reference.h"

//
// pool
//

namespace custom {

Ref Gen_Pool::create(void) {
	u32 id;
	if (gaps.count > 0) {
		id = gaps[gaps.count - 1];
		gaps.pop();
	}
	else {
		id = gens.count;
		gens.push(0);
	}
	return { id, gens.get(id) };
}

void Gen_Pool::destroy(Ref const & ref) {
	// @Change: ignore, but warn, if data doesn't exist?
	if (ref.gen != gens.get(ref.id)) {
		CUSTOM_ASSERT(false, "destroying null data");
		return;
	}
	++gens[ref.id];
	if (ref.id == gens.count - 1) {
		gens.pop();
	}
	else {
		gaps.push(ref.id);
	}
}

}
