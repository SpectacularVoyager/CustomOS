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

char* trim(char* str){
	while(str[0]==' '||str[0]=='\t'){
		if(str[0]==0)break;
		str++;
	}
	return str;
}
int findfirstch(char* str,char c){
	int i=0;
	while(str[i]){
		if(str[i]==c||str[i]=='\0'){
			break;
		}
		i++;
	}
	return i;
}

char* KSHELL_NEXT_ARG(char* str,char* dest){
	str=trim(str);
	if(str[0]=='\0')return 0;
	int len=findfirstch(str, ' ');
	memcpy(dest,str,len);
	dest[len]=0x0;
	return str+len;
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
	if(trim(buffer)[0]=='\0')return;
	char inst[100];
	buffer=KSHELL_NEXT_ARG(buffer,inst);
	if(strncmp(inst,"ls",2)==0){

	}else if(strncmp(inst,"exec",4)==0){
		EXT2_INODE elf;
		char file[100];
		buffer=KSHELL_NEXT_ARG(buffer,file);
		if(EXT2_GET_INODE_FROM_PATH(&elf,file)==1){
			char* hex=malloc(elf.size);
			EXT2_READFILE(&elf,hex,elf.size);
			ELF_FILE elffile;
			int s=ELF_PARSE(&elffile,hex,elf.size);
			if(s==1){
				char** args=malloc(sizeof(char*)*10);
				args[0]=strdup(file);
				USERMODE_EXEC_ELF(&elffile,args,NULL);
			}else{

				printf("FILE [%s] NOT EXECUTABLE\n",file);
			}
		}else{
			printf("FILE [%s] NOT READABLE\n",file);
		}
	}else if(strncmp(inst,"cat",3)==0){
		char file[1000];
		KSHELL_NEXT_ARG(buffer,file);
		EXT2_INODE elf;
		int inode;
		if((inode=EXT2_GET_INODE_FROM_PATH(&elf,file))==1){
			char* hex=malloc(elf.size);
			EXT2_READFILE(&elf,hex,elf.size);
			printf(hex);
		}
	}else{
		EXT2_INODE elf;
		if(EXT2_GET_INODE_FROM_PATH(&elf,inst)==1){
			char* hex=malloc(elf.size);
			EXT2_READFILE(&elf,hex,elf.size);
			ELF_FILE elffile;
			int s=ELF_PARSE(&elffile,hex,elf.size);
			if(s==1){
				char** args=malloc(sizeof(char*)*10);
				memset(args,0,sizeof(char*)*10);

				args[0]=strdup(inst);
				int i=0;
				for(i=1;i<9;i++){
					char file[1000];
					if(buffer[0]=='\0')break;
					buffer=trim(KSHELL_NEXT_ARG(buffer,file));
					args[i]=strdup(file);
				}
				args[i]=0;
				USERMODE_EXEC_ELF(&elffile,args,NULL);
			}else{
				printf("FILE [%s] NOT EXECUTABLE\n",inst);
			}
		}else{
			printf("COMMAND [%s] NOT FOUND\n",inst);
		}
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
