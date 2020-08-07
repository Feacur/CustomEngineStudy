local lua_tag = "\x1b[38;5;202m" .. "[lua]" .. "\x1b[0m" .. " "

local function create_visual(tp, tr, ts, vs, vt, vm)
	local entity = Entity.create()
	local transform = entity:add_component(Transform.type)
	local visual = entity:add_component(Visual.type)
	transform.position = tp
	transform.rotation = tr
	transform.scale = ts
	visual.shader = vs;
	visual.texture = vt;
	visual.mesh = vm;
end

function global_init()
	print(lua_tag .. "global_init")

	create_visual(
		vec3.new(-4, 1, 0), vec4.new(0, 0, 0, 1), vec3.new(2, 1, 2),
		1, 0, 1
	)

	create_visual(
		vec3.new(0, 1, 0), vec4.new(0, 0, 0, 1), vec3.new(1, 1, 1),
		1, 0, 1
	)

	create_visual(
		vec3.new(4, 2, 0), vec4.new(0, 0, 0, 1), vec3.new(1, 2, 1),
		1, 0, 1
	)
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
