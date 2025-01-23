#include "GDT.h"
#include "stdlib/string.h"
#include "utils/utils.h"

extern void* stack_top_syscall;

void GDT_LoadEntry(GDTEntry* entry,long base,int limit,unsigned char access,unsigned char flags){
	entry->LimitLow			= ((limit)&0xFFFF);
	entry->BaseLow			= ((base)&0xFFFF);		
	entry->BaseMiddle		= ((base>>16)&0xFF);
	entry->Access			= ((access));
	entry->FlagsLimitHigh	= (((flags&0xF)<<4)|((limit>>16)&0x0F));
	entry->BaseHigh			= ((base>>24)&0xFF);			
}
void TSS_LoadEntry(TSSEntry* entry,long base,int limit,unsigned char access,unsigned char flags){
	entry->LimitLow			= ((limit)&0xFFFF);
	entry->BaseLow			= ((base)&0xFFFF);		
	entry->BaseMiddle		= ((base>>16)&0xFF);
	entry->Access			= ((access));
	entry->FlagsLimitHigh	= (((flags&0xF)<<4)|((limit>>16)&0x0F));
	entry->BaseHigh			= ((base>>24)&0xFF);			
	entry->BaseLong			= ((base>>32));							
	entry->resv				= 0;								
}

GDTEntry __attribute__((aligned(64))) GDT_Table[7] ;
GDTDescriptor __attribute__((aligned(64))) GDT_Descriptor;
TSS __attribute__((aligned(64))) tss;

//https://forum.osdev.org/viewtopic.php?t=40898
void GDT_LOAD(){
	GDT_LoadEntry(&GDT_Table[0],0,0,0,0);
	GDT_LoadEntry(&GDT_Table[1],0,0,GDT_ACCESS_PRIV(0)|0x9A,0xA);
	GDT_LoadEntry(&GDT_Table[2],0,0,GDT_ACCESS_PRIV(0)|0x92,0xC);
	GDT_LoadEntry(&GDT_Table[3],0,0,GDT_ACCESS_PRIV(3)|0x9A,0xA);
	GDT_LoadEntry(&GDT_Table[4],0,0,GDT_ACCESS_PRIV(3)|0x92,0xC);

	memset(&tss,0,sizeof(TSS));

	tss.RSP0=(uint64_t)stack_top_syscall;
	TSS_LoadEntry((TSSEntry*)(&GDT_Table[5]),(uint64_t)&tss,sizeof(TSS)-1,
			GDT_ACCESS_PRESENT|GDT_ACCESS_PRIV(3)|GDT_ACCESS_EXEC|GDT_ACCESS_ACCESSED
			,0);
	GDT_Descriptor.ptr=GDT_Table;
	GDT_Descriptor.size=sizeof(GDT_Table)-1;
}
void GDT_FLUSH(){
	__asm__ __volatile__("lgdt %0"::"m"(GDT_Descriptor));
}
void TSS_FLUSH() {
	int r=0x28;
    __asm__ volatile (
        "ltr %0"  // Load Task Register with the value in 'selector'
        :  // No output operands
        : "r" (r)  // Input operand: 'selector'
        : "memory"  // Inform the compiler that memory is affected
    );
}
