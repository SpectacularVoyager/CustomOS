#include "Scheduler.h"
#include "devices/apic/timer.h"
#include "drivers/apic/apic.h"
#include "utils/list/da_array.h"
#include "utils/utils.h"

extern void USER_PRIV_LOOP();

Processes globalProcs={0,0,0,0,1,0};

void ProcessSetHollow(Process* proc){
	proc->id=globalProcs.id++;
}

Process* getProcess(){
	return globalProcs.current;
}

void ProcessRun(Process* p){
	globalProcs.current=p;
	kprintf("RUNNING PROCESS:[%d]\n",p->id);
	// LOGVALD(p->r.rip);
	// LOGVALD(p->r.rsp);
	USER_JUMP_ASM((void*)100,(void*)p->r.rip,(void*)p->r.rsp);
}

void SchedulerStart(){
	IRQ_RegisterHandler(0,SchedulerInterrupt);
	APIC_PERIODIC(1000*1000*10);
}
void SchedulerInterrupt(registers *r){
	__asm__ volatile("CLI");
	if(globalProcs.count==0){
		r->rip=(uint64_t)USER_PRIV_LOOP;
		APIC_SEND_EOI();
		__asm__ volatile("STI");
		return;
	}
	if(globalProcs.i>=globalProcs.count){
		globalProcs.i=0;
	}
	Process* p=&globalProcs.items[globalProcs.i];
	printf("%d is ready\n",p->id);
	globalProcs.i++;

	APIC_SEND_EOI();
	__asm__ volatile("STI");
	ProcessRun(p);
}
int SchedulerSubmit(Process* p){
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

