#pragma once
#include "interrupts/idt.h"
#include "interrupts/isr.h"
#include "drivers/ext2/ext2.h"
#include "filesystem/vfs.h"

extern void USER_PRIV_LOOP();

void TaskSwitch(int id);


typedef struct {
	int id;
	int ready;
	registers* r;
	FILE_DESC fd[256];
	void* address;
} TASK;

TASK* TaskCreate(char** args,void* address,void* stack);

void Scheduler_START();

void TaskKill();

TASK* TaskCurrent();

int TaskDup(TASK* n,TASK* old);

void TaskAddList(TASK* task);
