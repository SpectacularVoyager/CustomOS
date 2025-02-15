#include "Task.h"
#include "drivers/apic/apic.h"
#include "interrupts/idt.h"
#include "interrupts/irq.h"
#include "utils/list/list.h"
#include "utils/list/vector.h"
#include "utils/utils.h"
#include "core/user.h"
#include "stdlib/stdlib.h"
#include "stdlib/string.h"
#include "devices/apic/timer.h"
#include "stdlib/stdio.h"
#include "paging/paging.h"

ListNode* tasks=0;
ListNode* current=0;
uint32_t PID=0;

void Scheduler_LOOP_ROUND_ROBIN(registers* r);
void Scheduler_START(){
	IRQ_RegisterHandler(0,Scheduler_LOOP_ROUND_ROBIN);
	APIC_PERIODIC(1000*1000*100);
}
void TaskChange(TASK* t){
	MemoryRemap(0x400000,(uint64_t)t->address,0b111);
	USER_JUMP_ASM(0,(void*)t->r->rip,(void*)t->r->rsp);
}
TASK* TaskCurrent(){return current->val;}

TASK* TaskCreate(void* args,void* address,void* stack){
	TASK* t=malloc(sizeof(TASK));
	t->id=++PID;
	t->ready=1;
	memset(t->r,0,sizeof(registers));
	t->r->rip=(uint64_t)address;
	t->r->rsp=(uint64_t)stack;
	tasks=ListAdd(tasks,t);
	FORI(256){
		t->fd[i]=(FILE_DESC){.used=0};
	}
	t->address=address;
	return t;
}
void EMPTYLOOP(){
	while(1);
}
void TaskKill(){
	//EMPTYLOOP();
	ListNode* temp=current;
	PageDealloc(((TASK*)temp->val)->address);
	int _tasklen=ListLength(tasks);
	if(_tasklen==0){
		kprintf(INFO "UNEXPECTED NO TASK FOUND\n");
		return;
	}else if(_tasklen==1){
		tasks=ListRemove(tasks,temp);
		printf("AFTER REMOVING LIST_LEN[%d]",ListLength(tasks));
	}else{
		if(current->next==0){
			current=tasks;
		}else{
			current=current->next;
		}
		tasks=ListRemove(tasks,temp);
		TaskChange((TASK*)(current->val));
	}
}
void Scheduler_LOOP_ROUND_ROBIN(registers* r){
	kprintf("TICK\n");
	int _tasklen=ListLength(tasks);
	if(_tasklen==0){
		kprintf(INFO "NO TASKS FOUND IDLING\n");
		return;
	}
	if(_tasklen<=1){
		kprintf(INFO "CONTINUING EXISTING TASK\n",ListLength(tasks));
		//NO SWITCHING NEEDED
		if(current==0){
			current=tasks;
			registers* cur=((TASK*)(current->val))->r;
			APIC_SEND_EOI();
			TaskChange((TASK*)(current->val));
		}
		return;
	}

	((TASK*)(current->val))->r=r;
	if(current->next==0){
		current=tasks;
	}else{
		current=current->next;
	}
	registers* cur=((TASK*)(current->val))->r;
	//LOAD CONTEXT (current->r)
	//JUMP TO USER MODE
	APIC_SEND_EOI();
	TaskChange((TASK*)(current->val));
}
