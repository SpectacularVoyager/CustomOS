#pragma once
#include "stdint.h"

#define ELF_CLASS_64	2
#define ELF_OSABI_SYSV	0

#define ERROR_ELF_NO_MAGIC				-1
#define ERROR_ELF_UNSUPPORTED_ABI		-2
#define ERROR_ELF_UNSUPPORTED_ARCH		-3
#define ERROR_ELF_NO_STRING_TABLE		-4
#define ERROR_ELF_NO_SYMBOL_TABLE		-5
#define ERROR_ELF_UNSUPPORTED_TYPE		-6

#define ELF_TYPE_NONE	0
#define ELF_TYPE_REL	1
#define ELF_TYPE_EXEC	2
#define ELF_TYPE_DYN	3

char* errno_ELF(int x);
typedef struct {
	uint8_t Magic[4];
	uint8_t Class;
	uint8_t Data;
	uint8_t Version;
	uint8_t OsAbi;
	uint8_t AbiVersion;
	uint8_t Padding[6];
	uint8_t SizeIdent;

}__attribute__((packed)) ELF_IDENT;
typedef struct {
	ELF_IDENT Ident;
	uint16_t Type;
	uint16_t Machine;
	uint32_t Version;
	uint64_t Entry;
	uint64_t HeaderOffset;
	uint64_t SectionHeaderOffset;
	uint32_t Flags;
	uint16_t HeaderSize;
	uint16_t ProgHeaderSize;
	uint16_t ProgramHeaderCount;
	uint16_t SectionHeaderSize;
	uint16_t SectionHeaderCount;
	uint16_t SectionNameTableIndex;
}__attribute__((packed)) ELF_Header;

typedef struct{
	uint32_t NameOffset;
	uint32_t Type;
	uint64_t Flags;
	uint64_t Addr;
	uint64_t Offset;
	uint64_t Size;
	uint32_t Link;
	uint32_t Info;
	uint64_t Align;
	uint64_t EntrySize;
}__attribute__((packed)) ELF_SectionHeader;

typedef struct{
	uint32_t Name;
	uint8_t Info;
	uint8_t Other;
	uint16_t SectionTableIndex;
	uint64_t Value;
	uint64_t Size;
}__attribute__((packed)) ELF_Symbol;

typedef struct {
	uint64_t Offset;
	uint64_t Info;
} __attribute__((packed)) ELF_Rel;

typedef struct {
	uint64_t Offset;
	uint64_t Info;
	uint64_t Addend;
} __attribute__((packed)) ELF_Rela;

typedef struct{
	void* file;
	int len;
	ELF_Header* header;
	ELF_SectionHeader* sections;
	ELF_SectionHeader* name;
	ELF_SectionHeader* symbolTable;
	ELF_SectionHeader* stringTable;
	ELF_Rela* rela;
	ELF_Symbol* symbols;
} ELF_FILE;
int ELF_PARSE(ELF_FILE* elf,void* file,int len);

void* ELF_LookUpSection(ELF_FILE* elf,char* sectionName);
void* ELF_LookUpSymbol(ELF_FILE* elf,char* sectionName);
