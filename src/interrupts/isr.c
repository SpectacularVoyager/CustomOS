#include "isr.h"
#include "../stdlib/stdio.h"


void ISR_Handler() {
	printf("EXECPTION\n");
    __asm__ volatile ("cli; hlt"); // Completely hangs the computer
}
