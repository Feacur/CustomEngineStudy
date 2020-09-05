function script_rotate(entity, dt)
	local transform = entity:get_component(Transform.type)
	if transform == nil then Debug.custom_assert(false, "entity has no Transform") return end

	local rotator = quat.from_radians(0, 1 * dt, 0)
	transform.rotation = quat.product(transform.rotation, rotator)
end
