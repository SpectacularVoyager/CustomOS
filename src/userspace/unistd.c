#include <userspace/syscall.h>

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
	unsigned int idx=0;
	FORI(len){
		if(desc->offset>=desc->inode.size)break;
		buffer[i]=desc->buffer[desc->offset++];
		idx++;
	}
	return idx;
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
