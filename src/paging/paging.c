#include "paging.h"
#include"stdint.h"
#include <stdint.h>
#include "../stdlib/stdlib.h"
#include "../stdlib/stdio.h"

unsigned int pageCount=0;

void PageSetup(uint64_t fbindex){
	p3_table[0]=(uint64_t)p2_table|0b11;
}
void AllocatePage(int p,unsigned long address){
	uint64_t* ptr=mallocA(4096,4096);
	kprintf(TRACE "ALLOCATED PAGE TABLE(%d ->[%p]) AT %X\n",p,address,ptr);
	p3_table[p]=(uint64_t)ptr|0b11;
	for(int i=0;i<512;i++){
		ptr[i]=(0x200000L*i+address)|0b10000011L;
	}
}
