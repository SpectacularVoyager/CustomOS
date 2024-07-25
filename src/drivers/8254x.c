#include "8254x.h"
#include "pci.h"
#include "../stdio.h"
#include "../io.h"
#include <stdbool.h>


PCIGeneralDevice* NIC;
PCI_device* base;

uint32_t io_addr;
#define IO_ADDR ((uint32_t*)(NIC->BAR[0]))

void NIC_8254X_Init(PCI_device* devices){
	base=PCI_GetFromID(0x8086,0x100E);
	PCI_GetGeneralDevice(base,NIC);
	PCI_Device_Print(base);	
	for(int i=0;i<6;i++){
		printf("0X%8X\n",NIC->BAR[i]);
	}
}
