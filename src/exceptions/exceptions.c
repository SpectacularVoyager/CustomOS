#include "exceptions.h"
#include <stdint.h>
#include "interrupts/isr.h"
#include "utils/bit.h"
#include "userspace/syscall.h"
#include "utils/inst.h"
#include "utils/utils.h"

#include "usertask/Task.h"
#define PAGE_FAULT_EXCEPTION_PRESENT	0
#define PAGE_FAULT_EXCEPTION_WRITE		1
#define PAGE_FAULT_EXCEPTION_USER		2
#define PAGE_FAULT_EXCEPTION_RES_WRITE	3

void PageFaultHandler(registers* r){

	//SetColor(0xFF00000);
	printf(ERROR "PAGE FAULT\n");
	int error=r->zero;
	if(BIT(error,PAGE_FAULT_EXCEPTION_WRITE)){
		printf("\tILLEGAL WRITE\n");
	}else{
		printf("\tILLEGAL READ\n");
	}
	if(BIT(error,PAGE_FAULT_EXCEPTION_PRESENT)){
		printf("\tPAGE PRESENT VIOLATION\n");
	}
	if(BIT(error,PAGE_FAULT_EXCEPTION_USER)){
		printf("\tPAGE PROTECTION VIOLATION\n");
	}
		
	
	TASK* t=TaskCurrent();
	if(t!=NULL){
		printf("DURING EXECUTION OF TASK:%d\n",t->id);
		printf("\tTASK RIP:\t%p\n",t->r->rip);
	}

    uint64_t cr2_value;
    asm volatile ("mov %%cr2, %0" : "=r" (cr2_value));
	if(cr2_value==0){
		printf("\tNULL POINTER EXCEPTION\n");
	}
	printf("\tBAD INSTRUCTION ADDRESS\t%p\n",r->rip);
	printf("\tBAD ADDRESS\t%p\n",cr2_value);
	printf("\tERROR REGISTER:\t%x\n",error);
	if((r->rflags>>12)==3){
		r->rip=(uint64_t)USER_PRIV_LOOP;
		TaskKill();
	}else{
		__asm__ volatile("cli;hlt");
	}
}
void InvalidOpcodeException(registers* r){
	//SetColor(0xFF00000);
	printf(ERROR "INVALID OPCODE\n");
	uint8_t* inst=((uint8_t*)r->rip);
	printf("THE EXCEPTION OCCURED AT %p\n",r->rip);
	INST_READ(inst);
	//printf("THE Next Bytes Are %02X %02X %02X %02X\n",inst[0],inst[1],inst[2],inst[3]);
	printf("THE Previous Bytes Are %02X %02X %02X %02X\n",inst[-1],inst[-2],inst[-3],inst[-4]);
	if((r->rflags>>12)==3){
		r->rip=(uint64_t)USER_PRIV_LOOP;
		TaskKill();
	}else{
		__asm__ volatile("cli;hlt");
	}
}
void GeneralProtectionFault(registers* r){
	//SetColor(0xFF00000);
	printf(ERROR "GENERAL PROTECTION FAULT\n");
	if(r->zero!=0){
		SELECTOR_ERROR* error=(void*)&r->zero;

		printf("INCORRECT SEGMENT:\n");
		if(error->tbl==0x0)	printf("\tERROR IN TABLE GDT\n");
		if(error->tbl==0x1)	printf("\tERROR IN TABLE IDT\n");
		if(error->tbl==0x2)	printf("\tERROR IN TABLE LDT\n");
		if(error->tbl==0x3)	printf("\tERROR IN TABLE IDT\n");
		printf("\tSEGMENT:\t%x\n",error->index);
	}
	printf("SEGMENT[0x%x]\n",r->cs);
	printf("FLAGS[0x%x]\n",r->rflags);
	uint8_t* inst=((uint8_t*)r->rip);
	printf("THE EXCEPTION OCCURED AT %p\n",r->rip);
	printf("THE ERROR CODE IS ??\n");
	INST_READ(inst);
	// printf("THE Next Bytes Are %02X %02X %02X %02X\n",inst[0],inst[1],inst[2],inst[3]);
	printf("THE Previous Bytes Are %02X %02X %02X %02X\n",inst[-1],inst[-2],inst[-3],inst[-4]);
	if((r->rflags>>12)==3){
		r->rip=(uint64_t)USER_PRIV_LOOP;
		TaskKill();
	}else{
		__asm__ volatile("cli;hlt");
	}
}
void ExceptionInit(){
	ISR_addHandler(6,InvalidOpcodeException);
	ISR_addHandler(13,GeneralProtectionFault);
	ISR_addHandler(14,PageFaultHandler);
	ISR_addHandler(0x80,syscall);
}
