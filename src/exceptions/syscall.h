#include "interrupts/idt.h"
void syscall(registers* r);

#define SYSCALL_EXIT	60
#define SYSCALL_WRITE	1

extern void TEST_SYSCALL();
