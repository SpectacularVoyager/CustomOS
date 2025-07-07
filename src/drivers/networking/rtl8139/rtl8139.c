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
#include "drivers/networking/network.h"

void printMAC(uint8_t* ptr){
	for(int i=0;i<5;i++){
		printf("%02x::",ptr[i]);
	}
	printf("%02x\n",ptr[5]);
}
RTL8139 rtl8139;
uint32_t rtl8139_recv_packet_ptr=0;
RTL8139* nic(){
	return &rtl8139;
}

void RTL8139_SEND(void* data,unsigned long len){
	int tsad=(rtl8139.ioaddr+0x20+rtl8139.tsad*4)&(~0x1);
	int tsd=(rtl8139.ioaddr+0x10+rtl8139.tsad*4)&(~0x1);
	outportl(tsad,(uint32_t)MemoryPhysical(data));
	outportl(tsd,len);
	rtl8139.tsad=(rtl8139.tsad+1)%4;
}
void RTL8139_RECV(){
	// Skip packet header, get packet length
	while(1){
		uint16_t * t = (uint16_t*)(rtl8139.recv + rtl8139.recv_offset);
		uint8_t status = (*((uint8_t*)t));
		if(status!=1){
			kprintf("Malformed Packet\n");
			return;
		}
		uint16_t packet_length = (*(t + 1));
		LOGVALD(packet_length);
		PACKET_ETHERNET2_BEGIN* eth=(void*)(t+2);
		kprintf("(REQ TYPE):%x\n",eth->type);
		khexdump(rtl8139.recv,1024,32);
		khexdump(t,32,64);

		switch(htons(eth->type)){
			case ETH_TYPE_ARP:
				kprintf("ARP\n");
				NetworkOnARP(eth);
				break;
			case ETH_TYPE_IP:
				kprintf("IPv4 recieved\n");
				NetworkOnIPv4(eth);
				break;
			case 0x86dd:
				kprintf("IPv6 recieved\n");
				break;
			default:
				printf("PACKET OF TYPE %x RECIEVED FROM:",eth->type);
				printMAC(eth->mac_src);
		}
		//RECIEVED PACKET
		kprintf("OFFSET:%d\n",rtl8139.recv_offset);
		rtl8139.recv_offset=(rtl8139.recv_offset+packet_length+4+3)&(~3);
		if(rtl8139.recv_offset>512){
			rtl8139.recv-=512;
			kprintf("RESETTING\n");
			outw(rtl8139.ioaddr+0x38,rtl8139.recv_offset-0x10);
		}
	}
}

void NIC_INT(registers* r){
	int status=inw(rtl8139.ioaddr+RTL8139_ISR);
	if(status & 0x20){
	}
	if(status & 0x10){
	}
	if(status & RTL8139_STATUS_TOK) {
		kprintf("PACKET SENT\n");
	}
	if(status & RTL8139_STATUS_ROK){
		RTL8139_RECV();
		//outw(rtl8139.ioaddr+0x38,0x200);
	}
	// LOGVAL(status)
	// if(status & RTL8139_STATUS_TOK) {
	// 	kprintf("Packet sent\n");
	// 	return;
	// }
	// if (status & RTL8139_STATUS_ROK) {
	// 	printf("Received packet\n");
	// 	RTL8139_RECV();
	// }

	outw(rtl8139.ioaddr+RTL8139_ISR,status);
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

	rtl8139.recv_offset=0;

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
	printMAC(rtl8139.header->MAC);

	// RTL8139_SEND(pkt_data, sizeof(pkt_data));
	// RTL8139_SEND(pkt_data, sizeof(pkt_data));
	// RTL8139_SEND(pkt_data, sizeof(pkt_data));
	// RTL8139_SEND(pkt_data, sizeof(pkt_data));
	
	PACKET_ETHERNET2_BEGIN* eth=malloc(1584);
	uint8_t dest[6]={0xe4,0xa8,0xdf,0xe1,0xa8,0x47};
	ETHERNET_default(eth, dest,rtl8139.header->MAC,0x800);
	PACKET_IP* ip=(PACKET_IP*)&eth->data;
	IP_default(ip,IP(192,168,7,1),IP(192,168,7,1), 0,8);
	addUDP(ip,htons(8000),htons(12345),"Hello World123456",18);
	RTL8139_SEND(eth,60);

	// {
	// 	//DHCP
	// 	PACKET_ETHERNET2_BEGIN* eth=malloc(1584);
	// 	uint8_t dest[6]={0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
	// 	ETHERNET_default(eth, dest,rtl8139.header->MAC,0x800);
	// 	PACKET_IP* ip=(PACKET_IP*)&eth->data;
	// 	IP_default(ip,IP(0,0,0,0),IP(255,255,255,255), 0,8);
	// 	PACKET_DHCP* dhcp=malloc(300);
	// 	int len=DHCP_default(dhcp,DHCP_MESSAGE_TYPE_BOOT_REQUEST,rtl8139.header->MAC);
	// 	addUDP(ip,htons(68),htons(67),dhcp,len);
	// 	RTL8139_SEND(eth,14+htons(ip->len));
	// }
}
