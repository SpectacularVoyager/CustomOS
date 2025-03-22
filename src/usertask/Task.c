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
	APIC_PERIODIC(1000*1000*10);
}
int TaskNewPID(){
	return  ++PID;
}
int TaskDup(TASK* n,TASK* old){
	n->id=TaskNewPID();
	n->ready=1;
	memcpy(n->r,old->r,sizeof(registers));
	FORI(256){
		if(FILE_DESC_DUP(&n->fd[i],&old->fd[i])==0){
			return 0;
		}
	}

	return 1;
}
void TaskChange(TASK* t){
	MemoryRemap(0x400000,(uint64_t)t->address,0b111);
	LOGVALD(t->r->rsp);

	LOGVAL(t->id);
	LOGVAL(t->r->rdi);
	USER_JUMP_ASM(t->r,(void*)t->r->rip,(void*)t->r->rsp);
}
void JMP_PRIV_LOOP(){
	registers r;

	USER_JUMP_ASM(&r,USER_PRIV_LOOP,NULL);
}
TASK* TaskCurrent(){return current->val;}

int ARGS_LEN(char** args,int max){
	int i=0;
	for(;i<max;i++){
		if(args[i]==0)return i;
	}
	return i;
}
TASK* TaskCreate(char** args,void* address,void* stack){
	TASK* t=malloc(sizeof(TASK));
	t->id=TaskNewPID();
	t->ready=1;
	memset(t->r,0,sizeof(registers));
	t->r->rip=(uint64_t)address;
	t->r->rsp=(uint64_t)stack;


	printf("TASK CREATE\n");
	TaskAddList(t);
	FORI(256){
		t->fd[i]=(FILE_DESC){.used=0};
	}
	t->address=address;

	t->r->rsi=(uint64_t)args;
	t->r->rdi=(uint64_t)ARGS_LEN(args, 10);

	LOGVAL(t->id);
	LOGVAL(t->r->rdi);
	return t;
}
void TaskAddList(TASK* task){
	tasks=ListAdd(tasks,task);
}
void TasksPrint(){
	for(ListNode* node=tasks;node!=NULL;node=node->next){
		TASK* task=(TASK*)node->val;
		char c=(current==node)?'*':' ';
		printf("[TASK:%x][%x]%c -> ",task->id,task->ready,c);
	}printf("\n");

}
void EMPTYLOOP(){
	while(1);
}
ListNode* TaskNextFree(ListNode* start,ListNode* current){
	if(current==0)return start;
	ListNode* orig=current;
	for(;current->next;current=current->next){
		if(1)return current;
	}
	for(current=start;current!=orig;current=current->next){
		if(1)return current;
	}
	return NULL;
}
void TaskKill(){
	__asm__ volatile("CLI");
	//EMPTYLOOP();
	ListNode* temp=current;
	((TASK*)(current->val))->r->rip=(uint64_t)USER_PRIV_LOOP;
	PageDealloc(((TASK*)temp->val)->address);
	//
	// int _tasklen=ListLength(tasks);
	// if(_tasklen==0){
	// 	kprintf(INFO "UNEXPECTED NO TASK FOUND\n");
	// 	current=0;
	// 	return;
	// }else if(_tasklen==1){
	// 	tasks=ListRemove(tasks,temp);
	// 	current=0;
	// }else{
	// 	if(current->next==0){
	// 		current=tasks;
	// 	}else{
	// 		current=current->next;
	// 	}
	// 	tasks=ListRemove(tasks,temp);
	// 	TaskChange((TASK*)(current->val));
	// }
	//LOGVAL(ListLength(tasks));
	tasks=ListRemove(tasks,current);
	//current=TaskNextFree(tasks,current);
	current=NULL;
	//while(1);
	__asm__ volatile("STI");
}
void Scheduler_LOOP_ROUND_ROBIN(registers* r){
	__asm__ volatile("CLI");
	APIC_SEND_EOI();
	if((current=TaskNextFree(tasks,current))!=NULL){
		//TasksPrint();
		TASK* task=(TASK*)(current->val);
		printf("CURRENT:\t%d\n",task->id);
		TaskChange(task);
	}else{
		JMP_PRIV_LOOP();
	}
	__asm__ volatile("STI");
}
