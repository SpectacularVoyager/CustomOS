#include "rtl8139.h"
#include "drivers/pci.h"
#include <stdint.h>
#include "stdlib/stdio.h"
#include "stdlib/stdlib.h"
#include <utils/bit.h>
void printMAC(uint8_t* ptr){
	for(int i=0;i<5;i++){
		printf("%02x::",ptr[i]);
	}
	printf("%02x\n",ptr[5]);
}

void RTL8139_INIT(PCI_device* device,void* base){
	SetColor(0xd48506);
	PCIGeneralDevice nic;
	PCI_GetGeneralDevice(device,&nic);

	for(int i=0;i<6;i++){
		printf(INFO"BAR%d\t%x\n",i,nic.BAR[i]);
	}

	void* config=PCI_GetMMIO(device,base);
	void* data=(uint32_t*)nic.BAR[1];
	uint8_t* macaddr=data+RTL8139_MAC;
	printMAC(macaddr);
	U8(data+RTL8139_CONFIG1)=0;
	U8(data+RTL8139_CR)=0x10;
	while((U8(data+RTL8139_CR)&0x10)!=0){}
	printf("DONE\n");
	void* rx=mallocA(8192+16,8192);
	U32(data+RTL8139_RX_BUFFER)=(uint32_t)rx;
	U16(data+0x3C)=0x0005;
	U32(data+0x44)=0xf|(1<<7);
	U8(data+RTL8139_CR)=0x0C;

	
}
