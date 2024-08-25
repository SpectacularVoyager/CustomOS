#include "paging.h"
#include"stdint.h"
#include <stdint.h>
uint64_t __attribute__((aligned(4096))) fbpage[512];
uint64_t __attribute__((aligned(4096))) tempPage1[512];

void PageSetup(){
	p3_table[0]=(uint64_t)p2_table|0b11;
	unsigned int fbindex   =0xFD000000;
	unsigned int page_width=0x40000000;
	p3_table[fbindex/page_width]=(uint64_t)fbpage|0b11;
}
void putPage(uint64_t addr,int size){
	//uint64_t addr=0x40000000*3L;
	for(int i=0;i<512;i++){
		fbpage[i]=(0x200000L*i+addr)|0b10000011L;
	}
}
void identitymap(uint64_t addr){
	unsigned int page_width=0x40000000;
	p3_table[addr/page_width]=(uint64_t)tempPage1|0b11;

	for(int i=0;i<512;i++){
		tempPage1[i]=(0x200000L*i+addr)|0b10000011L;
	}
}
