local lua_tag = "\x1b[38;5;202m" .. "[lua]" .. "\x1b[0m" .. " "

function global_init_prefab_test()
	-- camera to debug the scene
	Asset.add(Prefab_Asset.type, "assets/prefabs/camera flying.prefab")

	-- load and instantiate
	Asset.add(Prefab_Asset.type, "assets/prefabs/- scene - prefabs test.prefab")

	-- set up
end

local counter = 0
function global_update_prefab_test()
	counter = counter + 1
	if counter == 100 then
		print(lua_tag .. "global_update_prefab_test (once in " .. counter .. " frames)")
		counter = 0
	end
end
