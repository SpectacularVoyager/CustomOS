#include "Task.h"
#include "drivers/apic/apic.h"
#include "interrupts/idt.h"
#include "interrupts/irq.h"
#include "utils/list/list.h"
#include "utils/list/vector.h"
#include "core/user.h"
#include "stdlib/stdlib.h"
#include "stdlib/string.h"
#include "devices/apic/timer.h"
#include "stdlib/stdio.h"

ListNode* tasks=0;
ListNode* current=0;
uint32_t PID=0;

void Scheduler_LOOP_ROUND_ROBIN(registers* r);
void Scheduler_START(){
	printf("HEY\n");
	IRQ_RegisterHandler(0,Scheduler_LOOP_ROUND_ROBIN);
	APIC_PERIODIC(1000*1000);
}
void TaskContextSwitch(registers* r){
	USER_JUMP_ASM(0,(void*)r->rip,(void*)r->rsp);
}

TASK* TaskCreate(void* args,void* address,void* stack){
	TASK* t=malloc(sizeof(TASK));
	t->id=++PID;
	t->ready=1;
	memset(t->r,0,sizeof(registers));
	t->r->rip=(uint64_t)address;
	t->r->rsp=(uint64_t)stack;
	ListAdd(tasks,t);
	return t;
}
void Scheduler_LOOP_ROUND_ROBIN(registers* r){
	kprintf("TICK\n");
	printRegs(r);
	//SAVE CONTEXT R
	((TASK*)(current->val))->r=r;

	if(ListLength(tasks)<=1){
		//NO SWITCHING NEEDED
		return;
	}
	if(current->next==0){
		current=tasks;
	}else{
		current=current->next;
	}

	registers* cur=((TASK*)(current->val))->r;
	//LOAD CONTEXT (current->r)
	//JUMP TO USER MODE
	TaskContextSwitch(cur);
	APIC_SEND_EOI();
}
