#include "stdint.h"
#define PIC1_COMMAND_PORT	0x20
#define PIC1_DATA_PORT		0x21
#define PIC2_COMMAND_PORT	0xA0
#define PIC2_DATA_PORT		0xA1

#define PIC_ICW1_ICW4	0x01		/* Indicates that ICW4 will be present */
#define PIC_ICW1_SINGLE	0x02		/* Single (cascade) mode */
#define PIC_ICW1_INTERVAL4	0x04		/* Call address interval 4 (8) */
#define PIC_ICW1_LEVEL	0x08		/* Level triggered (edge) mode */
#define PIC_ICW1_INIT	0x10		/* Initialization - required! */

#define PIC_ICW4_8086	0x01		/* 8086/88 (MCS-80/85) mode */
#define PIC_ICW4_AUTO	0x02		/* Auto (normal) EOI */
#define PIC_ICW4_BUF_SLAVE	0x08		/* Buffered mode/slave */
#define PIC_ICW4_BUF_MASTER	0x0C		/* Buffered mode/master */
#define PIC_ICW4_SFNM	0x10		/* Special fully nested (not) */

#define PIC_READ_IRR                0x0a    /* OCW3 irq ready next CMD read */
#define PIC_READ_ISR                0x0b    /* OCW3 irq service next CMD read */

#define PIC_EOI		0x20		/* End-of-interrupt command code */

void PIC_Initialize(uint16_t offset1,uint16_t offset2);

void PIC_Disable(void);

void IRQ_SetMask(uint8_t IRQline);

void IRQ_ClearMask(uint8_t IRQline);

uint16_t pic_GetIRR(void);
uint16_t PIC_GetISR(void);

void PIC_SendEOI(uint8_t irq);
