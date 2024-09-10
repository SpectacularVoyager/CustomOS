#include "../stdlib/stdio.h"
#include "../utils/bit.h"
#include "fpu.h"

void FPUEnable(){
	unsigned long cr0;
	unsigned long cr4;
    asm volatile ("mov %%cr0, %0": "=r" (cr0) :: "memory");
	FLAG_SET(cr0,CR0_MP);
	FLAG_RESET(cr0,CR0_EM);
	FLAG_SET(cr0,CR0_NE);
	FLAG_RESET(cr0,CR0_TS);

	asm volatile ("mov %0, %%cr0":: "r" (cr0): "memory");

    asm volatile ("mov %%cr4, %0": "=r" (cr4) :: "memory");
	FLAG_SET(cr4,CR0_OSFXSR);
	FLAG_SET(cr4,CR0_OSXMMEXCPT);
	asm volatile ("mov %0, %%cr4":: "r" (cr4): "memory");
	
}
