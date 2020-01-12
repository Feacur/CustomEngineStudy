static u8 const checksum[8] = {'s', 'o', 'u', 'n', 'd', 't', 'e', 's'};

//
//
//

static Complex const ROTATION_VECTOR = {1, 0};

struct Game_Data {
	u8 checksum[8];
	u32 random_state;
};
