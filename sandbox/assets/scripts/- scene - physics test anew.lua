function global_init_physics_test_anew()
	-- camera to debug the scene
	-- Asset.add(Prefab_Asset.type, "assets/prefabs/camera flying.prefab")

	-- load and instantiate
	local camera_prefab = Asset.add(Prefab_Asset.type, "assets/prefabs/camera.prefab")
	local camera = Prefab_Asset.promote_to_instance(camera_prefab)

	Asset.add(Prefab_Asset.type, "assets/prefabs/- scene - physics test anew.prefab")

	local square_phys_prefab = Asset.add(Prefab_Asset.type, "assets/prefabs/square phys.prefab")

	-- set up
	local camera_transform = camera:get_component(Transform.type)
	camera_transform.position = vec3.new(0, 10, -10)
	camera_transform.scale = vec3.new(0.5, 0.5, 1)

	local camera_component = camera:get_component(Camera.type)
	camera_component.ortho = 1
	camera_component.scale = 0.08

	local origin = vec3.new(-6.0, 0.75, 0.0)
	for i=1,12 do
		local position = vec3.new(origin.x, origin.y, origin.z)
		for j=i,12 do
			local square_phys = square_phys_prefab:instantiate()
			local square_phys_transform = square_phys:get_component(Transform.type)
			square_phys_transform.position = position
			position = position + vec3.new(1.125, 0.0, 0.0)
		end
		origin = origin + vec3.new(0.5625, 2.0, 0.0)
	end
end

function global_update_physics_test_anew()
end
