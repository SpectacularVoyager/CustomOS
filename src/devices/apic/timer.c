#include "timer.h"
#include "../../stdlib/stdio.h"

void APIC_TIMER_INT(registers* r){
	printf("INTERRUPTED\n");
	APIC_SEND_EOI();
}
void APIC_TIMER_INIT(int ticks){
	//TIMER STUFF
	*LAPIC(APIC_LAPIC_TIMER)=(1<<17)|32;
	*LAPIC(APIC_LAPIC_TIMER_DIVIDER)=0x3;
	*LAPIC(APIC_LAPIC_INITIAL_TIMER_COUNT)=ticks;

	IRQ_RegisterHandler(0,APIC_TIMER_INT);
}
