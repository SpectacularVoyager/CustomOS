#include "task.h"
#include "../stdlib/stdio.h"
#include "../stdlib/string.h"

void TaskInit(){
	Context* context=TaskGetContext();
	kprintf(INFO"ADDRESS RIP\t%p\n",context);
	kprintf(INFO"ADDRESS FUNC\t%p\n",TaskInit);
}
void TaskSwitch(Task* oldTask,Task* newTask){
	Context* current=TaskGetContext();
	memcpy(&oldTask->context,current,sizeof(Context));
	newTask->entry();
	TaskSetContext(&newTask->context);
}
//TODO: TEST
Task TaskCreate(uint32_t flags,void(*main),uint64_t pagedir,uint64_t stack){
	volatile Task t={};
	//t.context.r15	= 0;
	//t.context.r14 	= 0;
	//t.context.r13 	= 0;
	//t.context.r12 	= 0;
	//t.context.r11 	= 0;
	//t.context.r10 	= 0;
	//t.context.r9  	= 0;
	//t.context.r8  	= 0;
	//t.context.rbp 	= 0;
	//t.context.rdi 	= 0;
	//t.context.rsi 	= 0;
	//t.context.rdx 	= 0;
	//t.context.rcx 	= 0;
	//t.context.rbx 	= 0;
	//t.context.rax 	= 0;
	//t.context.rsp 	= stack;
	//t.context.rip	= (uint64_t)main;
	//t.context.flags	= flags;
	//t.context.cr3	= pagedir;
	//t.entry=main;
	kprintf("DONE\n");
	return t;
}
void TaskYield(){
	kprintf(ERROR "TODO YIELD\n");
}
