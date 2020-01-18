//
// game
//

namespace entities {	
	template<typename T> constexpr inline Component_Type get_type() { return Component_Type::None; }
	
	#define REGISTER_COMPONENT(TYPE, FIELD)\
	template<> constexpr inline Component_Type get_type<TYPE##_Component>() { return Component_Type::TYPE; }\
	void component_set(Game_Data * game_data, Entity entity, TYPE##_Component const & component) {\
		Array_Dynamic<Component_Type>   & flags      = game_data->entities.flags;\
		Array_Dynamic<TYPE##_Component> & components = game_data->entities.FIELD;\
		components[entity] = component;\
		flags[entity]      = flags[entity] | Component_Type::TYPE;\
	}

	REGISTER_COMPONENT(Position, position)
	REGISTER_COMPONENT(Rotation, rotation)
	REGISTER_COMPONENT(Velocity, velocity)
	REGISTER_COMPONENT(Sprite,   sprite)

	#undef COMPONENT_SET

	template<typename T>
	void component_remove(Game_Data * game_data, Entity entity) {
		Array_Dynamic<Component_Type> & flags = game_data->entities.flags;
		flags[entity] = bits_to_zero(flags[entity], get_type<T>());
	}

	template<typename T>
	bool component_has(Game_Data * game_data, Entity entity) {
		Array_Dynamic<Component_Type> & flags = game_data->entities.flags;
		return bits_are_set(flags[entity], get_type<T>());
	}

	Entity add(Game_Data * game_data) {
		Array_Dynamic<Component_Type>     & components_flags    = game_data->entities.flags;
		Array_Dynamic<Position_Component> & position_components = game_data->entities.position;
		Array_Dynamic<Rotation_Component> & rotation_components = game_data->entities.rotation;
		Array_Dynamic<Velocity_Component> & velocity_components = game_data->entities.velocity;
		Array_Dynamic<Sprite_Component>   & sprite_components   = game_data->entities.sprite;

		Entity entity = components_flags.length;
		components_flags.add(Component_Type::None);
		position_components.add();
		rotation_components.add();
		velocity_components.add();
		sprite_components.add();
		return entity;
	}
};

namespace rotation_system {
	void add(Game_Data * game_data, Entity entity) {
		if (!entities::component_has<Rotation_Component>(game_data, entity)) { return; }
		game_data->systems.rotation.add(entity);
	}

	void update(Game_Data * game_data) {
		Array_Dynamic<Entity> const       & entities  = game_data->systems.rotation;
		Array_Dynamic<Rotation_Component> & rotations = game_data->entities.rotation;

		r32 delta_time = globals::get_delta_seconds();
		Complex rotator = complex_from_radians((pi / 6) * delta_time);
		
		for (size_t i = 0; i < entities.length; ++i) {
			Entity               entity   = entities[i];
			Rotation_Component & rotation = rotations[entity];

			rotation.value = complex_rotate_vector(rotation.value, rotator);
			rotation.value = normalize(rotation.value);
		}
	}
};

namespace movement_system {
	void add(Game_Data * game_data, Entity entity) {
		if (!entities::component_has<Position_Component>(game_data, entity)) { return; }
		if (!entities::component_has<Velocity_Component>(game_data, entity)) { return; }
		game_data->systems.movement.add(entity);
	}

	void update(Game_Data * game_data) {
		Array_Dynamic<Entity> const             & entities   = game_data->systems.movement;
		Array_Dynamic<Position_Component>       & positions  = game_data->entities.position;
		Array_Dynamic<Velocity_Component> const & velocities = game_data->entities.velocity;

		r32 delta_time = globals::get_delta_seconds();
		
		for (size_t i = 0; i < entities.length; ++i) {
			Entity                     entity   = entities[i];
			Position_Component       & position = positions[entity];
			Velocity_Component const & velocity = velocities[entity];

			position.value += velocity.value * delta_time;
		}
	}
};

namespace world_bounds_system {
	void add(Game_Data * game_data, Entity entity) {
		if (!entities::component_has<Position_Component>(game_data, entity)) { return; }
		if (!entities::component_has<Velocity_Component>(game_data, entity)) { return; }
		game_data->systems.world_bounds.add(entity);
	}

