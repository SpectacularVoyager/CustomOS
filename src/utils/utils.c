#include"utils.h"
#include "stdlib/stdio.h"

void printWStr(uint16_t* ptr,size_t len){
	FORI(len){
		printf("%c",ptr[i]);
	}
	printf("\n");
}
void Utils_switchColor(int i){
	switch(i%4){
		case 0:
			SetColor(0xFFFFFF);
			break;
		case 1:
			SetColor(0xFF0000);
			break;
		case 2:
			SetColor(0x00FF00);
			break;
		case 3:
			SetColor(0x0000FF);
			break;
	}
}

void hexdump(void* mem,size_t size,size_t stride){
	uint8_t* mat=mem;
	int x=0;
	FORI(size/stride){
		FORJ(stride){
			Utils_switchColor(j);
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
	SetColor(0xFFFFFF);
}
