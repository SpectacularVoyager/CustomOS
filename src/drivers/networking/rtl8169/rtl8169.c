#include "rtl8169.h"
#include "stdlib/stdio.h"

void RTL8169_INIT(PCI_device* device){
	PCIGeneralDevice nic;
	PCI_GetGeneralDevice(device,&nic);
}
