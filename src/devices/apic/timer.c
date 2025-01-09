#include "timer.h"
#include "interrupts/irq.h"
#include "stdlib/stdio.h"
#include "drivers/apic/apic.h"
#include "devices/rtc/rtc.h"
#include "utils/utils.h"

unsigned long LAPIC_TIMER_FREQUENCY;
void APIC_TIMER_INT(registers* r){
	printf("INTERRUPTED\n");
	APIC_SEND_EOI();
}
volatile int apic_timer_c=0;
void TIMER_WAIT_INT(registers* r){
	apic_timer_c=1;
}
unsigned long APIC_TIMER_GETFREQ(){
	return LAPIC_TIMER_FREQUENCY;
}
void APIC_SET_TIMER_MODE(int mode){
	*LAPIC(APIC_LAPIC_TIMER)|=(mode<<17);
}
void APIC_TIMER_INIT(int ticks){
	int rate=12;

	//TIMER STUFF
	*LAPIC(APIC_LAPIC_TIMER)=(APIC_TIMER_ONE_SHOT<<17)|32;
	*LAPIC(APIC_LAPIC_TIMER_DIVIDER)=0x1011;
	*LAPIC(APIC_LAPIC_INITIAL_TIMER_COUNT)=0x2000000;
	IOAPIC_MASKIRQ(2);
	IRQ_RegisterHandler(0,APIC_TIMER_INT);
	unsigned long count =*LAPIC(APIC_LAPIC_CURRENT_TIMER_COUNT);
	LOGVAL(count);
	//122.070 micro seconds;
	IRQ_RegisterHandler(8,TIMER_WAIT_INT);
	RTC_SET_SPEED(rate);
	RTC_INTERRUPT_ENABLE(1);
	while(apic_timer_c==0);
	count=count-*LAPIC(APIC_LAPIC_CURRENT_TIMER_COUNT);
	LOGVAL(count);
	LAPIC_TIMER_FREQUENCY=count*1024L>>(rate-0x6);	
	printf(INFO"APIC FREQ:\t%d\n",LAPIC_TIMER_FREQUENCY);
	RTC_INTERRUPT_ENABLE(0);
	IRQ_RegisterHandler(8,0);
	IRQ_RegisterHandler(0,TIMER_WAIT_INT);
	*LAPIC(APIC_LAPIC_INITIAL_TIMER_COUNT)=0x0;
}
void APIC_SLEEP_MICRO(unsigned long s){
	s=s*LAPIC_TIMER_FREQUENCY/(1000*1000);
	apic_timer_c=0;
	*LAPIC(APIC_LAPIC_INITIAL_TIMER_COUNT)=s;
	while(apic_timer_c==0);
}
