#include "schedule/Scheduler.h"
#include "stdlib/stdlib.h"
#include "stdlib/string.h"
#include "paging/paging.h"
#include "utils/utils.h"
#include "utils/bit.h"

int ProcessFromFilePath(char* fp,EXT2_INODE* elf,Process* process){
	if(EXT2_GET_INODE_FROM_PATH(elf,fp)!=0){
		char* hex=malloc(elf->size);
		EXT2_READFILE(elf,hex,elf->size);
		ELF_FILE file;
		int s=ELF_PARSE(&file,hex,elf->size);
		if(s==1){
			Process proc;
			int val=ProcessFromELF(&file,&proc);
			if(val==0){
				printf("CANNOT EXECUTE ELF %s\n",fp);
				return 0;
			}else{
				ProcessInitialise(&proc,NULL,NULL);
				return 1;
			}
		}else{
			printf("%s -> FILE NOT EXECUTABLE [%s]\n",fp,errno_ELF(s));
			return 0;
		}
	}
	return 1;
}

int ProcessFromELF(ELF_FILE* elf,Process* process){

	//printf("STARTING PROCESS %s\n");
	//char* address=PageAllocateN(CEILDIV(elf->len, PAGE_P2_SIZE));

	ProcessSetHollow(process);
	ProcessSetUpPages(process);
	void* base=process->memory.base;
	ELF_Symbol* _start=ELF_LookUpSymbol(elf,"_start");
	if(_start==NULL){
		return 0;
	}
	uint64_t _start_addr=elf->sections[_start->SectionTableIndex].Offset;

	process->memory.entry=(void*)0x400000+_start_addr;
	memcpy(base,elf->file,elf->len);

	
	// LOGVAL(elf->header->Entry);
	void* nameTable=elf->file+elf->name->Offset;
	int flag=0;
	FORI(elf->header->SectionHeaderCount){
		ELF_SectionHeader* section=&elf->sections[i];
		char* name=&nameTable[section->NameOffset];
		if(section->Addr!=0){
			if(section->Addr/0x200000==2){
			}else{
				flag=1;
			}
		}
		// printf("%s\t[%x -> %x] [%x->%x]\n",name,section->Offset,section->Offset+section->Size,section->Addr,section->Addr+section->Size);
	}
	if(flag==1){
		printf("CAN ONLY EXECUTE Processes mapped at 0x400000\n");
		return 0;
	}

	FORI(elf->header->SectionHeaderCount){
		ELF_SectionHeader* section=&elf->sections[i];
		char* name=&nameTable[section->NameOffset];
		if(section->Addr!=0){
			memcpy(ADDR(section->Addr),elf->file+section->Offset,section->Size);
		}
	}
	if(elf->rela!=0){
		printf("RELA\t[%x %x]\n",elf->rela->Offset,elf->rela->Addend);
	}
	TRACK

	return 1;
}
