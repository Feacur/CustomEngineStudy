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
// asset ref
//

namespace custom {

template<typename T>
cstring Asset_RefT<T>::get_path(void) const {
	return Asset{ref, resource, Asset_Registry<T>::type}.get_path();
}

template<typename T>
bool Asset_RefT<T>::exists(void) const {
	return Asset{ref, resource, Asset_Registry<T>::type}.exists();
}

template<typename T>
void Asset_RefT<T>::destroy(void) {
	return Asset{ref, resource, Asset_Registry<T>::type}.destroy();
}

}

//
// asset
//

namespace custom {

template<typename T>
Asset_RefT<T> Asset::add(u32 resource) {
	return {add(Asset_Registry<T>::type, resource).ref, resource};
}

template<typename T>
void Asset::rem(u32 resource) {
	rem(Asset_Registry<T>::type, resource);
}

template<typename T>
Asset_RefT<T> Asset::get(u32 resource) {
	return {get(Asset_Registry<T>::type, resource).ref, resource};
}

template<typename T>
bool Asset::has(u32 resource) {
	return has(Asset_Registry<T>::type, resource);
}

}
