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

// 31      19		 11			 3	  2	   1	0
// +--------+---------+----------+----+----+----+
// |  0FEEH | Dest ID | Reserved | RH | DM | XX |
// +--------+---------+----------+----+----+----+
