#include "user.h"
#include <stdlib/stdio.h>
#include <stdlib/stdlib.h>
#include <stdlib/string.h>
#include "paging/paging.h"
#include "utils/utils.h"
#include "utils/bit.h"
#include "usertask/Task.h"

extern void* stack_top_syscall;

void func(){
	int a;
	//printf("%p\n",100);
	//SetColor(0xDE3163);
	//printf("ENTERED USER MODE\n");
	//printf("FIRST USERMODE PROGRAM\n");
	//__asm__ __volatile__("int $0x80");
	FORI(1024)
		(((uint32_t*)0xFD000000))[i]=0xFF00FF;
	
	while(1);
}
void sayHello(){
	printf("HELLO\n");
}
extern void TEST_USER();
extern void TEST_HALT();
uint8_t testprogram[14]   = {0xBB,0x00,0x00,0x00,0xFD,0xC7,0x03,0xFF,0x00,0xFF,0x00,0xF4,0xB8,0x40};
void __attribute__((optimize("O0"))) USERMODE_ENTER(){
	AllocatePage(5, 0x40000000, 0b111);
	void* address=(void*)(0x140000000);
	printf("TRYING TO ENTER USER MODE\n");

	memcpy(address,TEST_HALT,100);

	USER_JUMP_ASM(NULL,address,address+0x100000);
}
void USERMODE_ADD(){
	AllocatePage(5, 0x40000000, 0b111);
	void* address=(void*)(0x140000000);
	printf("TRYING TO ENTER USER MODE\n");

	memcpy(address,TEST_HALT,100);
	TaskCreate(NULL,address,address+0x100000);
}
