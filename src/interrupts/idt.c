#include "idt.h"
#include "../utils/bit.h"
#include "../stdlib/stdio.h"

idt_entry_t g_IDT[256];
idtr_t g_IDTDescriptor={sizeof(g_IDT)-1,g_IDT};
extern void* isr_stub_table[];

void IDT_SetGate(int interupt,void* base,uint16_t segmentDescriptor,uint8_t flags){
	idt_entry_t* a=&g_IDT[interupt];
	a->isr_low  = WORD((long)base,0);
	a->isr_mid  = WORD((long)base,1);
	a->isr_high = DWORD((long)base,1);
	a->kernel_cs=segmentDescriptor;
	a->flags=flags;
	a->ist=0;
	a->reserved=0;
}
void IDT_EnableGate(int interupt){
	FLAG_SET(g_IDT[interupt].flags,IDT_FLAG_PRESENT);
}
void IDT_DisableGate(int interupt){
	FLAG_RESET(g_IDT[interupt].flags,IDT_FLAG_PRESENT);
}

void IDT_Initialize(uint16_t code){
	IDT_Load(&g_IDTDescriptor);
	for(int i=0;i<256;i++){
		IDT_SetGate(i,isr_stub_table[i],code,0x8E);
		IDT_EnableGate(i);
	}

    //__asm__ volatile ("lidt %0" : : "m"(g_IDTDescriptor)); // load the new IDT
    //__asm__ volatile ("sti"); // set the interrupt flag
}
