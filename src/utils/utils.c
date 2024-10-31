#include"utils.h"

void printWStr(uint16_t* ptr,size_t len){
	FORI(len){
		printf("%c",ptr[i]);
	}
	printf("\n");
}
