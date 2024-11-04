#include "msix.h"
#include "drivers/pci.h"
#include "utils/bit.h"
#include "utils/utils.h"
#include "stdlib/stdio.h"
#define ADDR_FROM_BIR(BAR,a) (void*)(BAR_ADDR(BAR[a&0x7])+(a&(~0x7)))

unsigned long arch_msi_address(uint32_t* data, size_t vector, uint32_t processor, uint8_t edgetrigger, uint8_t deassert)
{
	*data = (vector & 0xFF) | (edgetrigger == 1 ? 0 : (1 << 15)) | (deassert == 1 ? 0 : (1 << 14));
	return (0xFEE00000 | (processor << 12));
}
//#define MSI_DEBUG

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
	unsigned int msi_data=0;
	uint64_t msi_addr = arch_msi_address(&msi_data, 0x2B, 0,1,0);
	FORI(maxintrs){
		mem[i].addrlow=(uint32_t)msi_addr&(~0x3);
		mem[i].addrhigh=(uint32_t)(msi_addr>>32);
		mem[i].data=msi_data;
		mem[i].vector=0;
	}
#ifdef MSI_DEBUG
	printf("MSI CONFIG:\t%x\n",data);
	printf("MSI ADDRESS:\t%x\n",msi_addr);
	printf("MSI DATA:\t%x\n",msi_data);
#endif
}
void MSI_HANDLE_CAPABILITY(void* data,PCIGeneralDevice* device,unsigned int maxintrs){
	U32(data)&=(~(0b1110001<<16));
	int vecs=0b01;
	U32(data)|=MSI_CONTROL(vecs, 1)<<16;
	int bit64=MSI_64_BIT(WORD(U32(data),1));
	int masking=MSI_PER_VECTOR_MASKING(WORD(U32(data),1));

	uint32_t* dataptr=data+0x8;
	if(bit64){
		dataptr+=0x1;
	}
	uint64_t address=arch_msi_address(dataptr, 0x2A, 0,1,0);
	U32(data+0x4)=DWORD(address, 0);
	if(bit64) U32(data+0x8)=DWORD(address,1);
	
	if(masking){
		U32(dataptr+0x4)=0;
	}
#ifdef MSI_DEBUG
	printf("MSI CONFIG:\t%x\n",data);
	printf("MSI ADDRESS:\t%x\n",address);
	printf("MSI DATA:\t%x\n",*dataptr);
#endif

}
