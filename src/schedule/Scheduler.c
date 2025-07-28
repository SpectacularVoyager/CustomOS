#include "Scheduler.h"
#include "devices/apic/timer.h"
#include "drivers/apic/apic.h"
#include "interrupts/idt.h"
#include "utils/list/da_array.h"
#include "utils/utils.h"
#include "paging/paging.h"

extern void USER_PRIV_LOOP();

Processes globalProcs={0,0,0,0,1,0};

void ProcessSetHollow(Process* proc){
	proc->id=globalProcs.id++;
	proc->ready=1;
}

void ProcessKillCurrent(){
	kprintf("KILLING PROCESS:[%d]\n",getProcess()->id);
	nob_da_remove_element(&globalProcs, globalProcs.i);
	globalProcs.current=0;
}
Process* getProcess(){
	return globalProcs.current;
}

void ProcessRun(Process* p){
	ProcessRemap(p);
	globalProcs.current=p;
	kprintf("RUNNING PROCESS:[%d]\n",p->id);
	kprintf("\tADDRESS:%p\n",p->r.rip);
	USER_JUMP_ASM(&p->r,(void*)p->r.rip,(void*)p->r.rsp);
}

void SchedulerStart(){
	IRQ_RegisterHandler(0,SchedulerInterrupt);
	APIC_PERIODIC(1000*1000*10);
}
void UserSpaceDoNothing(registers* r){
	if(r->rip!=(uint64_t)USER_PRIV_LOOP){
		kprintf("DOING NOTHING\n");
	}
	r->rip=(uint64_t)USER_PRIV_LOOP;
}
Process* TrySchedule(){
	globalProcs.i++;
	if(globalProcs.count==0){
		return NULL;
	}
	if(globalProcs.i>=globalProcs.count){
		globalProcs.i=0;
	}
	return &globalProcs.items[globalProcs.i];

}
void SchedulerInterrupt(registers *r){
	__asm__ volatile("CLI");
	Process* proc=TrySchedule();	
	APIC_SEND_EOI();
	__asm__ volatile("STI");
	if(proc==NULL){
		UserSpaceDoNothing(r);
	}else{
		ProcessRun(proc);
	}
}
int SchedulerSubmit(Process* p){
	kprintf("SUBMITTED PROCESS:\t%d\n",p->id);
	nob_da_append(&globalProcs,*p);
	return 1;
}

void ProcessInitialise(Process* proc,char** args,char** env){
	proc->r.rip=(uint64_t)proc->memory.entry;
	proc->r.rsp=(uint64_t)proc->memory.stackTop;
	proc->r.rbp=(uint64_t)proc->memory.stackTop;
	int argc=0;
	char** temp=args;
	while(temp!=0){
		temp++;
		argc++;
	}
	proc->r.rdi=argc;
	proc->r.rsi=(uint64_t)args;
}
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
			//continue;
		}
	}
	return 1;
}
void ProcessRemap(Process* p){
	unsigned int pages=1;
	void* base=p->memory.base;
	if(pages>2){
		//MIGHT INTERSECT WITH KERNEL
		printf("CANNOT ALLOCATED (%d) PAGES\n",pages);
		while(1);
	}
	FORI(pages){
		MemoryRemap(0x400000+PAGE_P2_SIZE*i,(uint64_t)base+i*PAGE_P2_SIZE,0b111);
	}
	MemoryRemap(0x800000,(uint64_t)base+0x400000,0b111);
}
//SET UP ENTRY MANUALLY
void ProcessSetUpPages(Process* p){

	//unsigned int pages=CEILDIV(elf->len, PAGE_P2_SIZE);
	unsigned int pages=1;
	void* base=PageAllocateN(pages);
	void* stack=PageAllocateN(1);
	ProcessRemap(p);

	p->memory.base=base;
	p->memory.stackTop=stack+PAGE_P2_SIZE-0x10;
	p->memory.stackBase=stack;
}
