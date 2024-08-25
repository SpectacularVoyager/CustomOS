#include "exceptions.h"
#define PAGE_FAULT_EXCEPTION_PRESENT	1<<0
#define PAGE_FAULT_EXCEPTION_WRITE		1<<1
#define PAGE_FAULT_EXCEPTION_USER		1<<2
#define PAGE_FAULT_EXCEPTION_RES_WRITE	1<<3
void PageFaultHandler(registers* r){
	kprintf(ERROR "PAGE FAULT\n");
	int error=r->zero;
	if(error&=PAGE_FAULT_EXCEPTION_WRITE){
		kprintf("\tILLEGAL WRITE\n");
	}else{
		kprintf("\tILLEGAL READ\n");
	}
	if(error&=PAGE_FAULT_EXCEPTION_PRESENT){
		kprintf("\tPAGE PROTECTION VIOLATION\n");
	}
    uint64_t cr2_value;
    asm volatile ("mov %%cr2, %0" : "=r" (cr2_value));
	kprintf("\tBAD ADDRESS\t%p",cr2_value);
	__asm__ volatile("cli;hlt");
}
void ExceptionInit(){
	ISR_addHandler(14,PageFaultHandler);
}
