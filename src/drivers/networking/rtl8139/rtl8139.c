#include "rtl8139.h"
#include "drivers/pci.h"
#include <stdint.h>
#include "stdlib/stdio.h"
#include "stdlib/string.h"
#include "stdlib/stdlib.h"
#include <utils/bit.h>
#include <utils/utils.h>
#include <utils/ports.h>
#include <paging/paging.h>
#include <drivers/apic/apic.h>
#include <interrupts/irq.h>
#include <specifications/net.h>
void printMAC(uint8_t* ptr){
	for(int i=0;i<5;i++){
		printf("%02x::",ptr[i]);
	}
	printf("%02x\n",ptr[5]);
}
RTL8139 rtl8139;
uint32_t rtl8139_packet_ptr=0;

void RTL8139_SEND(void* data,unsigned long len){
	int tsad=(rtl8139.ioaddr+0x20+rtl8139.tsad*4)&(~0x1);
	int tsd=(rtl8139.ioaddr+0x10+rtl8139.tsad*4)&(~0x1);
	outportl(tsad,(uint32_t)MemoryPhysical(data));
	outportl(tsd,len);
	rtl8139.tsad=(rtl8139.tsad+1)%4;
}
void RTL8139_RECV(){
	uint16_t * t = (uint16_t*)(rtl8139.recv + rtl8139_packet_ptr);
    // Skip packet header, get packet length
    uint16_t packet_length = *(t + 1);
	PACKET_ETHERNET2_BEGIN* eth=(void*)(t+2);
	switch(htons(eth->type)){
		case ETH_TYPE_ARP:
			PACKET_ARP* arp=(PACKET_ARP*)eth->data;
			LOGVAL(htonl(U32(ARP_GET_TARGET_PROTOCOL(arp))));

			PACKET_ETHERNET2_BEGIN* eth=malloc(1584);
			ETHERNET_default(eth,
					ARP_GET_SENDER_ADDR(arp), rtl8139.header->MAC,
					ETH_TYPE_ARP);
			PACKET_ARP* response=(PACKET_ARP*)&eth->data;
			memcpy(response,arp,8);
			memcpy(ARP_GET_TARGET_PROTOCOL(response),ARP_GET_SENDER_PROTOCOL(arp),arp->protocol_len);
			memcpy(ARP_GET_TARGET_ADDR(response),ARP_GET_SENDER_ADDR(arp),arp->hardware_len);
			memcpy(ARP_GET_SENDER_PROTOCOL(response),ARP_GET_TARGET_PROTOCOL(arp),arp->protocol_len);
			memcpy(ARP_GET_SENDER_ADDR(response),rtl8139.header->MAC,arp->hardware_len);
			response->operation=htons(2);
			printf("SENDING\n");
			RTL8139_SEND(eth,14+(8+20));
			// PACKET_ETHERNET2_BEGIN* eth=malloc(1584);
			// uint8_t dest[6]={0x3C,0xFD,0xFE,0x9E,0x7F,0x71};
			// ETHERNET_default(eth, rtl8139.header->MAC,dest,0x800);
			// PACKET_IP* ip=(PACKET_IP*)&eth->data;
			// IP_default(ip,IP(127,0,0,1),IP(127,0,0,1), 0,8);
			// addUDP(ip,htons(8000),htons(12345),"Hello World123456",18);
			// hexdump(eth,61,0x10);
			// RTL8139_SEND(eth,61);
			break;
	}
}

void NIC_INT(registers* r){
	int status=inw(rtl8139.ioaddr+RTL8139_ISR);
	printf("INT RECV\n");
	outw(rtl8139.ioaddr+RTL8139_ISR,0x5);
	// LOGVAL(status)
	// if(status & RTL8139_STATUS_TOK) {
	// 	kprintf("Packet sent\n");
	// 	return;
	// }
	// if (status & RTL8139_STATUS_ROK) {
	// 	printf("Received packet\n");
	// 	RTL8139_RECV();
	// }
	APIC_SEND_EOI();
}
//https://medium.com/@4yub1k/art-of-manually-crafting-packets-ip-icmp-a180a16c7f08
void RTL8139_INIT(PCI_device* device,void* base){
	rtl8139.tsad=0;
	rtl8139.device=device;
	SetColor(0xd48506);
	PCIGeneralDevice nic;
	PCI_GetGeneralDevice(device,&nic);
	rtl8139.mmio=PCI_GetMMIO(device,base);

	void* config=PCI_GetMMIO(device,base);
	void* data=(uint32_t*)nic.BAR[1];
	rtl8139.ioaddr=nic.BAR[0]&(~0xF);
	uint8_t* macaddr=data+RTL8139_MAC;
	rtl8139.header=data;
	device->command|=PCI_COMMAND_BUS_MASTERING;
	PCI_DeviceConfigWriteWord(device,0x4,
			PCI_DeviceConfigReadWord(device,0x4)|PCI_COMMAND_BUS_MASTERING
			);
	printf("INITIALIZING RTL8139\n");
	//TURN ON
	outb(rtl8139.ioaddr+0x52,0x0);

	//SOFTWARE RESET 
	outb(rtl8139.ioaddr + 0x37, 0x10);
	while((inb(rtl8139.ioaddr + 0x37) & 0x10) != 0) { }

	printf("RTL8139 SOFTWARE RESET OVER\n");
	//RECV STUFF
	rtl8139.recv=(mallocA(8192 + 16 + 1500,0x1000));
	//MEMORY PHYSICAL
	outportl(rtl8139.ioaddr + 0x30,(uint32_t)MemoryPhysical(rtl8139.recv));
	//IMR + ISR 
	outw(rtl8139.ioaddr + 0x3C,0x5);

	//RCR
	outportl(rtl8139.ioaddr + 0x44, 0xf | (1 << 7)); // (1 << 7) is the WRAP bit, 0xf is AB+AM+APM+AAP
	
	//ENABLE RX TX
	outb(rtl8139.ioaddr + 0x37, 0x0C); // Sets the RE and TE bits high
	
	LOGVAL(nic.interrupt_line)
	IRQ_RegisterHandler(nic.interrupt_line,NIC_INT);

	printf("HEY\n");

	// RTL8139_SEND(pkt_data, sizeof(pkt_data));
	// RTL8139_SEND(pkt_data, sizeof(pkt_data));
	// RTL8139_SEND(pkt_data, sizeof(pkt_data));
	// RTL8139_SEND(pkt_data, sizeof(pkt_data));
	
	PACKET_ETHERNET2_BEGIN* eth=malloc(1584);
	uint8_t dest[6]={0x3C,0xFD,0xFE,0x9E,0x7F,0x71};
	ETHERNET_default(eth, rtl8139.header->MAC,dest,0x800);
	PACKET_IP* ip=(PACKET_IP*)&eth->data;
	IP_default(ip,IP(127,0,0,1),IP(127,0,0,1), 0,8);
	addUDP(ip,htons(8000),htons(12345),"Hello World123456",18);
	RTL8139_SEND(eth,61);
	
}
