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
	char* address=PageAllocateN(CEILDIV(elf->len, PAGE_P2_SIZE));

	ELF_Symbol* _start=ELF_LookUpSymbol(elf,"_start");
	if(_start==NULL){
		return 0;
	}
	uint64_t _start_addr=elf->sections[_start->SectionTableIndex].Offset;
	kprintf("%p %p %p\n",MemoryPhysical(address),elf->file,elf->len);
	memcpy(address,elf->file,elf->len);

	
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
	unsigned int pages=CEILDIV(elf->len, PAGE_P2_SIZE);
	void* base=PageAllocateN(pages);
	void* stack=PageAllocateN(1);
	if(pages>2){
		//MIGHT INTERSECT WITH KERNEL
		printf("CANNOT ALLOCATED (%d) PAGES\n",pages);
		while(1);
	}
	FORI(pages){
		MemoryRemap(0x400000+PAGE_P2_SIZE*i,(uint64_t)base+i*PAGE_P2_SIZE,0b111);
	}
	MemoryRemap(0x800000,(uint64_t)address+0x400000,0b111);

	// void* stack=(void*)0x40000000;
	KLOGVALD(base);

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
	ProcessSetHollow(process);
	//void* base=PageAllocateN(CEILDIV(elf->len, PAGE_P2_SIZE));
	// void* stack=PageAllocateN(1);
	// MemoryRemap(0x600000,(uint64_t)(stack),0b111);
	// MemoryRemap(0x600000,(uint64_t)base+0x200000,0b111);
	// MemoryRemap(0x800000,(uint64_t)base+0x400000,0b111);
	process->memory.base=base;
	process->memory.stackTop=stack+PAGE_P2_SIZE-0x10;
	process->memory.entry=process->memory.base+_start_addr;

	// LOGVALD(process->memory.base);
	// LOGVALD(process->memory.stackTop);
	// LOGVALD(process->memory.entry);

	return 1;
}
