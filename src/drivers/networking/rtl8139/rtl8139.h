#include "drivers/pci.h"

#define RTL8139_MAC			0x0
#define RTL8139_RX_BUFFER	0x30
#define RTL8139_CR			0x37
#define RTL8139_CONFIG1		0x52

void RTL8139_INIT(PCI_device* device,void* base);
