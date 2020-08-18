local lua_tag = "\x1b[38;5;202m" .. "[lua]" .. "\x1b[0m" .. " "

local function create_visual(tp, tr, ts, vs, vt, vm)
	local entity = Entity.create()

	local transform = entity:add_component(Transform.type)
	transform.position = tp
	transform.rotation = tr
	transform.scale = ts

	local visual = entity:add_component(Visual.type)
	visual.shader = vs;
	visual.texture = vt;
	visual.mesh = vm;

	return entity
end

local function create_camera()
	local entity = Entity.create()

	local transform = entity:add_component(Transform.type)
	transform.position = vec3.new(0, 2, -5)
	transform.rotation = vec4.new(0, 0, 0, 1)
	transform.scale    = vec3.new(1, 1, 1)

	local camera = entity:add_component(Camera.type)
	camera.near = 0.1
	camera.far = 20
	camera.scale = 1 -- 1 / math.tan((math.pi / 2) / 2)
	camera.persp = 1

	local camera_script = entity:add_component(Lua_Script.type)
	camera_script.update = "script_fly"
end

function global_init()
	print(lua_tag .. "global_init")

	local shader2d = Asset.add(Shader_Asset.type, "assets/shaders/v2_texture_tint.glsl")
	local shader3d = Asset.add(Shader_Asset.type, "assets/shaders/v3_texture_tint.glsl")

	local texture1 = Asset.add(Texture_Asset.type, "assets/textures/checkerboard.png")
	local texture2 = Asset.add(Texture_Asset.type, "assets/textures/blue_noise.png")
	local texture3 = Asset.add(Texture_Asset.type, "assets/textures/proto_blue.png")

	local mesh1 = Asset.add(Mesh_Asset.type, "assets/meshes/plane_xz.obj")
	local mesh2 = Asset.add(Mesh_Asset.type, "assets/meshes/suzanne.obj")

	local shader = Asset.get(Shader_Asset.type, "assets/shaders/v3_texture_tint.glsl")
	print(Asset.get_path(Shader_Asset.type, shader))

	create_visual(
		vec3.new(0, 0, 0), vec4.new(0, 0, 0, 1), vec3.new(10, 10, 10),
		shader3d, texture3, mesh1
	)

	create_visual(
		vec3.new(-4, 1, 0), vec4.new(0, 0, 0, 1), vec3.new(2, 1, 2),
		shader, texture1, mesh2
	)

	local some_entity = create_visual(
		vec3.new(0, 1, 0), vec4.new(0, 0, 0, 1), vec3.new(1, 1, 1),
		shader, texture1, mesh2
	)
	local some_script = some_entity:add_component(Lua_Script.type)
	some_script.update = "script_rotate"

	create_visual(
		vec3.new(4, 2, 0), vec4.new(0, 0, 0, 1), vec3.new(1, 2, 1),
		shader, texture1, mesh2
	)

	create_camera()
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

function script_fly(entity, dt)
	local mouse_wheel = Input.get_mouse_wheel()
	if (mouse_wheel.y ~= 0) then
		local camera = entity:get_component(Camera.type)
		camera.scale = math.min(math.max(camera.scale + mouse_wheel.y * dt, 0.5), 2.0)
	end

	if (not Input.get_mouse_key(Mouse_Code.Key2)) then return end

	local position_delta = vec3.new(
		(Input.get_key(Key_Code.D) and 1 or 0) - (Input.get_key(Key_Code.A) and 1 or 0),
		(Input.get_key(Key_Code.E) and 1 or 0) - (Input.get_key(Key_Code.Q) and 1 or 0),
		(Input.get_key(Key_Code.W) and 1 or 0) - (Input.get_key(Key_Code.S) and 1 or 0)
	);

	local move_speed = (Input.get_key(Key_Code.Shift) and 4 or 1)
	local rotation_speed = 0.3

	local mouse_delta = Input.get_mouse_delta();
	local rotation_delta = quat.from_radians(
		-mouse_delta.y * (rotation_speed * dt),
		mouse_delta.x * (rotation_speed * dt),
		0
	)

	local transform = entity:get_component(Transform.type)
	transform.position = transform.position + position_delta * (move_speed * dt);
	transform.rotation = quat.product(transform.rotation, rotation_delta)
end
