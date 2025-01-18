#include "interrupts/idt.h"
#include "interrupts/isr.h"

void TaskSwitch(int id);


typedef struct {
	int id;
	int ready;
	registers* r;
} TASK;

TASK* TaskCreate(void* args,void* address,void* stack);

void Scheduler_START();
