#include "paging.h"
#include"stdint.h"
#include <stdint.h>
#include "stdlib/stdlib.h"
#include "stdlib/stdio.h"
#include "utils/utils.h"

unsigned int pageCount=0;
#define PAGE_P2_SIZE 0x200000L

#define TABLE(x) ((uint64_t*)((uint64_t)x&(~0xff)))
#define PAGE(x) ((uint64_t*)((uint64_t)x&(~0x1fffff)))
inline void invlpg(uint64_t address){
	asm volatile("invlpg (%0)" ::"r" (address) : "memory");
}
void PageSetup(uint64_t fbindex){
	p3_table[0]=(uint64_t)p2_table|0b111;
}
void AllocatePage(int p,unsigned long address,unsigned int flags){
	if((TABLE(p3_table)[p])&0x3){
		kprintf(TRACE"PAGE 0x%x ALREADY ALLOCATED\n",p);
		return;
	}
	uint64_t* ptr=mallocA(0x1000,0x1000);
	kprintf(TRACE "ALLOCATED PAGE TABLE(%d ->[%p]) AT %X\n",p,address,ptr);
	p3_table[p]=(uint64_t)ptr|0b111;
	for(int i=0;i<512;i++){
		ptr[i]=(PAGE_P2_SIZE*i+address)|0b10000111L|flags;
	}
}
void UnmapPage(uint64_t page){
	p2_table[0]=0;
	// int p=page/PAGE_WIDTH;
	// int offset=(page%PAGE_WIDTH)/PAGE_P2_SIZE;
	// uint64_t* table=TABLE(p3_table[p]);
	// table[offset]=0x0;
	// KLOGVALD(p3_table)
	invlpg(page);
}
void PageRemap(int p,uint64_t offset,uint64_t address,int flags){
	uint64_t* table=TABLE(p3_table[p]);
	table[offset]=address|0b10000111L|flags;
	invlpg(address);
}
void MemoryRemap(uint64_t memory,uint64_t address,int flags){
	int p=memory/PAGE_WIDTH;
	int offset=(memory%PAGE_WIDTH)/PAGE_P2_SIZE;
	uint64_t* table=TABLE(p3_table[p]);
	kprintf(TRACE"REMAPPING %p to %x[%x]\n",address,p,offset);
	table[offset]=address|0b10000111L|flags;
	asm volatile("invlpg (%0)" ::"r" (address) : "memory");
}
void PageIdentity(int p1,int p2,int flags){
	p3_table[p1]=(p2*PAGE_WIDTH)|0b111|flags;
}
void* MemoryPhysical(void* p_addr){
	uint64_t addr=(uint64_t)p_addr;

	uint64_t _p=p3_table[addr/PAGE_WIDTH];
	uint64_t* p=TABLE(_p);

	if((_p&PAGE_PRESENT)==0)return NULL;

	uint64_t off=addr%PAGE_WIDTH;

	uint64_t _p2=p[off/PAGE_P2_SIZE];
	uint64_t* p2=PAGE(_p2);

	if((_p2&PAGE_PRESENT)==0)return NULL;

	void* final=(void*)(((uint64_t)p2)|(off%PAGE_P2_SIZE));
	return final;
}
