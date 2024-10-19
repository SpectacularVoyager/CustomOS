#include "string.h"
void memcpy(void* dest,const void* src,int size){
	uint8_t* _dest=(uint8_t*)dest;
	uint8_t* _src=(uint8_t*)src;
	for(int i=0;i<size;i++)_dest[i]=_src[i];
} 
void memset(void* dest,uint8_t val,int size){
	uint8_t* _dest=(uint8_t*)dest;
	for(int i=0;i<size;i++)_dest[i]=val;
} 
 int strncmp(const char *s1, const char *s2, size_t n){
	for(size_t i=0;i<n;i++){
		if(s1[i]!=s2[i])return s1[i]-s2[i];
	}
	return 0;
}
char* strchr(char *s, int c){
	while(1){
		if(*s=='\0')return NULL;
		if(*s==c)return s;
		s++;
	}
}
char* strchrnul(char *s, int c){
	while(1){
		if(*s=='\0')return s;
		if(*s==c)return s;
		s++;
	}
}

void memcpy32(uint32_t* src,uint32_t* dest,int n){
	for(volatile int i=0;i<n;i++){
		dest[i]=src[i];
	}
}
void memset32(uint32_t* dest,uint32_t src,int n){
	for(volatile int i=0;i<n;i++){
		dest[i]=src;
	}
}
