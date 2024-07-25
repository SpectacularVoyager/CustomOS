
#include "../io.h"
#include "pci.h"
#include "../stdio.h"

typedef struct {
	PCIGeneralDevice* NIC;
	PCI_device* base;
	uint32_t io_addr;
	uint8_t rx_buffer[8192 + 16];
}RTL_8139;
void RTL_8139_Init(PCI_device* devices,RTL_8139* rtl);
