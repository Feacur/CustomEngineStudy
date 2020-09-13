function global_init_physics_test()
	-- camera to debug the scene
	-- Asset.add(Prefab_Asset.type, "assets/prefabs/camera flying.prefab")

	-- load and instantiate
	local camera_prefab = Asset.add(Prefab_Asset.type, "assets/prefabs/camera.prefab")
	local camera = Prefab_Asset.promote_to_instance(camera_prefab)

	Asset.add(Prefab_Asset.type, "assets/prefabs/- scene - physics test 2.prefab")

	-- set up
	local camera_transform = camera:get_component(Transform.type)
	camera_transform.position = vec3.new(8, 4, -10)

	local camera_component = camera:get_component(Camera.type)
	camera_component.ortho = 1
	camera_component.scale = 0.08
end

function global_update_physics_test()
end
