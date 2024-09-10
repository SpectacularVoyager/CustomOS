#include "exceptions.h"
#include <stdint.h>
#include "../utils/bit.h"
#define PAGE_FAULT_EXCEPTION_PRESENT	0
#define PAGE_FAULT_EXCEPTION_WRITE		1
#define PAGE_FAULT_EXCEPTION_USER		2
#define PAGE_FAULT_EXCEPTION_RES_WRITE	3

void PageFaultHandler(registers* r){
	kprintf(ERROR "PAGE FAULT\n");
	int error=r->zero;
	if(BIT(error,PAGE_FAULT_EXCEPTION_WRITE)){
		kprintf("\tILLEGAL WRITE\n");
	}else{
		kprintf("\tILLEGAL READ\n");
	}
	if(BIT(error,PAGE_FAULT_EXCEPTION_PRESENT)){
		kprintf("\tPAGE PROTECTION VIOLATION\n");
	}
		
	


    uint64_t cr2_value;
    asm volatile ("mov %%cr2, %0" : "=r" (cr2_value));
	kprintf("\tBAD ADDRESS\t%p\n",cr2_value);
	kprintf("\tERROR REGISTER:\t%x\n",error);
	__asm__ volatile("cli;hlt");
}
void InvalidOpcodeException(registers* r){
	kprintf(ERROR "INVALID OPCODE\n");
	uint8_t* inst=((uint8_t*)r->rip);
	kprintf("THE EXCEPTION OCCURED AT %p\n",r->rip);
	kprintf("THE Next Bytes Are %02X %02X %02X %02X\n",inst[0],inst[1],inst[2],inst[3]);
	kprintf("THE Previous Bytes Are %02X %02X %02X %02X\n",inst[-1],inst[-2],inst[-3],inst[-4]);
	__asm__ volatile("cli;hlt");
}
void ExceptionInit(){
	ISR_addHandler(14,PageFaultHandler);
	ISR_addHandler(6,InvalidOpcodeException);
}
