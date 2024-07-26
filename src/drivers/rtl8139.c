#include "rtl8139.h"
#include "pci.h"
#include <stdint.h>


void RTL_8139_Init(PCI_device* devices,RTL_8139* rtl){
	RTL_8139 device;
	rtl->base=PCI_GetFromID(0x10EC,0x8139);
	if(!rtl->base){
		printf("DEVICE RTL_8139 NOT FOUND\n");
		return;
	}
	PCI_GetGeneralDevice(rtl->base,rtl->NIC);
	uint32_t ioaddr= rtl->NIC->BAR[0] & (~0x3);
	rtl->io_addr=ioaddr;

	//PRINT DEVICE
	//PCI_DeviceConfigWriteWord(rtl->base,PCI_COMMAND,rtl->base->command|4);
	PCI_Refresh(rtl->base);
	PCI_Device_Print(rtl->base);	
	printf("\tINTERUPT:\t%d\n",rtl->NIC->interrupt_line);

	//RESET
	outb(ioaddr+ 0x52, 0x0);
	outb( ioaddr + 0x37, 0x10);
	uint8_t in;
	while(1) { 
		in=inb(ioaddr + 0x37);
		if((in &0x10)==0){
			break;
		}
	}
	printf("DONE:\n");

	for(int i=0;i<6;i++){
		printf("%2x::",inb(ioaddr+i));
	}
	printf("\n");
	//CHECK FOR BUGS
	outportl(ioaddr + 0x30, (uintptr_t)rtl->rx_buffer);
	outportw(ioaddr + 0x3C, 0x0005); // Sets the TOK and ROK bits high
	outportl(ioaddr + 0x44, 0xf | (1 << 7)); // (1 << 7) is the WRAP bit, 0xf is AB+AM+APM+AAP
	outb(ioaddr + 0x37, 0x0C); // Sets the RE and TE bits high
							   //
	outportl(ioaddr+0x20,((0x1)<<21)|0x100);
	printf("0x%x",inportl(ioaddr+0x20));
}
