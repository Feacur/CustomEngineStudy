#pragma once
#include "engine/api/internal/reference.h"

// https://github.com/etodd/lasercrabs/blob/master/src/data/entity.h

//
// pool
//

namespace custom {

template<typename T>
RefT<T> Ref_Pool<T>::create(void) {
	CUSTOM_ASSERT(generations.gens.capacity < 100, "");
	CUSTOM_ASSERT(generations.gaps.capacity < 100, "");
	CUSTOM_ASSERT(instances.capacity < 100, "");
	if (generations.gaps.count == 0) { instances.push(); }
	Ref ref = generations.create();
	return {ref};
}

template<typename T>
void Ref_Pool<T>::destroy(Ref const & ref) {
	generations.destroy(ref);
	if (ref.id == instances.count - 1) { instances.pop(); }
}

}

//
// universal access
//

namespace custom {

template<typename T> REF_VOID_FUNC(ref_pool_create) {
	return RefT<T>::pool.create();
}

template<typename T> VOID_REF_FUNC(ref_pool_destroy) {
	RefT<T>::pool.destroy(ref);
}

template<typename T> BOOL_REF_FUNC(ref_pool_contains) {
	return RefT<T>::pool.contains(ref);
}

}
