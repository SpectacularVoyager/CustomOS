#include "kshell.h"
#include "printf/printf.h"
#include "stdlib/stdio.h"
#include "utils/utils.h"
#include "stdlib/string.h"
#include "vga/term.h"
#include "graphics/graphics.h"
#include "drivers/ext2/ext2.h"
#include "stdlib/stdlib.h"
#include "graphics/graphicsterm.h"
#include "core/user.h"
#include "arch/GDT.h"
#include "usertask/Task.h"
#include "drivers/ext2/ext2.h"
#include "specifications/elf/elf.h"

#define KSHELL_MAX_BUFFER_SIZE 1024

char kshell_buffer[KSHELL_MAX_BUFFER_SIZE];
int kshell_ptr=0;
int kshell_init=0;

char* KSHELL_NEXT_ARG(char* str){
	return strchrnul(str,' ');
}

void prompt(){
	SetColor(0x00FF00);
	printf("kshell>");
	SetColor(0xFFFFFF);
}
void INIT(){
	if(!kshell_init){
		TERM_SET_POS(0, 0);
		ClearScreen();
		prompt();
		kshell_init=1;
	}
}
void evaluate(char* buffer){
	if(strncmp(buffer,"ls",2)==0){
		char* next=KSHELL_NEXT_ARG(buffer)+1;
		volatile char* path="/home";
		if(!(next[0]=='\0'||next[0]==' ')){
			printf("%s\n",next);
			path=next;
			hexdump(next,10,10);
		};
		EXT2_INODE elf;
		int inode;
		if((inode=EXT2_GET_INODE_FROM_PATH(&elf,path))==1){
			printf("IN");
			EXT2_LS(&elf);
			//EXT2_FIND_IN_DIR(&elf,"/");
		}

	}else if(strncmp(buffer,"exec",4)==0){
		EXT2_INODE elf;
		char* next=KSHELL_NEXT_ARG(buffer)+1;
		volatile char* path="/home/ASM/main.c";
		if(!(next[0]=='\0'||next[0]==' ')){
			path=next;
		};
		if(EXT2_GET_INODE_FROM_PATH(&elf,"/home/ASM/a.out")==1){
			char* hex=malloc(elf.size);
			EXT2_READFILE(&elf,hex,elf.size);
			ELF_FILE file;
			int s=ELF_PARSE(&file,hex,elf.size);
			if(s==1){
				USERMODE_EXEC_ELF(&file);
			}
		}
	}else if(strncmp(buffer,"cat",3)==0){
		EXT2_INODE elf;
		int inode;
		char* next=KSHELL_NEXT_ARG(buffer)+1;
		volatile char* path="/home/ASM/main.c";
		if(!(next[0]=='\0'||next[0]==' ')){
			path=next;
		};
		if((inode=EXT2_GET_INODE_FROM_PATH(&elf,path))==1){
			FORI(4){
				printf("%p\t",elf.blockPointers[i]);
			}printf("\n");
			char* hex=malloc(elf.size);
			EXT2_READFILE(&elf,hex,elf.size);
			printf(hex);
		}
	}else{
		printf("COMMAND \"%s\" NOT FOUND:\n",buffer);
	}
}

void __getch(char x){
	INIT();
	if(x=='\b'){
		if(kshell_ptr>0){
			printf("%c",x);
			kshell_ptr--;
		}
	}else if(x=='\0'){

	}else if(x=='\n'){
		printf("%c",x);
		char buffer[kshell_ptr+1];
		memcpy(buffer,kshell_buffer,kshell_ptr);
		buffer[kshell_ptr+1]=0;
		evaluate(buffer);
		kshell_ptr=0;
		prompt();
	}else{
		printf("%c",x);
		kshell_buffer[kshell_ptr]=x;
		kshell_ptr=MIN(kshell_ptr+1,KSHELL_MAX_BUFFER_SIZE);
	}
	//_putCursor();
}
