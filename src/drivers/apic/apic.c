#include "apic.h"
#include "../../stdlib/stdio.h"

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
	while(record<((void*)madt)+n){
		APIC_RECORD* apic_record=((APIC_RECORD*)record);
		printf(INFO"APIC RECORD:%x\n",apic_record->type);

		if(apic_record->type==APIC_LOCAL_ADDRESS_OVERRIDE){
			localAPICaddr=(void*)((APIC_LOCAL_ADDRESS_OVERRIDE_RECORD*)apic_record)->phy_addr;
		}
		record+=apic_record->length;
	}
	localAPICaddr[0xF0]|=0x100;
	//asm volatile("INT $0x22");
	printf(INFO"APIC ADDRESS %p\n",localAPICaddr);
	return 1;
}
