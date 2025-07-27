#include "Scheduler.h"
#include "devices/apic/timer.h"
#include "drivers/apic/apic.h"
#include "interrupts/idt.h"
#include "utils/list/da_array.h"
#include "utils/utils.h"

extern void USER_PRIV_LOOP();

Processes globalProcs={0,0,0,0,1,0};

void ProcessSetHollow(Process* proc){
	proc->id=globalProcs.id++;
	proc->ready=1;
}

void ProcessKillCurrent(){
	nob_da_remove_element(&globalProcs, globalProcs.i);
	globalProcs.current=0;
}
Process* getProcess(){
	return globalProcs.current;
}

void ProcessRun(Process* p){
	globalProcs.current=p;
	kprintf("RUNNING PROCESS:[%d]\n",p->id);
	USER_JUMP_ASM((void*)100,(void*)p->r.rip,(void*)p->r.rsp);
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

