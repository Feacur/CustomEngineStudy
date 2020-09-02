function script_control_2d(entity, dt)
	local position_delta = vec3.new(
		(Input.get_key(Key_Code.D) and 1 or 0) - (Input.get_key(Key_Code.A) and 1 or 0),
		(Input.get_key(Key_Code.W) and 1 or 0) - (Input.get_key(Key_Code.S) and 1 or 0),
		0
	);

	local move_speed = (Input.get_key(Key_Code.Shift) and 15 or 1)
	position_delta.x = position_delta.x * 5
	position_delta.y = position_delta.y * move_speed

	local transform = entity:get_component(Transform.type)
	transform.position = transform.position + position_delta * (dt);
end
