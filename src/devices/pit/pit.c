#include "pit.h"
#include "utils/ports.h"
#include "interrupts/idt.h"
#include "interrupts/irq.h"
#include "stdlib/stdio.h"
unsigned PIT_ReadCount(void) {
	unsigned count = 0;
	
	// Disable interrupts
	disableInterrupts();
	
	// al = channel in bits 6 and 7, remaining bits clear
	outb(PIT_PORT_MODE,0b0000000);
	
	count = inb(PIT_PORT0);		// Low byte
	count |= inb(PIT_PORT0)<<8;		// High byte
	enableInterrupts();
	
	return count;
}
void PIT_SetCount(unsigned count) {
	// Disable interrupts
	disableInterrupts();
	
	// Set low byte
	outb(PIT_PORT0,count&0xFF);		// Low byte
	outb(PIT_PORT0,(count&0xFF00)>>8);	// High byte
	enableInterrupts();
	return;
}
volatile int count;
void PIT_INT(registers* r){
	count--;
}
void PIT_SLEEP(unsigned int millis){
	if(millis<=0)return;
	unsigned int start=PIT_ReadCount();
	while(start+millis>=PIT_ReadCount());
}
void PIT_Initialize(int freq){
    //uint16_t divisor = PIT_FREQUENCY / freq;
    uint16_t divisor = 1;
	disableInterrupts();
	outb(PIT_PORT_MODE,PIT_SELECT_CHANNEL_0|PIT_ACCESS_HILOBYTE|PIT_MODE2);
    //outb(PIT_PORT0, (uint8_t)(divisor & 0xFF));
    //outb(PIT_PORT0, (uint8_t)((divisor >> 8) & 0xFF));
	enableInterrupts();
	IRQ_RegisterHandler(0,PIT_INT);
}
