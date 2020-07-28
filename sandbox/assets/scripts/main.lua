local counter = 0

function ecs_update()
	counter = counter + 1
	if counter > 100 then
		counter = 0
		print("update")
	end
end
