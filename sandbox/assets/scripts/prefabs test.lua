local lua_tag = "\x1b[38;5;202m" .. "[lua]" .. "\x1b[0m" .. " "

function global_init_prefab_test()
	print(lua_tag .. "global_init_prefab_test")

	-- this camera is expected to be instanced automatically
	Asset.add(Prefab_Asset.type, "assets/prefabs/camera flying.entity")

	local floor_prefab = Asset.add(Prefab_Asset.type, "assets/prefabs/floor.entity")
	local suzanne_rotating_prefab = Asset.add(Prefab_Asset.type, "assets/prefabs/suzanne rotating.entity")

	Prefab_Asset.instantiate(floor_prefab)
	Prefab_Asset.instantiate(suzanne_rotating_prefab)

	local camera_prefab = Asset.add(Prefab_Asset.type, "assets/prefabs/camera.entity")
	local camera = Prefab_Asset.promote_to_instance(camera_prefab)
	local camera_component = camera:get_component(Camera.type)
	camera_component.clear = 2;
	camera_component.layer = 1;

	-- Asset.destroy(camera_prefab)
end

local counter = 0
function global_update_prefab_test()
	counter = counter + 1
	if counter == 100 then
		print(lua_tag .. "global_update_prefab_test (once in " .. counter .. " frames)")
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
