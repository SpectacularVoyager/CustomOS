#include "string.h"
#include <stdint.h>
void memcpy(void* dest,const void* src,int size){
	uint8_t* _dest=(uint8_t*)dest;
	uint8_t* _src=(uint8_t*)src;
	for(int i=0;i<size;i++)_dest[i]=_src[i];
} 
void memset(void* dest,uint8_t val,int size){
	uint8_t* _dest=(uint8_t*)dest;
	for(int i=0;i<size;i++)_dest[i]=val;
} 
