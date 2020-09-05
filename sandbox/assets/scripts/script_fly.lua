function script_fly(entity, dt)
	local transform = entity:get_component(Transform.type)
	if transform == nil then Debug.custom_assert(false, "entity has no Transform") return end

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

	local direction = vec3.new(
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

	transform.position = transform.position + quat.rotate(transform.rotation, direction) * (move_speed * dt);
	transform.rotation = quat.product(transform.rotation, rotation_delta)
end
