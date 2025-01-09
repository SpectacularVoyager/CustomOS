#include "stdint.h"
#include "utils/bit.h"

#define GDT_FLAG_GRAN	(1<<3)
#define GDT_FLAG_DB		(1<<2)
#define GDT_FLAG_LONG	(1<<1)

#define GDT_ACCESS_ACCESSED		(1<<0)
#define GDT_ACCESS_RW			(1<<1)	
#define GDT_ACCESS_DIR			(1<<2)	
#define GDT_ACCESS_EXEC			(1<<3)	
#define GDT_ACCESS_TYPE			(1<<4)	
#define GDT_ACCESS_PRIV(P)		((P)<<5)	//1 for code/data and 0 for tss
#define GDT_ACCESS_PRESENT		(1<<7)	


#define GDT_ENTRY(base,limit,access,flags){		\
	((limit)&0xFFFF),							\
	((base)&0xFFFF),							\
	((base>>16)&0xFF),							\
	((access)&0xFF),							\
	((flags&0xF0)|((limit>>16)&0x0F)),			\
	((base>>24)&0xFF),							\
	((base>>32)),								\
	0											\
}

typedef struct {
	uint16_t LimitLow;
	uint16_t BaseLow;
	uint8_t  BaseMiddle;
	uint8_t  Access;
	uint8_t  FlagsLimitHigh;
	uint8_t  BaseHigh;
	uint32_t BaseLong;
	uint32_t resv;

}__attribute__((packed)) GDTEntry;

typedef struct {
	uint16_t size;
	GDTEntry* ptr;
} GDTDescriptor;

void GDT_LoadEntry(GDTEntry* entry,long base,int limit,unsigned char access,unsigned char flags){
	entry->LimitLow			= ((limit)&0xFFFF);
	entry->BaseLow			= ((base)&0xFFFF);		
	entry->BaseMiddle		= ((base>>16)&0xFF);
	entry->Access			= ((access));
	entry->FlagsLimitHigh	= (((flags&0xF0)<<4)|((limit>>16)&0x0F));
	entry->BaseHigh			= ((base>>24)&0xFF);			
	entry->BaseLong			= ((base>>32));							
	entry->resv				= 0;								
}

GDTEntry GDT_Table[4] __attribute__((aligned(64)));
GDTDescriptor GDT_Descriptor;
extern GDTEntry* gdt64;

void loadGDT(){
	uint32_t* gdt=(uint32_t*)GDT_Table;
	gdt[0]=0;
	gdt[1]=0;
	gdt[2]=0;
	gdt[3]=0xA<<20|0x9A<<8;

	//GDTEntry* TSS=&GDT_Table[3];
	//GDT_LoadEntry(&GDT_Table[1],0,
	//	0,
	//	GDT_ACCESS_RW|GDT_ACCESS_EXEC|GDT_ACCESS_TYPE|0x80,
	//	GDT_FLAG_GRAN|GDT_FLAG_LONG);
	//GDT_Table[1]=(GDTEntry)GDT_ENTRY(0,
	//	0,
	//	GDT_ACCESS_RW|GDT_ACCESS_EXEC|GDT_ACCESS_TYPE|GDT_ACCESS_PRIV(0)|GDT_ACCESS_PRESENT,
	//	0xFF);
	////GDT_Table[2]=(GDTEntry)GDT_ENTRY(0L,
	////	0,
	////	GDT_ACCESS_RW|GDT_ACCESS_TYPE|GDT_ACCESS_PRIV(0)|GDT_ACCESS_PRESENT,
	////	GDT_FLAG_GRAN|GDT_FLAG_LONG);
	////U64(TSS)=(0x0L)<<52 | (0x89L)<<40 | (uint64_t)TSS<<16 | 8;
	////U64(&GDT_Table[3])=(0x0L)<<52 | (0x89L)<<40 | 8;
	//GDT_Descriptor.ptr=GDT_Table;
	//GDT_Descriptor.size=sizeof(GDT_Table)-1;
}
