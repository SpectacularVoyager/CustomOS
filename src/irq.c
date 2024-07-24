#include "irq.h"
#include "isr.h"
#include "pic.h"
#include "io.h"
#include "stdio.h"

#define PIC_REMAP_OFFSET	0x20

IRQHandler g_IRQHandlers[16];

void IRQ_Handler(Registers* r){
	int irq=r->interrupt - PIC_REMAP_OFFSET;
	if(g_IRQHandlers[irq]){
		g_IRQHandlers[irq](r);
	}else{
		printf("UNHANDLED IRQ %d\n",irq);
	}
	PIC_SendEOI(irq);
}

void IRQ_Initialize(){
	PIC_Initialize(PIC_REMAP_OFFSET,PIC_REMAP_OFFSET+8);
	for(int i=0;i<16;i++){
		ISR_addHandler(PIC_REMAP_OFFSET+i,IRQ_Handler);
	}
	EnableInterrupts();	
}
void IRQ_RegisterHandler(int irq,IRQHandler handler){
	g_IRQHandlers[irq]=handler;
}
