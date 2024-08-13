#include "pic.h"
#include "../utils/ports.h"
#include <stdint.h>

void PIC_sendEOI(uint8_t irq)
{
	if(irq >= 8)
		outb(PIC2_COMMAND,PIC_EOI);
	
	outb(PIC1_COMMAND,PIC_EOI);
}
void PIC_INIT(int offset1,int offset2){
	int a1=inb(PIC1_DATA);
	int a2=inb(PIC2_DATA);

	outb(PIC1_COMMAND,PIC_ICW1_INIT|PIC_ICW1_ICW4);
	io_wait();
	outb(PIC2_COMMAND,PIC_ICW1_INIT|PIC_ICW1_ICW4);
	io_wait();

	outb(PIC1_DATA,offset1);
	io_wait();
	outb(PIC2_DATA,offset2);
	io_wait();

	outb(PIC1_DATA,4);
	io_wait();
	outb(PIC2_DATA,2);
	io_wait();

	outb(PIC1_DATA,PIC_ICW4_8086);
	io_wait();
	outb(PIC2_DATA,PIC_ICW4_8086);
	io_wait();

	outb(PIC1_DATA,a1);
	outb(PIC2_DATA,a2);

}

void PIC_Disable(){
    outb(PIC1_DATA, 0xff);
    outb(PIC2_DATA, 0xff);
}
static uint16_t __PICGetIrqReg(int ocw3)
{
    /* OCW3 to PIC CMD to get the register values.  PIC2 is chained, and
     * represents IRQs 8-15.  PIC1 is IRQs 0-7, with 2 being the chain */
    outb(PIC1_COMMAND, ocw3);
    outb(PIC2_COMMAND, ocw3);
    return (inb(PIC2_COMMAND) << 8) | inb(PIC1_COMMAND);
}

/* Returns the combined value of the cascaded PICs irq request register */
uint16_t PIC_GetIRR(void)
{
    return __PICGetIrqReg(PIC_READ_IRR);
}

/* Returns the combined value of the cascaded PICs in-service register */
uint16_t PIC_GetISR(void)
{
    return __PICGetIrqReg(PIC_READ_ISR);
}
