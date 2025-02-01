#include "syscall.h"
#include "interrupts/idt.h"
#include "stdlib/stdio.h"
#include <utils/utils.h>


void TaskKill();
#define ARG1(r) r->rdi
#define ARG2(r) r->rsi
#define ARG3(r) r->rdx
#define RETURN(r) r->rax=

void exit(int code);
int write(int fd,char* buffer,int len);

void syscall(registers* r){
	int ret=0;
	switch(r->rax){
		case SYSCALL_EXIT:
			exit(ARG1(r));
			break;
		case SYSCALL_WRITE:
			ret=write(ARG1(r),(void*)ARG2(r),ARG3(r));
			break;
		default:
			printf("UNRECOGNISED SYSCALL [0x%x]\n",r->rax);
	}
	RETURN(r) ret;
}
int write(int fd,char* buffer,int len){
	if(fd!=1)return -1;
	printf(buffer);
	return len;
}
void exit(int code){
	printf("EXITING WITH CODE[%x]\n",code);
	TaskKill();
}
