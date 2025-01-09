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
	while(1);
}
void USERMODE_ENTER(){
	printf("TRYING TO ENTER USER MODE\n");
	USER_JUMP_ASM(func);
}
