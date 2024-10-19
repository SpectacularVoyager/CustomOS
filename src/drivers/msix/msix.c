#include "msix.h"
#include "drivers/pci.h"
#include "utils/bit.h"
#include "utils/utils.h"
#include "stdlib/stdio.h"
#define ADDR_FROM_BIR(BAR,a) (void*)(BAR_ADDR(BAR[a&0x7])+(a&(~0x7)))

unsigned long arch_msi_address(uint64_t* data, size_t vector, uint32_t processor, uint8_t edgetrigger, uint8_t deassert)
{
	*data = (vector & 0xFF) | (edgetrigger == 1 ? 0 : (1 << 15)) | (deassert == 1 ? 0 : (1 << 14));
	return (0xFEE00000 | (processor << 12));
}

void MSIX_HANDLE_CAPABILITY(void* data,PCIGeneralDevice* device,unsigned int maxintrs){
	volatile uint32_t d=U32(data);
	*(uint32_t*)data|=1<<31;
	d=U32(data);
	uint16_t msgctrl=WORD(d,1);
	unsigned int tableNum=(msgctrl&0x3ff)+1;
	kprintf("\tMSGCTRL:\t%x\n",msgctrl);
	kprintf("\tTABLE:\t%x\n",U32(data+0x4));
	kprintf("\tSIZE:\t%x\n",tableNum);
	uint32_t addr=U32(data+0x4);
	uint32_t pba=U32(data+0x8);
	MSIX_Table* mem=ADDR_FROM_BIR(device->BAR,addr);
	MSIX_Table* mempba=ADDR_FROM_BIR(device->BAR,pba);
	kprintf("\tMEM:\t%p\t%p\n",mem,mempba);
	//mem[0].vector=0;
	//mem[0].addrlow=(0xFEE<<20);
	//mem[0].addrhigh=0;
	//mem[0].data=0x2A;
	unsigned long msi_data=0;
	uint64_t msi_addr = arch_msi_address(&msi_data, 0x2B, 0,1,0);
	FORI(maxintrs){
		mem[i].addrlow=(uint32_t)msi_addr&(~0x3);
		mem[i].addrhigh=(uint32_t)(msi_addr>>32);
		mem[i].data=msi_data;
		mem[i].vector=0;
	}

}
