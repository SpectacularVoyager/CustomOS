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
void khexdump(void* mem,size_t size,size_t stride){
	uint8_t* mat=mem;
	int x=0;
	FORI(size/stride){
		kprintf("0x%02x\t",stride*x);
		FORJ(stride){
			kprintf("%02x ",mat[stride*i+j]);
		}
		kprintf("\n");
			x++;
	}
	kprintf("0x%02x\t",stride*x);
	FORI(size%stride){
		x++;
		kprintf("%02x ",mat[size/stride+i]);
	}
	kprintf("\n");
}
