#include "Scheduler.h"
#include "devices/apic/timer.h"
#include "drivers/apic/apic.h"
#include "interrupts/idt.h"
#include "stdlib/stdio.h"
#include "utils/list/da_array.h"
#include "utils/utils.h"
#include "paging/paging.h"



//REQUIRES MEMORY TO BE SET UP
int ProcessDup(Process* n,Process* old){
	unsigned int pages=1;
	ProcessSetHollow(n);
	ProcessSetUpPages(n);
	memcpy(n->memory.base,old->memory.base,pages*PAGE_P2_SIZE);
	memcpy(n->memory.stackBase,old->memory.stackBase,PAGE_P2_SIZE);
	memcpy(&n->r,&old->r,sizeof(registers));

	//DUP FD
	FORI(256){
		if(FILE_DESC_DUP(&n->fd[i],&old->fd[i])==0){
			return 0;
		}
	}
	return 1;
}
void ProcessRemap(Process* p){
	unsigned int pages=1;
	void* base=p->memory.base;
	void* stack=p->memory.stackBase;
	if(pages>2){
		//MIGHT INTERSECT WITH KERNEL
		printf("CANNOT ALLOCATED (%d) PAGES\n",pages);
		while(1);
	}
	//MemoryRemap(0x400000,(uint64_t)base,0b111);
	MemoryRemap(0x400000,(uint64_t)base,0b111);
	FORI(pages){
		//MemoryRemap(0x400000+PAGE_P2_SIZE*i,(uint64_t)base+i*PAGE_P2_SIZE,0b111);
	}
	//MemoryRemap((uint64_t)p->memory.stackBase,(uint64_t)p->memory.stackBase,0b111);
	MemoryRemap(0x800000,(uint64_t)stack,0b111);
}
//SET UP ENTRY MANUALLY
void ProcessSetUpPages(Process* p){

	//unsigned int pages=CEILDIV(elf->len, PAGE_P2_SIZE);
	unsigned int pages=1;
	void* base=PageAllocateN(pages);
	void* stack=PageAllocateN(1);

	p->memory.base=base;
	p->memory.stackTop=stack+PAGE_P2_SIZE-0x10;
	p->memory.stackBase=stack;
	ProcessRemap(p);
}

void ProcessInitialise(Process* proc,char** args,char** env){
	proc->r.rip=(uint64_t)proc->memory.entry;
	proc->r.rsp=(uint64_t)0x800000;
	proc->r.rbp=(uint64_t)0x800000;
	int argc=0;
	char** temp=args;
	while(temp!=0){
		temp++;
		argc++;
	}
	proc->r.rdi=argc;
	proc->r.rsi=(uint64_t)args;
}
