static u8 const checksum[8] = {'r', 'a', 'y', 't', 'r', 'a', 'c', 'e'};

//
//
//

struct Game_Data {
	u8 checksum[8];
	u32 random_state;
};
