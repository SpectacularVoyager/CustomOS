#include"utils.h"

void printWStr(uint16_t* ptr,size_t len){
	FORI(len){
		printf("%c",ptr[i]);
	}
	printf("\n");
}

inline void hexdump(void* mem,size_t size,size_t stride){
	uint8_t* mat=mem;
	FORI(size/stride){
		FORJ(stride){
			printf("%x ",mat[stride*i+j]);
		}
		printf("\n");
	}
	FORI(stride){
		printf("%x ",mat[size/stride+i]);
	}
	printf("\n");
}
