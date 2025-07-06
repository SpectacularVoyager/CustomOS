#include "usertask/Task.h"
#include <userspace/syscall.h>
#include "utils/utils.h"
#include <stdlib/string.h>

int write(int fd,char* buffer,unsigned int len){
	if(fd!=1)return -1;
	FORI(len){
		printf("%c",buffer[i]);
	}
	return len;
}
int read(int fd,char* buffer,unsigned int len){
	TASK* t=TaskCurrent();
	FILE_DESC* desc=&t->fd[fd];
	if(desc->used!=1)return 0;
	void* hex=FILE_GetBuffer(&desc->file);
	int bytes_read=MIN(len,FILE_GetRemaining(desc));

	memcpy(buffer,hex+desc->offset,bytes_read);
	desc->offset+=bytes_read;
	return bytes_read;
}
void exit(registers* r,int code){
	r->rip=(uint64_t)USER_PRIV_LOOP;
	TaskKill();
}
int execve(const char* path,char **argv,char **envp){

	EXT2_INODE elf;
	if(EXT2_GET_INODE_FROM_PATH(&elf,path)==1){
		char* hex=malloc(elf.size);
		EXT2_READFILE(&elf,hex,elf.size);
		ELF_FILE file;
		int s=ELF_PARSE(&file,hex,elf.size);
		if(s==1){
			USERMODE_EXEC_ELF(&file,argv,envp);
		}
	}
	return 0;
}
int fork(void){
	TASK* t=TaskCurrent();
	TASK* _new=malloc(sizeof(TASK));
	TaskDup(_new,t);
	_new->r->rax=0;
	TaskAddList(_new);
	return _new->id;
}
