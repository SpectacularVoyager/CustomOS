#include "interrupts/idt.h"
void syscall(registers* r);

#define SYSCALL_EXIT	60

extern void TEST_SYSCALL();
