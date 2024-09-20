#include "irq.h"
#include "isr.h"
#include "idt.h"
#include "../drivers/pic.h"
#include "../utils/ports.h"
#include "../stdlib/stdio.h"

#define PIC_REMAP_OFFSET	0x20

IRQHandler g_IRQHandlers[16];

void IRQ_Handler(registers* r){
	int irq=r->interupt - PIC_REMAP_OFFSET;
	if(g_IRQHandlers[irq]){
		g_IRQHandlers[irq](r);
	}else{
		//printf("UNHANDLED IRQ %d\n",irq);
		if(irq!=0){
			printf(TRACE"UNHANDLED IRQ %d\n",irq);
		}
	}
	PIC_sendEOI(irq);
}

void IRQ_Initialize(){
	PIC_INIT(PIC_REMAP_OFFSET,PIC_REMAP_OFFSET+8);
	for(int i=0;i<16;i++){
		ISR_addHandler(PIC_REMAP_OFFSET+i,IRQ_Handler);
	}
	enableInterrupts();
	//EnableInterrupts();	
}
void IRQ_RegisterHandler(int irq,IRQHandler handler){
	g_IRQHandlers[irq]=handler;
}
