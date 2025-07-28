#include "interrupts/idt.h"
#include "usertask/Task.h"
#include <userspace/syscall.h>
#include "utils/utils.h"
#include <stdlib/string.h>
#include "CharacterDevice/CharacterDevice.h"
#include "schedule/Scheduler.h"

int write(int fd,char* buffer,unsigned int len){
	if(fd!=1)return -1;
	FORI(len){
		printf("%c",buffer[i]);
	}
	return len;
}
int read(int fd,char* buffer,unsigned int len){
	if(fd==0){
		//STDIN
		memcpy(buffer,"Hello\n World\n",13);
		return 13;
		//int x= CharacterDeviceRead(STDIO(),buffer,len);
		// if(x==0)return 0;
		// return x;
	}

	Process* proc=getProcess();
	FILE_DESC* desc=&proc->fd[fd];
	if(desc->used!=1)return 0;
	void* hex=FILE_GetBuffer(&desc->file);
	int bytes_read=MIN(len,FILE_GetRemaining(desc));

	memcpy(buffer,hex+desc->offset,bytes_read);
	desc->offset+=bytes_read;
	return bytes_read;
}
void exit(registers* r,int code){
	ProcessKillCurrent();
	Process* proc=TrySchedule();
	if(proc){
		ProcessRun(proc);
	}else{
		UserSpaceDoNothing(r);
	}
	// r->rip=(uint64_t)USER_PRIV_LOOP;
	// TaskKill();
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
int len=3;
int fork(registers* r){
	Process* proc=getProcess();
	Process n;
	int i=ProcessDup(&n,proc);
	if(i==0)return -1; 
	n.r.rax=0;
	ProcessRemap(proc);
	SchedulerSubmit(&n);
	return n.id;
}
