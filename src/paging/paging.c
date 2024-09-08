#include "paging.h"
#include"stdint.h"
#include <stdint.h>

uint64_t __attribute__((aligned(4096))) fbpage[512];
uint64_t __attribute__((aligned(4096))) tempPage[512];
unsigned int pageCount=0;

unsigned int page_width=0x40000000;
PageTable mainTable;
void PageSetup(uint64_t fbindex){
	p3_table[0]=(uint64_t)p2_table|0b11;
	//unsigned int fbindex   =0xFD000000;
	p3_table[fbindex/page_width]=(uint64_t)fbpage|0b11;
}
void putPage(uint64_t addr,int size){
	//uint64_t addr=0x40000000*3L;
	for(int i=0;i<512;i++){
		fbpage[i]=(0x200000L*i+addr)|0b10000011L;
	}
}
Page PageAlloc(){
	Page page={.offset=pageCount};
	pageCount++;
	return page;
}
void identitymap(uint64_t addr){
	p3_table[addr/page_width]=(uint64_t)tempPage|0b11;

	for(int i=0;i<512;i++){
		tempPage[i]=(0x200000L*i+addr)|0b10000011L;
	}
}
void PageCreateEmpty(int id){
	p3_table[id]=(uint64_t)tempPage|0b11;
	//USE MALLOC
	for(int i=0;i<512;i++){
		tempPage[i]=((0x200000L*i)+0x40000000*2L)|0b10000011L;
	}
}
