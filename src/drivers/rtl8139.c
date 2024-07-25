#include "rtl8139.h"
#include "pci.h"
#include <stdint.h>

PCIGeneralDevice* RTL_NIC;
PCI_device* RTL_Base;

//uint32_t io_addr;
//#define IO_ADDR ((uint32_t*)(NIC->BAR[0]))

void RTL_8139_Init(PCI_device* devices){
	RTL_Base=PCI_GetFromID(0x10EC,0x8139);
	PCI_GetGeneralDevice(RTL_Base,RTL_NIC);
	uint32_t ioaddr= RTL_NIC->BAR[0] & (~0x3);
	PCI_Device_Print(RTL_Base);	
	printf("COMMAND: 0X%08X\n",RTL_Base->command);
	PCI_DeviceConfigWriteWord(RTL_Base,PCI_COMMAND,RTL_Base->command|4);
	PCI_Refresh(RTL_Base);
	printf("COMMAND: 0X%08X\n",RTL_Base->command);
	for(int i=0;i<6;i++){
		printf("\tBAR[%d]:\t0X%8X\n",i,RTL_NIC->BAR[i]);
	}
	outb(ioaddr+ 0x52, 0x0);
	outb( ioaddr + 0x37, 0x10);
	uint8_t in;
	while(1) { 
		in=inb(ioaddr + 0x37);
		if((in &0x10)==0){
			break;
		}
		//printf("%d\n",in);
	}
	printf("DONE\n");
}
