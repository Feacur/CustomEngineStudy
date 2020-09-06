#pragma once
#include "engine/api/internal/reference.h"

// https://github.com/etodd/lasercrabs/blob/master/src/data/entity.h

//
// pool
//

namespace custom {

template<typename T>
RefT<T> Ref_PoolT<T>::create(void) {
	if (generations.gaps.count == 0) { instances.push(); }
	Ref ref = generations.create();
	return {ref};
}

template<typename T>
void Ref_PoolT<T>::destroy(Ref const & ref) {
	generations.destroy(ref);
	if (ref.id == instances.count - 1) { instances.pop(); }
}

// template<typename T>
// void Ref_PoolT<T>::reset_system(void) {
// 	generations.reset_system();
// 	instances.count = 0;
// }

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

// template<typename T> VOID_VOID_FUNC(ref_pool_reset_system) {
// 	RefT<T>::pool.reset_system();
// }

}
