#include "custom_pch.h"

#include "engine/core/code.h"
#include "engine/debug/log.h"
#include "engine/api/internal/reference.h"
#include "engine/impl/array.h"

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
		u32 capacity_before = gens.capacity;
		gens.push();
		for (u32 i = capacity_before; i < gens.capacity; ++i) { gens.data[i] = 0; }
	}
	return { id, gens[id] };
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

//
// pool
//

namespace custom {

Ref_Pool::Ref_Pool(u32 size) : size(size) {}

Ref Ref_Pool::create_void(void) {
	if (generations.gaps.count == 0) { buffer.push_range(size); }
	Ref ref = generations.create();
	return {ref};
}

void Ref_Pool::destroy(Ref const & ref) {
	generations.destroy(ref);
	if ((ref.id + 1) * size == buffer.count) { buffer.pop_range(size); }
}

}