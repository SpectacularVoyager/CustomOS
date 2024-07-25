#include "../io.h"
#include "pci.h"
#include "../stdio.h"

typedef struct {
	PCIGeneralDevice* NIC;
	PCI_device* base;
	uint32_t io_addr;
	uint8_t rx_buffer[8192 + 16];
}NIC_8254X;
void NIC_8254X_Init(PCI_device* devices,NIC_8254X* rtl);
