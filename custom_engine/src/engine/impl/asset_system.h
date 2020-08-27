#pragma once
#include "engine/api/internal/asset_system.h"

//
// universal access
//

namespace custom {
namespace loading {

template<typename T> LOADING_FUNC(asset_pool_load);
template<typename T> LOADING_FUNC(asset_pool_unload);

}}

//
// asset
//

namespace custom {

template<typename T>
RefT<T> Asset::add(u32 id) {
	return {add(Asset_Registry<T>::type, id)};
}

template<typename T>
void Asset::rem(u32 id) {
	rem(Asset_Registry<T>::type, id);
}

template<typename T>
RefT<T> Asset::get(u32 id) {
	return {get(Asset_Registry<T>::type, id)};
}

template<typename T>
bool Asset::has(u32 id) {
	return has(Asset_Registry<T>::type, id);
}

template<typename T>
cstring Asset::get_path(RefT<T> const & ref) {
	return get_path(Asset_Registry<T>::type, (Asset const &)ref);
}

}
