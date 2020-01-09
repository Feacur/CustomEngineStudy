Ray3 camera_sample_ray(Game_Data * game_data, Vector3 position, Quaternion rotation, Vector3 direction, float focus, float aperture) {
	// anti-alias
	direction.xy = direction.xy + random2_radius01(&game_data->random_state);
	direction = normalize(direction);
	
	// imitate focus blur by offsetting ray and then
	// converging it to the base direction at the target distance
	Vector3 aperture_offset = {random2_radius01(&game_data->random_state) * aperture, 0};
	position += aperture_offset;
	direction = normalize(direction * focus - aperture_offset);
	
	return {position, quaternion_rotate_vector(rotation, direction)};
}
