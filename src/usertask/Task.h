#include "interrupts/idt.h"
#include "interrupts/isr.h"
#include "drivers/ext2/ext2.h"

extern void USER_PRIV_LOOP();

void TaskSwitch(int id);

typedef struct {
	char* buffer;
	EXT2_INODE inode;
	int used;
	int offset;
} FILE_DESC;

typedef struct {
	int id;
	int ready;
	registers* r;
	FILE_DESC fd[256];
	void* address;
} TASK;

TASK* TaskCreate(void* args,void* address,void* stack);

void Scheduler_START();

void TaskKill();

TASK* TaskCurrent();
