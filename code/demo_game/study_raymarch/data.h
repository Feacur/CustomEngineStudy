static u8 const checksum[8] = {'r', 'a', 'y', 'm', 'a', 'r', 'c', 'h'};

//
//
//

struct Game_Data {
	u8 checksum[8];
	u32 random_state;
};
