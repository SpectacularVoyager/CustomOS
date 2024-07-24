#include "pic.h"
#include <stdint.h>
#include "io.h"

void PIC_Initialize(uint16_t offset1,uint16_t offset2){
	uint8_t a1,a2;
	a1=inb(PIC1_DATA_PORT);
	a2=inb(PIC2_DATA_PORT);

	outb(PIC1_COMMAND_PORT,PIC_ICW1_ICW4|PIC_ICW1_INIT);
	io_wait();
	outb(PIC2_COMMAND_PORT,PIC_ICW1_ICW4|PIC_ICW1_INIT);
	io_wait();

	outb(PIC1_DATA_PORT,offset1);
	io_wait();
	outb(PIC2_DATA_PORT,offset2);
	io_wait();

	outb(PIC1_DATA_PORT,0x4);
	io_wait();
	outb(PIC2_DATA_PORT,0x2);
	io_wait();

	outb(PIC1_DATA_PORT,PIC_ICW4_8086);
	io_wait();
	outb(PIC2_DATA_PORT,PIC_ICW4_8086);
	io_wait();

	outb(PIC1_DATA_PORT,a1);
	io_wait();
	outb(PIC2_DATA_PORT,a2);
	io_wait();
}
void PIC_Disable(void) {
    outb(PIC1_DATA_PORT, 0xff);
    outb(PIC2_DATA_PORT, 0xff);
}

void PIC_SendEOI(uint8_t irq)
{
	if(irq >= 8)
		outb(PIC2_COMMAND_PORT,PIC_EOI);
	
	outb(PIC1_COMMAND_PORT,PIC_EOI);
}
void IRQ_SetMask(uint8_t IRQline) {
    uint16_t port;
    uint8_t value;

    if(IRQline < 8) {
        port = PIC1_DATA_PORT;
    } else {
        port = PIC2_DATA_PORT;
        IRQline -= 8;
    }
    value = inb(port) | (1 << IRQline);
    outb(port, value);        
}

void IRQ_ClearMask(uint8_t IRQline) {
    uint16_t port;
    uint8_t value;

    if(IRQline < 8) {
        port = PIC1_DATA_PORT;
    } else {
        port = PIC2_DATA_PORT;
        IRQline -= 8;
    }
    value = inb(port) & ~(1 << IRQline);
    outb(port, value);        
}
static uint16_t __pic_get_irq_reg(int ocw3)
{
    /* OCW3 to PIC CMD to get the register values.  PIC2 is chained, and
     * represents IRQs 8-15.  PIC1 is IRQs 0-7, with 2 being the chain */
    outb(PIC1_COMMAND_PORT, ocw3);
    outb(PIC2_COMMAND_PORT, ocw3);
    return (inb(PIC2_COMMAND_PORT) << 8) | inb(PIC1_COMMAND_PORT);
}

/* Returns the combined value of the cascaded PICs irq request register */
uint16_t pic_GetIRR(void)
{
    return __pic_get_irq_reg(PIC_READ_IRR);
}

/* Returns the combined value of the cascaded PICs in-service register */
uint16_t PIC_GetISR(void)
{
    return __pic_get_irq_reg(PIC_READ_ISR);
}
