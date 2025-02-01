#include "syscall.h"
#include "interrupts/idt.h"
#include "stdlib/stdio.h"
#include "stdlib/stdlib.h"
#include <utils/utils.h>
#include <drivers/ext2/ext2.h>
#include <usertask/Task.h>

#define ARG1(r) r->rdi
#define ARG2(r) r->rsi
#define ARG3(r) r->rdx
#define RETURN(r) r->rax=


void syscall(registers* r){
	int ret=0;
	switch(r->rax){
		case SYSCALL_EXIT:
			exit(ARG1(r));
			break;
		case SYSCALL_WRITE:
			ret=write(ARG1(r),(void*)ARG2(r),ARG3(r));
			break;
		case SYSCALL_OPEN:
			ret=open((void*)ARG1(r),ARG2(r),ARG3(r));
			break;
		case SYSCALL_READ:
			ret=read(ARG1(r),(void*)ARG2(r),ARG3(r));
			break;
		default:
			printf("UNRECOGNISED SYSCALL [0x%x]\n",r->rax);
	}
	RETURN(r) ret;
}
int write(int fd,char* buffer,unsigned int len){
	// printf("WRITE[%p]\n",buffer);
	if(fd!=1)return -1;
	FORI(len){
		printf("%c",buffer[i]);
	}
	return len;
}

int open(const char* path,int flags,umode_t mode){
	// printf("OPEN [%s]\n",path);
	TASK* t=TaskCurrent();
	int idx=-1;
	FILE_DESC* emptyfd;
	for(int i=3;i<256;i++){
		if(t->fd[i].used!=1){
			emptyfd=&t->fd[i];
			idx=i;
			break;
		}
	}
	if(idx==-1)return 0;
	if(EXT2_GET_INODE_FROM_PATH(&emptyfd->inode,path)==1){
		char* hex=malloc(emptyfd->inode.size);
		EXT2_READFILE(&emptyfd->inode,hex,emptyfd->inode.size);
		emptyfd->used=1;
		emptyfd->buffer=hex;
		emptyfd->offset=0;
		return idx;
	}else{
		printf("FILE :%s NOT FOUND",path);
	}
	return 0;
}
int read(int fd,char* buffer,unsigned int len){
	// printf("READ [%p] AT [%x]\n",buffer,fd);
	TASK* t=TaskCurrent();
	FILE_DESC* desc=&t->fd[fd];
	if(desc->used!=1)return 0;
	unsigned int idx=0;
	FORI(len){
		if(desc->offset>=desc->inode.size)break;
		buffer[i]=desc->buffer[desc->offset++];
		idx++;
	}
	return idx;
}
void exit(int code){
	printf("EXITING WITH CODE[%x]\n",code);
	TaskKill();
}
