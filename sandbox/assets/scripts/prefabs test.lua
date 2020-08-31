local lua_tag = "\x1b[38;5;202m" .. "[lua]" .. "\x1b[0m" .. " "

function global_init_prefab_test()
	print(lua_tag .. "global_init_prefab_test")

	-- this camera is expected to be instanced automatically
	Asset.add(Prefab_Asset.type, "assets/prefabs/camera flying.entity")

	local floor_prefab = Asset.add(Prefab_Asset.type, "assets/prefabs/plane with suzannes.entity")
	local suzanne_rotating_prefab = Asset.add(Prefab_Asset.type, "assets/prefabs/stack of rotating suzannes.entity")

	Prefab_Asset.instantiate(floor_prefab)
	Prefab_Asset.instantiate(suzanne_rotating_prefab)

	local camera_prefab = Asset.add(Prefab_Asset.type, "assets/prefabs/camera.entity")
	local camera = Prefab_Asset.promote_to_instance(camera_prefab)
	local camera_component = camera:get_component(Camera.type)
	camera_component.clear = 2;
	camera_component.layer = 1;

	-- Asset.destroy(camera_prefab)
end

local counter = 0
function global_update_prefab_test()
	counter = counter + 1
	if counter == 100 then
		print(lua_tag .. "global_update_prefab_test (once in " .. counter .. " frames)")
		counter = 0
	end
end
