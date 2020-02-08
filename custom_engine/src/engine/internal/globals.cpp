#include "custom_pch.h"

#include "engine/core/types.h"
#include "engine/api/system.h"
#include "engine/api/timer.h"
#include "engine/api/window.h"
#include "engine/api/entity_system.h"
#include "engine/api/rendering_settings.h"
#include "engine/impl/array.h"

namespace custom {

System system = {};
Timer timer = {};
bool Window::should_close = false;

RefPool<Entity> Entity::pool;
u32 Entity::component_types_count;
Array<Ref_Raw> Entity::components;

Array<RefPoolBase *> World::component_pools;

Context_Settings context_settings;
Pixel_Format pixel_format_hint;

Ref<Entity> World::create() {
	return Entity::pool.create();
}

void World::destroy(Ref<Entity> ref) {
	u32 entity_offset = ref.id * Entity::component_types_count;
	for (u32 i = 0; i < component_pools.count; ++i) {
		Ref_Raw & ref_raw = Entity::components[entity_offset + i];
		if (ref_raw.id != empty_id) {
			component_pools[i]->destroy(ref_raw.id, ref_raw.gen);
		}
	}
	Entity::pool.destroy(ref);
}

void globals_init(void) {
	// context_settings
	context_settings = {};
	context_settings.major_version = 4;
	context_settings.minor_version = 6;

	// pixel_format_hint
	pixel_format_hint = {};
	pixel_format_hint.red_bits     = 8;
	pixel_format_hint.green_bits   = 8;
	pixel_format_hint.blue_bits    = 8;
	pixel_format_hint.alpha_bits   = 8;
	pixel_format_hint.depth_bits   = 24;
	pixel_format_hint.stencil_bits = 8;
	pixel_format_hint.doublebuffer = true;
}

}
