#include "syscall.h"
#include "stdlib/stdio.h"


void TaskKill();
#define ARGS_1(r) r->rdi

void exit(int code);

void syscall(registers* r){
	switch(r->rax){
		case SYSCALL_EXIT:
			exit(ARGS_1(r));
		default:
			printf("UNRECOGNISED SYSCALL [0x%x]\n",r->rax);
	}
}

void exit(int code){
	printf("EXITING WITH CODE[%x]\n",code);
	TaskKill();
}
