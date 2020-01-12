#include "demo_c/main_common.h"

typedef struct {
	s32 a;
	s32 b;
	s32 c;
	s32 d;
	s32 e;
} Some_Struct;

int main(int argc, char * argv[]) {
	printf("Hello, World!\n");

	Some_Struct data = (Some_Struct) {
		.a = 1,
		.c = 3,
		.e = 5,
	};
	
	printf("a is %d; b is %d; c is %d; d is %d; e is %d\n", data.a, data.b, data.c, data.d, data.e);

	getchar();
	return 0;
}
