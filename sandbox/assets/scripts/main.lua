local lua_tag = "\x1b[38;5;202m" .. "[lua]" .. "\x1b[0m" .. " "

function global_init()
	print(lua_tag .. "global_init")

	local lua_entity = Entity.create()
	local lua_transform = lua_entity:add_component(Transform.type)
	local lua_visual = lua_entity:add_component(Visual.type)
	lua_transform.position = vec3.new(0, 1, 0)
	lua_transform.rotation = vec4.new(0, 0, 0, 1)
	lua_transform.scale = vec3.new(1, 1, 1)
	lua_visual.shader = 1;
	lua_visual.texture = 0;
	lua_visual.mesh = 1;
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
	local transform = entity:get_component(Transform.type)
	-- if (transform ~= nil) then
	-- 	local position = transform.position
	-- 	print(lua_tag .. string.format("position is %s, dot is %f", position, vec3.dot(position, position)))
	-- else
	-- 	print(lua_tag .. "transform is nil")
	-- end
end
