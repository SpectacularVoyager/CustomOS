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
void __attribute__((optimize("O0"))) TIMER_WAIT_INT(registers* r){
	apic_timer_c=1;
	//printf("RTC\n");
}
unsigned long APIC_TIMER_GETFREQ(){
	return LAPIC_TIMER_FREQUENCY;
}
void APIC_SET_TIMER_MODE(int mode){
	*LAPIC(APIC_LAPIC_TIMER)|=(mode<<17);
}
inline int getDivisor(int d){
	int t= (d-1)&7;
	return ((t<<1)&0b1000)|(t&0x3);
}
unsigned long __attribute__((optimize("O0"))) logTime(){
	while(apic_timer_c==0);
	unsigned long count =*LAPIC(APIC_LAPIC_CURRENT_TIMER_COUNT);
	//apic_timer_c=1;
	//while(apic_timer_c==0);
	printf("HEY");
	unsigned long count2 = count-*LAPIC(APIC_LAPIC_CURRENT_TIMER_COUNT);
	LOGVAL(count);
	LOGVAL(count2);
	return count2;
}
void APIC_TIMER_INIT(int ticks){
	int rate=10;

	//TIMER STUFF
	*LAPIC(APIC_LAPIC_TIMER)=(APIC_TIMER_ONE_SHOT<<17)|32;
	*LAPIC(APIC_LAPIC_TIMER_DIVIDER)=getDivisor(0);
	IOAPIC_MASKIRQ(2);
	IRQ_RegisterHandler(0,APIC_TIMER_INT);
	//122.070 micro seconds;
	IRQ_RegisterHandler(8,TIMER_WAIT_INT);
	RTC_SET_SPEED(14);
	RTC_INTERRUPT_ENABLE(1);
	*LAPIC(APIC_LAPIC_INITIAL_TIMER_COUNT)=0x1FFFFFFF;

	unsigned long count =logTime();

	LAPIC_TIMER_FREQUENCY=count<<16;	
	printf(INFO"APIC FREQ:\t%d\n",LAPIC_TIMER_FREQUENCY);
	RTC_INTERRUPT_ENABLE(0);
	IRQ_RegisterHandler(8,0);
	//IRQ_RegisterHandler(0,TIMER_WAIT_INT);
	*LAPIC(APIC_LAPIC_INITIAL_TIMER_COUNT)=0x0;
}
void APIC_SLEEP_MICRO(unsigned long s){
	printf("DEPRECATED\n");
	return;



	// s=s*LAPIC_TIMER_FREQUENCY/(1000*1000);
	// apic_timer_c=0;
	// *LAPIC(APIC_LAPIC_INITIAL_TIMER_COUNT)=s;
	// while(apic_timer_c==0);
}
void APIC_TIMER_STOP(){
	*LAPIC(APIC_LAPIC_INITIAL_TIMER_COUNT)=0x0;
}
void APIC_TIMER_LOOP(int ms,IRQHandler handler){
	int ticks=(ms*LAPIC_TIMER_FREQUENCY)/(1000*1000L);
	IRQ_RegisterHandler(0,handler);
	*LAPIC(APIC_LAPIC_INITIAL_TIMER_COUNT)=ticks;
	*LAPIC(APIC_LAPIC_TIMER)=(APIC_TIMER_PERIODIC<<17)|32;
}
void APIC_TIMER_ONCE(int ms,IRQHandler handler){
	int ticks=(ms*LAPIC_TIMER_FREQUENCY)/(1000*1000L);
	IRQ_RegisterHandler(0,handler);
	*LAPIC(APIC_LAPIC_INITIAL_TIMER_COUNT)=ticks;
	*LAPIC(APIC_LAPIC_TIMER)=(APIC_TIMER_ONE_SHOT<<17)|32;
	*LAPIC(APIC_LAPIC_INITIAL_TIMER_COUNT)=0;
}
