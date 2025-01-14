#include "user.h"
#include <stdlib/stdio.h>
#include <stdlib/stdio.h>

void TSS_load(int r) {
    __asm__ volatile (
        "ltr %0"  // Load Task Register with the value in 'selector'
        :  // No output operands
        : "r" (r)  // Input operand: 'selector'
        : "memory"  // Inform the compiler that memory is affected
    );
}
void func(){
	SetColor(0xDE3163);
	printf("ENTERED USER MODE\n");
	printf("FIRST USERMODE PROGRAM\n");
	__asm__ __volatile__("int $0x80");
	while(1);
}
void USERMODE_ENTER(){
	printf("TRYING TO ENTER USER MODE\n");
	USER_JUMP_ASM(func);
}
