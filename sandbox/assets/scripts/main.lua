function global_init()
	Asset.add(Lua_Asset.type, "assets/scripts/physics test.lua")
	global_init_physics_test()
	Asset.add(Lua_Asset.type, "assets/scripts/prefabs test.lua")
	global_init_prefab_test()
end

function global_update()
	global_update_physics_test()
	-- global_update_prefab_test()
end