	void update(Game_Data * game_data) {
		Array_Dynamic<Entity> const       & entities   = game_data->systems.world_bounds;
		Array_Dynamic<Position_Component> & positions  = game_data->entities.position;
		Array_Dynamic<Velocity_Component> & velocities = game_data->entities.velocity;

		Aabb2 const & bounds = game_data->world_bounds;
		Vector2 bounds_min = bounds.position - bounds.extents;
		Vector2 bounds_max = bounds.position + bounds.extents;

		for (size_t i = 0; i < entities.length; ++i) {
			Entity               entity   = entities[i];
			Position_Component & position = positions[entity];
			Velocity_Component & velocity = velocities[entity];

			if (position.value.x < bounds_min.x) {
				position.value.x = bounds_min.x;
				velocity.value.x = -velocity.value.x;
			}
			else if (position.value.x > bounds_max.x) {
				position.value.x = bounds_max.x;
				velocity.value.x = -velocity.value.x;
			}
			
			if (position.value.y < bounds_min.y) {
				position.value.y = bounds_min.y;
				velocity.value.y = -velocity.value.y;
			}
			else if (position.value.y > bounds_max.y) {
				position.value.y = bounds_max.y;
				velocity.value.y = -velocity.value.y;
			}
		}
	}
};

namespace rendering_system {
	void add(Game_Data * game_data, Entity entity) {
		if (!entities::component_has<Position_Component>(game_data, entity)) { return; }
		if (!entities::component_has<Rotation_Component>(game_data, entity)) { return; }
		if (!entities::component_has<Sprite_Component>(game_data, entity)) { return; }
		game_data->systems.rendering.add(entity);
	}

	void update(Game_Data * game_data) {
		Array_Dynamic<Entity> const             & entities  = game_data->systems.rendering;
		Array_Dynamic<Position_Component> const & positions = game_data->entities.position;
		Array_Dynamic<Rotation_Component> const & rotations = game_data->entities.rotation;
		Array_Dynamic<Sprite_Component> const   & sprites   = game_data->entities.sprite;
		
		static Vector2 const SIZE = {10, 10};

		Vector2 CAMERA_OFFSET = {
			globals::render_buffer.size.x * 0.5f,
			globals::render_buffer.size.y * 0.5f
		};

		r32 delta_time = globals::get_delta_seconds();
		
		for (size_t i = 0; i < entities.length; ++i) {
			Entity                     entity   = entities[i];
			Position_Component const & position = positions[entity];
			Rotation_Component const & rotation = rotations[entity];
			Sprite_Component const   & sprite   = sprites[entity];
			
			Vector2 axis_x = rotation.value;
			Vector2 axis_y = vec2(-axis_x.y, axis_x.x);
			Vector2 centre_offset = -(axis_x * SIZE.x + axis_y * SIZE.y) / 2;

			Renderable renderable;
			renderable.position = CAMERA_OFFSET + position.value + centre_offset;
			renderable.rotation = rotation.value;
			renderable.size     = SIZE;
			renderable.color    = sprite.color;

			game_data->renderables.add(renderable);
		}
	}
};

namespace rendering_no_rotation_system {
	void add(Game_Data * game_data, Entity entity) {
		if (!entities::component_has<Position_Component>(game_data, entity)) { return; }
		if (entities::component_has<Rotation_Component>(game_data, entity)) { return; }
		if (!entities::component_has<Sprite_Component>(game_data, entity)) { return; }
		game_data->systems.rendering_no_rotation.add(entity);
	}

	void update(Game_Data * game_data) {
		Array_Dynamic<Entity> const             & entities  = game_data->systems.rendering_no_rotation;
		Array_Dynamic<Position_Component> const & positions = game_data->entities.position;
		Array_Dynamic<Sprite_Component> const   & sprites   = game_data->entities.sprite;
		
		static Vector2 const SIZE = {10, 10};
		static Vector2 const ROTATION = {1, 0};
		
		Vector2 CAMERA_OFFSET = {
			globals::render_buffer.size.x * 0.5f,
			globals::render_buffer.size.y * 0.5f
		};

		r32 delta_time = globals::get_delta_seconds();
		
		for (size_t i = 0; i < entities.length; ++i) {
			Entity                     entity   = entities[i];
			Position_Component const & position = positions[entity];
			Sprite_Component const   & sprite   = sprites[entity];
			
			Vector2 axis_x = ROTATION;
			Vector2 axis_y = vec2(-axis_x.y, axis_x.x);
			Vector2 centre_offset = -(axis_x * SIZE.x + axis_y * SIZE.y) / 2;

			Renderable renderable;
			renderable.position = CAMERA_OFFSET + position.value + centre_offset;
			renderable.rotation = ROTATION;
			renderable.size     = SIZE;
			renderable.color    = sprite.color;

			game_data->renderables.add(renderable);
		}
	}
};

