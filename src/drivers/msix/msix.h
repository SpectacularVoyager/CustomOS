#include "stdint.h"
#include "drivers/pci.h"

#define MSIXCAPID(addr) ((*(uint32_t*)(addr))&0xff)

void MSI_INIT(void* data,PCIGeneralDevice* device);

typedef struct{
	uint32_t addrlow;
	uint32_t addrhigh;
	uint32_t data;
	uint32_t vector;
} __attribute__((packed)) MSIX_Table;

#define MSI_X_ENABLED	15
#define MSI_ENABLED		0

#define MSI_X_CAP_SIG	0x11
#define MSI_CAP_SIG		0x05

// 31      19		 11			 3	  2	   1	0
// +--------+---------+----------+----+----+----+
// |  0FEEH | Dest ID | Reserved | RH | DM | XX |
// +--------+---------+----------+----+----+----+

typedef struct{
	uint32_t IMAN;
	uint32_t IMOD;
	uint32_t ERSTSZ;
	uint32_t res1;
	uint64_t ERSTBA;
	uint64_t ERDP;
}__attribute__((packed)) XHCI_INT_RUNTIME_REG;

#define XHCI_RT_IMAN_IE	1<<0x1
