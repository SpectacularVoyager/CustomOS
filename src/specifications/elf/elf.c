#include "elf.h"
#include <stdlib/string.h>
#include "utils/utils.h"

char ELF_MAGIC[4]={0x7F,'E','L','F'};

char* errno_ELF(int x){
#define ERR_MSG(e)	if(x==e)return (#e);
	ERR_MSG(ERROR_ELF_NO_MAGIC);
	ERR_MSG(ERROR_ELF_UNSUPPORTED_ABI);
	ERR_MSG(ERROR_ELF_UNSUPPORTED_ARCH);
	ERR_MSG(ERROR_ELF_NO_STRING_TABLE);
	ERR_MSG(ERROR_ELF_NO_SYMBOL_TABLE);
	ERR_MSG(ERROR_ELF_UNSUPPORTED_TYPE);
#undef ERR_MSG
	return "MESSAGE NOT FOUND";
}


void* ELF_LookUpSection(ELF_FILE* elf,char* sectionName){
	void* nameTable=elf->file+elf->name->Offset;
	FORI(elf->header->SectionHeaderCount){
		ELF_SectionHeader* section=&elf->sections[i];
		char* name=&nameTable[section->NameOffset];
		if(strncmp(name,sectionName,strlen(sectionName))==0){
			return section;
		}
	}
	return NULL;
}
void* ELF_LookUpSymbol(ELF_FILE* elf,char* sectionName){
	ELF_Symbol* symbols=elf->file+elf->symbolTable->Offset;
	char* strings=elf->file+elf->stringTable->Offset;
	FORI(elf->symbolTable->Size/elf->symbolTable->EntrySize){
		ELF_Symbol* symbol=&symbols[i];
		char* name=&strings[symbol->Name];
		if(strncmp(name,sectionName,strlen(sectionName))==0){
			return symbol;
		}
	}
	return NULL;
}
// #define LOOKUP(table,index) &table[index]
int ELF_PARSE(ELF_FILE* elf,void* file,int len){
	elf->header=file;
	elf->file=file;
	elf->len=len;
	if(strncmp((char*)elf->header->Ident.Magic,ELF_MAGIC,4)!=0){
		return ERROR_ELF_NO_MAGIC;
	}
	if(elf->header->Ident.Class!=ELF_CLASS_64){
		return ERROR_ELF_UNSUPPORTED_ARCH;
	}
	if(elf->header->Ident.OsAbi!=ELF_OSABI_SYSV){
		return ERROR_ELF_UNSUPPORTED_ABI;
	}
	//TODO: DO SOMETING WITH TYPE
	switch(elf->header->Type){
		case ELF_TYPE_NONE:
			return ERROR_ELF_UNSUPPORTED_TYPE;
			break;
		case ELF_TYPE_EXEC:
			break;
		default:
			return ERROR_ELF_UNSUPPORTED_TYPE;
	}
	elf->sections=elf->file+elf->header->SectionHeaderOffset;
	elf->name=&elf->sections[elf->header->SectionNameTableIndex];
	elf->symbolTable=ELF_LookUpSection(elf,".symtab");
	elf->stringTable=ELF_LookUpSection(elf,".strtab");
	elf->rela		=ELF_LookUpSection(elf,".rela");
	if(elf->symbolTable==NULL){
		return ERROR_ELF_NO_SYMBOL_TABLE;
	}
	if(elf->stringTable==NULL){
		return ERROR_ELF_NO_STRING_TABLE;
	}
	// if(elf->rela==0){
	// 	printf("ONLY SUPPORT RELOCATABLE FILES\n");
	// 	return 0;
	// }
	return 1;
}