namespace game {
	Game_Data * allocate_game_memory() {
		auto game_data = globals::allocate_permanent<Game_Data>();
		*game_data = {};

		// entities memory
		game_data->entities.flags.set_capacity(ENTITIES_COUNT, &globals::allocate_permanent);
		game_data->entities.position.set_capacity(ENTITIES_COUNT, &globals::allocate_permanent);
		game_data->entities.rotation.set_capacity(ENTITIES_COUNT, &globals::allocate_permanent);
		game_data->entities.velocity.set_capacity(ENTITIES_COUNT, &globals::allocate_permanent);
		game_data->entities.sprite.set_capacity(ENTITIES_COUNT, &globals::allocate_permanent);

		// systems memory
		game_data->systems.rotation.set_capacity(ENTITIES_COUNT, &globals::allocate_permanent);
		game_data->systems.movement.set_capacity(ENTITIES_COUNT, &globals::allocate_permanent);
		game_data->systems.world_bounds.set_capacity(ENTITIES_COUNT, &globals::allocate_permanent);
		game_data->systems.rendering.set_capacity(ENTITIES_COUNT, &globals::allocate_permanent);
		game_data->systems.rendering_no_rotation.set_capacity(ENTITIES_COUNT, &globals::allocate_permanent);

		// rendering
		game_data->renderables.set_capacity(ENTITIES_COUNT, &globals::allocate_permanent);

		return game_data;
	}
	
	bool needs_init() {
		if (globals::permanent_memory->length == 0) { return true; }
		auto game_data = (Game_Data *)globals::permanent_memory->data;
		return memcmp(game_data->checksum, checksum, sizeof(checksum)) != 0;
	}

	void init() {
		auto game_data = allocate_game_memory();
		memcpy(game_data->checksum, checksum, sizeof(checksum));
		game_data->random_state = (u32)globals::frame_timestamp;

		game_data->world_bounds = BOUNDS;
		
		Aabb2 const & bounds = BOUNDS;
		Vector2 bounds_min = bounds.position - bounds.extents;
		Vector2 bounds_max = bounds.position + bounds.extents;

		for (; game_data->entities.flags.length < ENTITIES_COUNT;) {
			Entity entity = entities::add(game_data);

			Position_Component position_component;
			position_component.value = {
				interpolate(bounds_min.x, bounds_max.x, random_01(&game_data->random_state)),
				interpolate(bounds_min.y, bounds_max.y, random_01(&game_data->random_state))
			};
			entities::component_set(game_data, entity, position_component);
			
			if (random_01(&game_data->random_state) < 0.8f) {
				Rotation_Component rotation_component;
				rotation_component.value = random2_radius01(&game_data->random_state);
				rotation_component.value = normalize(rotation_component.value);
				rotation_component.value = complex_reciprocal(rotation_component.value);
				entities::component_set(game_data, entity, rotation_component);
			}
			
			if (random_01(&game_data->random_state) < 0.5f) {
				Velocity_Component velocity_component;
				velocity_component.value = random2_radius01(&game_data->random_state) * 80 + 20;
				entities::component_set(game_data, entity, velocity_component);
			}

			Sprite_Component sprite_component;
			sprite_component.color = random4_01(&game_data->random_state) * 0.5f + 0.5f;
			entities::component_set(game_data, entity, sprite_component);
		}
		
		for (Entity entity = 0; entity < game_data->entities.flags.length; ++entity) {
			rotation_system::add(game_data, entity);
			movement_system::add(game_data, entity);
			world_bounds_system::add(game_data, entity);
			rendering_system::add(game_data, entity);
			rendering_no_rotation_system::add(game_data, entity);
		}
	}
	
	void update(Game_Data * game_data) {
		game_data->renderables.length = 0;

		rotation_system::update(game_data);
		movement_system::update(game_data);
		world_bounds_system::update(game_data);
		rendering_system::update(game_data);
		rendering_no_rotation_system::update(game_data);
	}
}
