#include "pit.h"
#include "io.h"

unsigned PIT_ReadCount(void) {
	unsigned count = 0;
	
	// Disable interrupts
	DisableInterrupts();
	
	// al = channel in bits 6 and 7, remaining bits clear
	outb(PIT_PORT_MODE,0b0000000);
	
	count = inb(PIT_PORT0);		// Low byte
	count |= inb(PIT_PORT0)<<8;		// High byte
	EnableInterrupts();
	
	return count;
}
void PIT_SetCount(unsigned count) {
	// Disable interrupts
	DisableInterrupts();
	
	// Set low byte
	outb(PIT_PORT0,count&0xFF);		// Low byte
	outb(PIT_PORT0,(count&0xFF00)>>8);	// High byte
	EnableInterrupts();
	return;
}
void PIT_Initialize(){
    uint16_t divisor = PIT_FREQUENCY / 20;
	DisableInterrupts();
	outb(PIT_PORT_MODE,PIT_SELECT_CHANNEL_0|PIT_ACCESS_HILOBYTE|PIT_MODE2);
    outb(PIT_PORT0, (uint8_t)(divisor & 0xFF));
    outb(PIT_PORT0, (uint8_t)((divisor >> 8) & 0xFF));
	EnableInterrupts();
}
