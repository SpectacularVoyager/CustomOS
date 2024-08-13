#pragma once
#include "isr.h"
#include "../stdlib/stdio.h"

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

ISRHandler g_ISRHandlers[256];
void ISR_Handler(uint64_t* regs) {
	registers* r=(registers*)regs;
	
	
	int _int=r->interupt;
	if(g_ISRHandlers[_int]){
		g_ISRHandlers[_int](r);
	}else if(_int>=32){
		printf("UNHANDLED INTERRUPT %d\n",_int);
	}else{
		//printf("EXECPTION:%d\t HALTING IMMEDIATELY\n",r->interupt);
		printf("UNHANDLED EXCEPTION[%d] %s\n",_int,g_Exceptions[_int]);
		__asm__ volatile ("cli; hlt"); // Completely hangs the computer
	}
}

void ISR_addHandler(int interrupt,ISRHandler handler){
	g_ISRHandlers[interrupt]=handler;
}
