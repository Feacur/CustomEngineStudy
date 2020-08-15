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
	return entity
end

function global_init()
	print(lua_tag .. "global_init")

	local shader2d = Asset.add(Shader_Asset.type, "assets/shaders/v2_texture_tint.glsl")
	local shader3d = Asset.add(Shader_Asset.type, "assets/shaders/v3_texture_tint.glsl")

	local shader = Asset.get(Shader_Asset.type, "assets/shaders/v3_texture_tint.glsl")
	print(Asset.get_path(Shader_Asset.type, shader))

	create_visual(
		vec3.new(0, 0, 0), vec4.new(0, 0, 0, 1), vec3.new(10, 10, 10),
		shader3d, 2, 0
	)

	create_visual(
		vec3.new(-4, 1, 0), vec4.new(0, 0, 0, 1), vec3.new(2, 1, 2),
		shader, 0, 1
	)

	local some_entity = create_visual(
		vec3.new(0, 1, 0), vec4.new(0, 0, 0, 1), vec3.new(1, 1, 1),
		shader, 0, 1
	)
	local some_script = some_entity:add_component(Lua_Script.type)
	some_script.update = "script_rotate"

	create_visual(
		vec3.new(4, 2, 0), vec4.new(0, 0, 0, 1), vec3.new(1, 2, 1),
		shader, 0, 1
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

function script_rotate(entity, dt)
	local rotator = quat.from_radians(0, 1 * dt, 0)
	local transform = entity:get_component(Transform.type)
	transform.rotation = quat.product(transform.rotation, rotator)
end
