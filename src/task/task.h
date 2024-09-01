#include "stdint.h"

typedef struct {
	uint64_t cr3  ;
	uint64_t flags;
	uint64_t rip  ;
	uint64_t rsp  ;
	uint64_t r15  ;
	uint64_t r14  ;
	uint64_t r13  ;
	uint64_t r12  ;
	uint64_t r11  ;
	uint64_t r10  ;
	uint64_t r9   ;
	uint64_t r8   ;
	uint64_t rbp  ;
	uint64_t rdi  ;
	uint64_t rsi  ;
	uint64_t rdx  ;
	uint64_t rcx  ;
	uint64_t rbx  ;
	uint64_t rax  ;
} __attribute__((packed)) Context;

typedef struct {
	int PID;
	Context context;
	const void(*entry)();
} Task;


void TaskSwitch(Task* oldTask,Task* newTask);

void TaskYield();

Task TaskCreate(uint32_t flags,void(*main),uint64_t pagedir,uint64_t stack);

void TaskInit();

Context* TaskGetContext();
extern void TaskSetContext(Context* context);
