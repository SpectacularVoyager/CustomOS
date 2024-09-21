#include "paging.h"
#include"stdint.h"
#include <stdint.h>
#include "../stdlib/stdlib.h"
#include "../stdlib/stdio.h"

unsigned int pageCount=0;
#define PAGE_P2_SIZE 0x200000L

#define TABLE(x) ((uint64_t*)((uint64_t)x&(~0xff)))
#define PAGE(x) ((uint64_t*)((uint64_t)x&(~0x1fffff)))
void PageSetup(uint64_t fbindex){
	p3_table[0]=(uint64_t)p2_table|0b11;
}
void AllocatePage(int p,unsigned long address,unsigned int flags){
	if((TABLE(p3_table)[p])&0x3){
		kprintf(TRACE"PAGE 0x%x ALREADY ALLOCATED\n",p);
		return;
	}
	uint64_t* ptr=mallocA(0x1000,0x1000);
	kprintf(TRACE "ALLOCATED PAGE TABLE(%d ->[%p]) AT %X\n",p,address,ptr);
	p3_table[p]=(uint64_t)ptr|0b11;
	for(int i=0;i<512;i++){
		ptr[i]=(PAGE_P2_SIZE*i+address)|0b10000011L|flags;
	}
}
void PageRemap(int p,uint64_t offset,uint64_t address,int flags){
	uint64_t* table=TABLE(p3_table[p]);
	table[offset]=address|0b10000011L|flags;
	asm volatile("invlpg (%0)" ::"r" (address) : "memory");
}
