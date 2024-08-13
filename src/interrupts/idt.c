#include "idt.h"
#include "../utils/bit.h"
#include "../stdlib/stdio.h"
#include "isrgen.h"


idt_entry_t g_IDT[256];
idtr_t g_IDTDescriptor={sizeof(g_IDT)-1,g_IDT};





void IDT_SetGate(int interupt,void* base,uint16_t segmentDescriptor,uint8_t flags){
	idt_entry_t* a=&g_IDT[interupt];
	g_IDT[interupt].isr_low  = WORD((long)base,0);
	g_IDT[interupt].isr_mid  = WORD((long)base,1);
	g_IDT[interupt].isr_high = DWORD((long)base,1);
	g_IDT[interupt].kernel_cs=segmentDescriptor;
	g_IDT[interupt].flags=flags;
	g_IDT[interupt].ist=0;
	g_IDT[interupt].reserved=0;
}
void IDT_EnableGate(int interupt){
	FLAG_SET(g_IDT[interupt].flags,IDT_FLAG_PRESENT);
}
void IDT_DisableGate(int interupt){
	FLAG_RESET(g_IDT[interupt].flags,IDT_FLAG_PRESENT);
}
void IDT_Initialize(uint16_t code){
	for(int i=0;i<15;i++){
		//IDT_SetGate(i,ISRFunctions[i],code,0x8E);
		//IDT_EnableGate(i);
	}

	SetISRs(code);
	IDT_Load(&g_IDTDescriptor);

}
