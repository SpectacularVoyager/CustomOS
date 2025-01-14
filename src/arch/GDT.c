#include "GDT.h"

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

GDTEntry GDT_Table[7] __attribute__((aligned(64)));
GDTDescriptor GDT_Descriptor;
extern GDTEntry* gdt64;
TSS tss;

void GDT_LOAD(){
	GDT_LoadEntry(&GDT_Table[0],0,0,0,0);
	GDT_LoadEntry(&GDT_Table[1],0,0,0x9A,0xA);
	GDT_LoadEntry(&GDT_Table[2],0,0,0x92,0xC);
	GDT_LoadEntry(&GDT_Table[3],0,0,0x9A,0xA);
	GDT_LoadEntry(&GDT_Table[4],0,0,0x92,0xC);
	GDT_LoadEntry(&GDT_Table[5],(uint64_t)&tss,sizeof(TSS)-1,0x89,0);
}
void GDT_FLUSH(){
	__asm__ __volatile__("lgdt %0"::"m"(GDT_Table));
}
