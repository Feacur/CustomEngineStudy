function script_control_phys2d(entity, dt)
	local phys2d = entity:get_component(Phys2d.type)
	if phys2d == nil then Debug.custom_assert(false, "entity has no Phys2d") return end

	local direction = vec2.new(
		(Input.get_key(Key_Code.D) and 1 or 0) - (Input.get_key(Key_Code.A) and 1 or 0),
		(Input.get_key(Key_Code.W) and 1 or 0) - (Input.get_key(Key_Code.S) and 1 or 0)
	);

	local move_speed = (Input.get_key(Key_Code.Shift) and 20 or 10)

	phys2d.velocity = phys2d.velocity + direction * (move_speed * dt);
end
