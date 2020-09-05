function script_control_2d(entity, dt)
	if not Input.get_key(Key_Code.Shift) then return end

	local direction = vec2.new(
		(Input.get_key(Key_Code.D) and 1 or 0) - (Input.get_key(Key_Code.A) and 1 or 0),
		(Input.get_key(Key_Code.W) and 1 or 0) - (Input.get_key(Key_Code.S) and 1 or 0)
	);

	local phys2d = entity:get_component(Phys2d.type)
	phys2d.velocity = direction * 10;
end
