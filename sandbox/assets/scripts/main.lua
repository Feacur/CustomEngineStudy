local lua_tag = "\x1b[38;5;202m" .. "[lua]" .. "\x1b[0m" .. " "

function global_init()
	print(lua_tag .. "global_init")

	local lua_entity = Entity.create()
	local lua_transform = lua_entity:add_component(Component_Type.Transform)
	local lua_visual = lua_entity:add_component(Component_Type.Visual)
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
	local transform = entity:get_component(Component_Type.Transform)
	-- if (transform ~= nil) then
	-- 	local position = transform:get_position();
	-- 	print(lua_tag .. string.format("position is %s, dot is %f", position, vec3.dot(position, position)))
	-- else
	-- 	print(lua_tag .. "transform is nil")
	-- end
end
