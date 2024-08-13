#include "idt.h"
#include "../utils/bit.h"
#include "../stdlib/stdio.h"

idt_entry_t g_IDT[256];
idtr_t g_IDTDescriptor={sizeof(g_IDT)-1,g_IDT};
extern void* isr_stub_table[];

void IDT_SetGate(int interupt,void* base,uint16_t segmentDescriptor,uint8_t flags){
	idt_entry_t* a=&g_IDT[interupt];
	//g_IDT[interupt].isr_low  = WORD((long)base,0);
	//g_IDT[interupt].isr_mid  = WORD((long)base,1);
	//g_IDT[interupt].isr_high = DWORD((long)base,1);
	g_IDT[interupt].isr_low  = (uint64_t)base & 0xFFFF;
	g_IDT[interupt].isr_mid  = ((uint64_t)base >> 16) & 0xFFFF;
	g_IDT[interupt].isr_high = ((uint64_t)base >> 32) & 0xFFFFFFFF;
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
void ISR0();
void ISR1();
void ISR2();
void ISR3();
void ISR4();
void ISR5();
void ISR6();
void IDT_Initialize(uint16_t code){
	for(int i=0;i<256;i++){
		//IDT_SetGate(i,isr_stub_table[i],code,0x8E);
		//IDT_EnableGate(i);
	}
	printf("CODE:%d\n",code);
	IDT_SetGate(0,ISR0,code,0x8E);
	IDT_SetGate(1,ISR1,code,0x8E);
	IDT_SetGate(2,ISR2,code,0x8E);
	IDT_SetGate(3,ISR3,code,0x8E);
	IDT_SetGate(4,ISR4,code,0x8E);
	IDT_SetGate(5,ISR5,code,0x8E);
	IDT_SetGate(6,ISR6,code,0x8E);
	IDT_Load(&g_IDTDescriptor);

    //__asm__ volatile ("lidt %0" : : "m"(g_IDTDescriptor)); // load the new IDT
    //__asm__ volatile ("sti"); // set the interrupt flag
}
