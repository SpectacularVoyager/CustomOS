#include "rtl8139.h"
#include "drivers/pci.h"
#include <stdint.h>
#include "stdlib/stdio.h"
#include "stdlib/stdlib.h"
#include <utils/bit.h>
#include <utils/utils.h>
#include <utils/ports.h>
#include <paging/paging.h>
#include <drivers/apic/apic.h>
#include <interrupts/irq.h>
void printMAC(uint8_t* ptr){
	for(int i=0;i<5;i++){
		printf("%02x::",ptr[i]);
	}
	printf("%02x\n",ptr[5]);
}
RTL8139 rtl8139;

void NIC_INT(registers* r){
	printf("NIC\n");
}
void RTL8139_SEND(void* data,unsigned long len){
	outportl(rtl8139.ioaddr+0x10+rtl8139.tsad*4,(uint32_t)data);
	outportl(rtl8139.ioaddr+0x10+rtl8139.tsad*4,len);
	rtl8139.tsad=(rtl8139.tsad+1)%4;
}
//https://medium.com/@4yub1k/art-of-manually-crafting-packets-ip-icmp-a180a16c7f08
void RTL8139_INIT(PCI_device* device,void* base){
	rtl8139.tsad=0;
	rtl8139.device=device;
	// hexdump(base,6,6);
	SetColor(0xd48506);
	PCIGeneralDevice nic;
	PCI_GetGeneralDevice(device,&nic);
	rtl8139.mmio=PCI_GetMMIO(device,base);

	void* config=PCI_GetMMIO(device,base);
	void* data=(uint32_t*)nic.BAR[1];
	rtl8139.ioaddr=nic.BAR[0];
	uint8_t* macaddr=data+RTL8139_MAC;
	RTL8139_HEADER* header=data;
	device->command|=PCI_COMMAND_BUS_MASTERING;

	//TURN ON
	outb(rtl8139.ioaddr+0x52,0x0);

	//SOFTWARE RESET 
	outb(rtl8139.ioaddr + 0x37, 0x10);
	while((inb(rtl8139.ioaddr + 0x37) & 0x10) != 0) { }

	//RECV STUFF
	void* recieve=mallocA(8192+16,0x1000);
	//MEMORY PHYSICAL
	outportl(rtl8139.ioaddr + 0x30,(uint32_t)recieve);
	//IMR + ISR 
	outw(rtl8139.ioaddr + 0x3C,0x5);

	//RCR
	outportl(rtl8139.ioaddr + 0x44, 0xf | (1 << 7)); // (1 << 7) is the WRAP bit, 0xf is AB+AM+APM+AAP
	
	//ENABLE RX TX
	outb(rtl8139.ioaddr + 0x37, 0x0C); // Sets the RE and TE bits high
	
	LOGVAL(nic.interrupt_line)
	IRQ_RegisterHandler(nic.interrupt_line,NIC_INT);

	printMAC(header->MAC);

	void* hello="Hello World\n";
	RTL8139_SEND(hello, 13);
	
}
