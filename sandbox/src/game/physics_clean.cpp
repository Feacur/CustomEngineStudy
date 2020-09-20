#include "engine/api/internal/component_types.h"
#include "engine/api/internal/asset_system.h"
#include "engine/api/internal/entity_system.h"
#include "engine/api/internal/asset_types.h"
#include "engine/impl/array.h"
#include "engine/impl/reference.h"
#include "engine/impl/math_linear.h"

#include "../entity_system/component_types.h"

namespace {

struct Physics_Settings {
	u32  frequency = 50;
	vec2 gravity   = {0, -9.81f};
};

}

static Physics_Settings settings;

static void ecs_update_physics_iteration(r32 dt);

//
//
//

namespace sandbox {

static custom::Asset_RefT<custom::Config_Asset> config_ref = {custom::empty_ref, custom::empty_index};
void ecs_init_physics_clean(void) {
	u32 config_id = custom::Asset::get_resource("assets/configs/client.cfg", custom::empty_index);
	config_ref = custom::Asset::add<custom::Config_Asset>(config_id);
}

static void consume_config(void) {
	u32 version = custom::empty_index;

	custom::Config_Asset const * config = config_ref.ref.get_safe();
	CUSTOM_ASSERT(config, "no config");

	if (version == config->version) { return; }
	version = config->version;

	settings.frequency = config->get_value<u32>("physics_frequency", 50);
	settings.gravity   = {
		config->get_value<r32>("physics_gravity_x", 0),
		config->get_value<r32>("physics_gravity_y", -9.81f),
	};
}

void ecs_update_physics_clean(r32 dt) {
	consume_config();

	CUSTOM_ASSERT(settings.frequency, "zero frequency");
	r32 period = 1.0f / settings.frequency;

	//
	static r32 elapsed = 0; elapsed += dt;
	while (elapsed >= period) {
		elapsed -= period;
		ecs_update_physics_iteration(period);
	}
}

}

//
//
//

static void ecs_update_physics_iteration(r32 dt) {
	
}
