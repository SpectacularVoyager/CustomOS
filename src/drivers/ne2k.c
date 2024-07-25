#include "8254x.h"
#include "pci.h"
#include "../stdio.h"
#include "ne2k.h"
#include "../io.h"

PCIGeneralDevice* NIC;
PCI_device* base;

uint32_t io_base;
#define IO_ADDR ((uint32_t*)(NIC->BAR[0]))
void NIC_Ne2k_Init(PCI_device* devices){
	base=PCI_GetFromID(0x1234,0x1111);
	PCI_GetGeneralDevice(base,NIC);
	PCI_Device_Print(base);	
	for(int i=0;i<6;i++){
		printf("0X%8X\n",NIC->BAR[i]);
	}
	uint32_t val=IO_ADDR[0];
	io_base=NIC->BAR[0]& 0x3;
	outportl(io_base + 0x1F, inportl(io_base + 0x1F));
	while ((inportl(io_base + 0x07) & 0x80) == 0){
		printf("LOADING\n");
	}
	outb(io_base + 0x07, 0xFF);                   

	uint8_t prom[32];
	outb(io_base, (1 << 5) | 1);	// page 0, no DMA, stop
	outb(io_base + 0x0E, 0x49);		// set word-wide access
	outb(io_base + 0x0A, 0);		// clear the count regs
	outb(io_base + 0x0B, 0);
	outb(io_base + 0x0F, 0);		// mask completion IRQ
	outb(io_base + 0x07, 0xFF);
	outb(io_base + 0x0C, 0x20);		// set to monitor
	outb(io_base + 0x0D, 0x02);		// and loopback mode.
	outb(io_base + 0x0A, 32);		// reading 32 bytes
	outb(io_base + 0x0B, 0);		// count high
	outb(io_base + 0x08, 0);		// start DMA at 0
	outb(io_base + 0x09, 0);		// start DMA high
	outb(io_base, 0x0A);		// start the read

	int i;
	for (i=0; i<32; i++)
	{
		prom[i] = inb(io_base + 0x10);
	};
	for(int i=0;i<6;i++){
		printf("%02X::",prom[i]);
	}
	printf("\n");
}
