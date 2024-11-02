#include"utils.h"

void printWStr(uint16_t* ptr,size_t len){
	FORI(len){
		printf("%c",ptr[i]);
	}
	printf("\n");
}

void hexdump(void* mem,size_t size,size_t stride){
	uint8_t* mat=mem;
	int x=0;
	FORI(size/stride){
		FORJ(stride){
			x++;
			printf("%02x ",mat[stride*i+j]);
		}
		printf("\n");
	}
	FORI(size%stride){
		x++;
		printf("%02x ",mat[size/stride+i]);
	}
	printf("\n");
}
