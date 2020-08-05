local lua_tag = "\x1b[38;5;202m" .. "[lua]" .. "\x1b[0m" .. " "

function global_init()
	print(lua_tag .. "global_init")

	local lua_entity = custom_entity.create()
	local lua_transform = lua_entity:add_component(custom_entity.component.Transform)
	local lua_visual = lua_entity:add_component(custom_entity.component.Visual)
	lua_transform:set()
	lua_visual:set()
end

local counter = 0
function global_update()
	counter = counter + 1
	if counter == 100 then
		print(lua_tag .. "global_update (once in " .. counter .. " frames)")
		counter = 0
	end
end

function some_component_update(entity)
	-- if (entity == nil) then
	-- 	print(lua_tag .. "entity is nil")
	-- 	return
	-- end
	-- print(lua_tag .. string.format("entity is %s", entity))
	local transform = entity:get_component(custom_entity.component.Transform)
	-- if (transform ~= nil) then
	-- 	print(lua_tag .. string.format("position is %s", transform:get_position()))
	-- else
	-- 	print(lua_tag .. "transform is nil")
	-- end
end
