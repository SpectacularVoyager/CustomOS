#include "apic.h"
#include "../../stdlib/stdio.h"
#include "../../stdlib/stdlib.h"
#include "../../utils/ports.h"
#include "../../utils/bit.h"
#include "../../paging/paging.h"
#include <stdint.h>


volatile uint8_t* localAPICaddr;
volatile uint64_t ioapic_base=0xfec00000;
#define LAPIC_GET(x) ((uint32_t*)(localAPICaddr+x))

void APIC_SEND_EOI(){
	*LAPIC_GET(APIC_LAPIC_EOI)=0;
}
uint32_t* LAPIC(unsigned int x){
	return LAPIC_GET(x);
}
void IOAPIC_WRITE(uint8_t offset, uint32_t val) {
    *(volatile uint32_t*) (ioapic_base + IOAPIC_IOREGSEL) = offset;
    *(volatile uint32_t*) (ioapic_base + IOAPIC_IOREGWIN) = val;
}

uint32_t IOAPIC_READ(uint8_t offset) {
    *(volatile uint32_t*) (ioapic_base + IOAPIC_IOREGSEL) = offset;
    return *(volatile uint32_t*) (ioapic_base + IOAPIC_IOREGWIN);
}
uint64_t IOAPIC_READIRQ(uint8_t offset) {
	return
		IOAPIC_READ(0x10+offset*2)
		|(((uint64_t)IOAPIC_READ(0x10+offset*2+1))<<32)
		;
}
void IOAPIC_WRITEIRQ(uint8_t offset,uint64_t val) {
	uint32_t low=val&0xffffffff;
	uint32_t high=(val>>32)&0xffffffff;
	IOAPIC_WRITE(0x10+offset*2,low);
	IOAPIC_WRITE(0x10+offset*2+1,high);
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

	APIC_IO_RECORD* ioapic=0;

	while(record<((void*)madt)+n){
		APIC_RECORD_UNION* apic_record=((APIC_RECORD_UNION*)record);
		//printf(INFO"APIC RECORD:%x\n",apic_record->base.type);

		if(apic_record->base.type==APIC_IO){
			ioapic=&apic_record->ioapic;
		}
		if(apic_record->base.type==APIC_LOCAL_ADDRESS_OVERRIDE){
			localAPICaddr=(void*)apic_record->address_override.phy_addr;
		}
		if(apic_record->base.type==APIC_IO_INT_SOURCE_OVERRIDE){
			printf(INFO"INT %x[%x]\t%x\n",apic_record->ioapic_int.bus,apic_record->ioapic_int.irq,apic_record->ioapic_int.global_int_base);
		}
		record+=apic_record->base.length;
	}

	*LAPIC_GET(APIC_LAPIC_SPURIOUS_INTERRUPTS)|=(1<<8);
	*LAPIC_GET(APIC_LAPIC_SPURIOUS_INTERRUPTS)&=~(0xf);

	printf(INFO"LAPIC ID\t%x\n" ,*LAPIC_GET(APIC_LAPIC_ID));
	printf(INFO"LAPIC VERSION\t%x\n",*LAPIC_GET(APIC_LAPIC_VERSION));
	printf(INFO"LAPIC SPURIOUS\t%x\n",*LAPIC_GET(APIC_LAPIC_SPURIOUS_INTERRUPTS));

	void* lapic_memory=mallocA(0x200000,0x200000);
	MemoryRemap((uint64_t)lapic_memory,ioapic->ioapic_addr,1<<4);
	ioapic_base=(uint64_t)lapic_memory;

	printf(INFO"IOAPIC_VERSION:\t%x\n",IOAPIC_READ(0x1));
	printf(INFO"IRQ 0:\t%p\n",IOAPIC_READIRQ(0));
	printf(INFO"IRQ 1:\t%p\n",IOAPIC_READIRQ(1));
	
	IOAPIC_WRITEIRQ(1,0x21);
	//printf(INFO"LAPIC\t%p\n",(1<<11)|0x21||(0xf<56));
	//printf(INFO"IRQ 1:\t%p\n",IOAPIC_READIRQ(2));
	return 1;
}
