#include "isr.h"
#include "idt.h"
#include "stdio.h"
#include "isr_gen.c"

ISRHandler g_ISRHandlers[256];
static const char* const g_Exceptions[] = {
    "Divide by zero error",
    "Debug",
    "Non-maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "Device Not Available",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack-Segment Fault",
    "General Protection Fault",
    "Page Fault",
    "",
    "x87 Floating-Point Exception",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Control Protection Exception ",
    "",
    "",
    "",
    "",
    "",
    "",
    "Hypervisor Injection Exception",
    "VMM Communication Exception",
    "Security Exception",
    ""
};

void __attribute__((cdecl)) ISR_Handler(Registers* r){
	int _int=r->interrupt;
	if(g_ISRHandlers[_int]){
		g_ISRHandlers[_int](r);
	}else if(_int>=32){
		printf("UNHANDLED INTERRUPT %d\n",_int);
	}else{
		printf("UNHANDLED EXCEPTION[%d] %s\n",_int,g_Exceptions[_int]);
	}
}
void ISR_Initialize(){
	ISR_SET_GATES();
	for(int i=0;i<256;i++){
		IDT_EnableGate(i);
	}
}
void ISR_addHandler(int interrupt,ISRHandler handler){
	g_ISRHandlers[interrupt]=handler;
}
