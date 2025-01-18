#include "syscall.h"
#include "stdlib/stdio.h"


#define ARGS_1(r) r->rdi

void exit(int code);

void syscall(registers* r){
	printf("SYSCALL\n");
	switch(r->rax){
		case SYSCALL_EXIT:
			exit(ARGS_1(r));
		default:
			printf("UNRECOGNISED SYSCALL [0x%x]",r->rax);
	}
}

void exit(int code){
}
