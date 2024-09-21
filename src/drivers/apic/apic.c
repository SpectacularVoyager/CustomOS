#include "apic.h"
#include "../../stdlib/stdio.h"
#include "../../utils/ports.h"
#include "../../utils/bit.h"
#include <stdint.h>


volatile uint8_t* localAPICaddr;
int APIC_INIT(MADT* madt){

	SetColor(0x00ffff);
	if(!madt){
		printf(ERROR "MADT NOT FOUND\n");
		return 0;
	}
	localAPICaddr=(uint8_t*)madt->localAPICaddr;
	int n=madt->h.Length;
	void* record=madt->records;

	uint64_t ptr;
	uint32_t eax,edx;
	rdmsr(0x1B,&eax,&edx);
	uint64_t addr=((uint64_t)edx)<<32|eax;
	printf(INFO"RDMSR ADDR:\t%x%08x\n",edx,eax);
	if(!(BIT(eax,11))){
		printf(ERROR"APIC NOT ENABLED\n",eax);
	}
	printf(INFO"APIC ADDRESS %p\n",localAPICaddr);
	while(record<((void*)madt)+n){
		APIC_RECORD* apic_record=((APIC_RECORD*)record);
		//printf(INFO"APIC RECORD:%x\n",apic_record->type);

		if(apic_record->type==APIC_PROCESSOR_LOCAL){
			APIC_PROCESSOR_LOCAL_RECORD* local=(APIC_PROCESSOR_LOCAL_RECORD*)apic_record;
		}
		if(apic_record->type==APIC_LOCAL_ADDRESS_OVERRIDE){
			localAPICaddr=(void*)((APIC_LOCAL_ADDRESS_OVERRIDE_RECORD*)apic_record)->phy_addr;
		}
		record+=apic_record->length;
	}
	printf(INFO"ADDRESS\t%p\n",localAPICaddr);
	printf(INFO"ADDRESS\t%x\n",*(uint32_t*)(localAPICaddr+0x10));
	//for(int i=0;i<200;i++){
	//	for(int j=0;j<80;j++){
	//		printf("%x",localAPICaddr[i*80+j]);
	//	}
	//	printf("\n");
	//}
	//localAPICaddr=(uint8_t*)addr;
	//printf(INFO"APIC ADDRESS %p\n",localAPICaddr);
	//printf(INFO"LAPICID %p\n",*(uint64_t*)localAPICaddr);
	//asm volatile("INT $0x22");
	return 1;
}
