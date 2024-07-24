#include "isr.h"
#include "idt.h"
#include "stdio.h"
#include "isr_gen.c"

ISRHandler g_ISRHandlers[256];

void __attribute__((cdecl)) ISR_Handler(Registers* r){
	int _int=r->interrupt;
	if(g_ISRHandlers[_int]){
		g_ISRHandlers[_int](r);
	}else if(_int>=32){
		printf("UNHANDLED INTERRUPT %d\n",_int);
	}else{
		printf("UNHANDLED EXCEPTION %d\n",_int);
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
