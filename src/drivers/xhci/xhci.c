#include "xhci.h"
#include "drivers/pci.h"
#include "stdlib/stdio.h"
#include <stddef.h>

void XHCI_INIT(PCI_device* device,void* pcibase){
	PCIGeneralDevice usb;
	PCI_GetGeneralDevice(device,&usb);
	printf("STATUS ->%x\n",usb.capabilities_pointer);

	void* mmio=PCI_GetMMIO(device,pcibase);
	XHCI_CAP_REG* config=(mmio+usb.capabilities_pointer);
	SetColor(0xff0068);
	printf("CAPLENGTH:\t%x\n",config->CAPLENGTH);
	printf("VERSION:\t%x\n",config->HCIVersion);
	printf("VERSION:\t%x\n",config->HCSParams1);
	printf("VERSION:\t%x\n",config->HCSParams2);

	PCIGeneralDevice* temp=(mmio-6);

	printf("BAR0:\t%x\n",usb.base.headerType);	
	printf("BAR0:\t%x\n",usb.capabilities_pointer);	
	printf("BAR0:\t%x\n",temp->capabilities_pointer);	

	SetColor(0xffffff);
}
