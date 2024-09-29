#include "apic.h"
#include "../../stdlib/stdio.h"
#include "../../stdlib/stdlib.h"
#include "../../utils/ports.h"
#include "../../utils/bit.h"
#include <stdint.h>


volatile uint8_t* localAPICaddr;
#define LAPIC_GET(x) ((uint32_t*)(localAPICaddr+x))

void APIC_SEND_EOI(){
	*LAPIC_GET(APIC_LAPIC_EOI)=0;
}
uint32_t* LAPIC(unsigned int x){
	return LAPIC_GET(x);
}

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

	volatile uint64_t addr=RDMSR(IA32_APIC_BASE_MSR);
	printf(INFO"RDMSR ADDR:\t%p\n",addr);
	if(!(BIT(addr,11))){
		printf(ERROR"APIC NOT ENABLED\n",addr);
	}

	APIC_IO_RECORD* apicio=0;

	while(record<((void*)madt)+n){
		APIC_RECORD_UNION* apic_record=((APIC_RECORD_UNION*)record);
		printf(INFO"APIC RECORD:%x\n",apic_record->base.type);

		if(apic_record->base.type==APIC_IO){
			apicio=&apic_record->ioapic;
		}
		if(apic_record->base.type==APIC_LOCAL_ADDRESS_OVERRIDE){
			localAPICaddr=(void*)((APIC_LOCAL_ADDRESS_OVERRIDE_RECORD*)apic_record)->phy_addr;
		}
		record+=apic_record->base.length;
	}

	*LAPIC_GET(APIC_LAPIC_SPURIOUS_INTERRUPTS)|=(1<<8);
	*LAPIC_GET(APIC_LAPIC_SPURIOUS_INTERRUPTS)&=~(0xf);

	printf(INFO"LAPIC ID\t%x\n" ,*LAPIC_GET(APIC_LAPIC_ID));
	printf(INFO"LAPIC VERSION\t%x\n",*LAPIC_GET(APIC_LAPIC_VERSION));
	printf(INFO"LAPIC SPURIOUS\t%x\n",*LAPIC_GET(APIC_LAPIC_SPURIOUS_INTERRUPTS));


	return 1;
}
