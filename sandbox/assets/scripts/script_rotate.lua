function script_rotate(entity, dt)
	local rotator = quat.from_radians(0, 1 * dt, 0)
	local transform = entity:get_component(Transform.type)
	transform.rotation = quat.product(transform.rotation, rotator)
end
