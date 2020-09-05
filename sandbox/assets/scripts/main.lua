-- executed on load
Asset.reset_system(Prefab_Asset.type)
Entity.reset_system()

Asset.add(Lua_Asset.type, "assets/scripts/physics test.lua")
global_init_physics_test()

-- Asset.add(Lua_Asset.type, "assets/scripts/prefabs test.lua")
-- global_init_prefab_test()

-- executed on the app start
function global_init()
end

-- executed each frame
function global_update()
	-- global_update_physics_test()
	-- global_update_prefab_test()
end
