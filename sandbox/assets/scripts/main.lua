local lua_tag = "\x1b[38;5;202m" .. "[lua]" .. "\x1b[0m" .. " "

function global_init()
	print(lua_tag .. "global_init")

	local camera_prefab = Asset.add(Prefab_Asset.type, "assets/prefabs/camera.entity")
	local camera_flying_prefab = Asset.add(Prefab_Asset.type, "assets/prefabs/camera flying.entity")
	local floor_prefab = Asset.add(Prefab_Asset.type, "assets/prefabs/floor.entity")
	local suzanne_prefab = Asset.add(Prefab_Asset.type, "assets/prefabs/suzanne.entity")
	local suzanne_rotating_prefab = Asset.add(Prefab_Asset.type, "assets/prefabs/suzanne rotating.entity")

	local suzanne1 = Prefab_Asset.instantiate(suzanne_prefab)
	local transform1 = suzanne1:get_component(Transform.type)
	transform1.position = vec3.new(-4, 1, 0)
	transform1.scale = vec3.new(2, 1, 2)

	Prefab_Asset.instantiate(floor_prefab)
	Prefab_Asset.instantiate(suzanne_rotating_prefab)

	local suzanne2 = Entity.copy(suzanne1)
	local transform2 = suzanne2:get_component(Transform.type)
	transform2.position = vec3.new(4, 2, 0)
	transform2.scale = vec3.new(1, 2, 1)

	Prefab_Asset.instantiate(camera_prefab)
	Prefab_Asset.instantiate(camera_flying_prefab)
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
	if (mouse_wheel.y ~= 0) and entity:has_component(Camera.type) then
		local camera = entity:get_component(Camera.type)
		if Input.get_key(Key_Code.Control) then
			camera.ortho = math.min(math.max(camera.ortho + mouse_wheel.y * 4 * dt, 0), 1)
		elseif Input.get_key(Key_Code.Alt) then
			camera.far = math.min(math.max(camera.far + mouse_wheel.y * 10 * dt, 0), math.huge)
		else
			camera.scale = math.min(math.max(camera.scale + mouse_wheel.y * 2 * dt, 0.5), 2)
		end
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
	transform.position = transform.position + quat.rotate(transform.rotation, position_delta) * (move_speed * dt);
	transform.rotation = quat.product(transform.rotation, rotation_delta)
end
