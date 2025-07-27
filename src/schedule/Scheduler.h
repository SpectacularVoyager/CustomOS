#pragma once
#include "interrupts/idt.h"
#include "interrupts/isr.h"
#include "specifications/elf/elf.h"
#include "stdlib/stdio.h"
#include "core/user.h"

typedef struct{
	void* entry;
	void* base;
	void* stackTop;
}Memory;

typedef struct{
	int id;
	Memory memory;
	registers r;
} Process;

typedef struct{
	Process *items;
	unsigned int count;
	unsigned int capacity;
	unsigned int i;
	unsigned int id;
	Process* current;
} Processes;

void ProcessSetHollow(Process* proc);

void SchedulerStart();

int SchedulerSubmit(Process* p);

int ProcessFromELF(ELF_FILE* elf,Process* process);

void SchedulerInterrupt(registers* r);

int ProcessReady(Process* process);

void ProcessRun(Process* process);

void ProcessInitialise(Process* proc,char** args,char** env);

Process* getProcess();
