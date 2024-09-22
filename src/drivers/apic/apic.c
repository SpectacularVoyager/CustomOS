#include "apic.h"
#include "../../stdlib/stdio.h"
#include "../../stdlib/stdlib.h"
#include "../../utils/ports.h"
#include "../../utils/bit.h"
#include <stdint.h>


volatile uint8_t* localAPICaddr;
#define LAPIC_GET(x) ((uint32_t*)(localAPICaddr+x))
int APIC_INIT(MADT* madt){
	void* memory=mallocA(0x200000,0x200000);
	WRMSR(IA32_APIC_BASE_MSR,
			(uint64_t)memory|IA32_APIC_BASE_MSR_BSP|IA32_APIC_BASE_MSR_ENABLE);

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
	rdmsr(IA32_APIC_BASE_MSR,&eax,&edx);
	volatile uint64_t addr=((uint64_t)edx)<<32|eax;
	printf(INFO"RDMSR ADDR:\t%x%08x\n",edx,eax);
	if(!(BIT(eax,11))){
		printf(ERROR"APIC NOT ENABLED\n",eax);
	}
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

	printf(INFO"LAPIC ID\t%x\n" ,*LAPIC_GET(APIC_LAPIC_ID));
	printf(INFO"LAPIC VERSION\t%x\n",*LAPIC_GET(APIC_LAPIC_VERSION));
	//*LAPIC_GET(APIC_LAPIC_SPURIOUS_INTERRUPTS)|=(1<<8|1<<12);

	return 1;
}
