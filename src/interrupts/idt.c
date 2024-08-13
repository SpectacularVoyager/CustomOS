#include "idt.h"
#include "../utils/bit.h"
#include "../stdlib/stdio.h"
#include "isrgen.h"


idt_entry_t g_IDT[256];
idtr_t g_IDTDescriptor={sizeof(g_IDT)-1,g_IDT};





void IDT_SetGate(int interupt,void* base,uint16_t segmentDescriptor,uint8_t flags){
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

void printRegs(registers* r){
	printf("[REGISTERS]\n");
	printf("r15	\t%x\n",r->r15);
	printf("r14	\t%x\n",r->r14);
	printf("r13	\t%x\n",r->r13);
	printf("r12	\t%x\n",r->r12);
	printf("r11	\t%x\n",r->r11);
	printf("r10	\t%x\n",r->r10);
	printf("r9	\t%x\n",r->r9);
	printf("r8	\t%x\n",r->r8);
	printf("rbp	\t%x\n",r->rbp);
	printf("rdi	\t%x\n",r->rdi);
	printf("rsi	\t%x\n",r->rsi);
	printf("rdx	\t%x\n",r->rdx);
	printf("rcx	\t%x\n",r->rcx);
	printf("rbx	\t%x\n",r->rbx);
	printf("rax	\t%x\n",r->rax);
	printf("interupt\t%x\n",r->interupt);
	printf("IP  \t%x\n",r->rip);
	printf("CS  \t%x\n",r->cs);
	printf("RFLAGS \t%x\n",r->rflags);
	printf("RSP  \t%x\n",r->rsp);
	printf("SS  \t%x\n",r->ss);

}
