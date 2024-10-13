#include "msix.h"
#include "drivers/pci.h"
#include "utils/bit.h"
#include "stdlib/stdio.h"
#define ADDR_FROM_BIR(BAR,a) (void*)(BAR_ADDR(BAR[a&0x7])+(a&(~0x7)))

void handleMSIX(void* data,PCIGeneralDevice* device){
	volatile uint32_t d=U32(data);
	*(uint32_t*)data|=1<<31;
	d=U32(data);
	uint16_t msgctrl=WORD(d,1);
	unsigned int tableNum=(msgctrl&0x3ff)+1;
	printf("\tMSGCTRL:\t%x\n",msgctrl);
	printf("\tTABLE:\t%x\n",U32(data+0x4));
	printf("\tSIZE:\t%x\n",tableNum);
	uint32_t addr=U32(data+0x4);
	uint32_t pba=U32(data+0x8);
	MSIX_Table* mem=ADDR_FROM_BIR(device->BAR,addr);
	MSIX_Table* mempba=ADDR_FROM_BIR(device->BAR,pba);
	printf("\tMEM:\t%p\t%p\n",mem,mempba);
	mem[0].vector=0;
	mem[0].addrlow=(0xFEE<<20);
	mem[0].addrhigh=0;
	mem[0].data=0x2A;
}
void MSI_INIT(void* data,PCIGeneralDevice* device){
	SetColor(0x13fc03);
	while(1){
		uint32_t d=U32(data);
		if(BYTE(d,0)==MSI_X_CAP_SIG){
			printf("MSI-X DETECTED\t ENABLED=%x\n",BIT(WORD(d,1),MSI_X_ENABLED));
			handleMSIX(data,device);
			int off=BYTE(d,1);
			data=(void*)(((uint64_t)data&(~0xFF))|off);
			if(off==0x0)break;
		}else if(BYTE(d,0)==MSI_CAP_SIG){
			printf("MSI DETECTED\t ENABLED=%x\n",BIT(WORD(d,1),MSI_ENABLED));
			int off=BYTE(d,1);
			data=(void*)(((uint64_t)data&(~0xFF))|off);
			if(off==0x0)break;
		}else{
			int off=BYTE(d,1);
			data=(void*)(((uint64_t)data&(~0xFF))|off);
			printf("UNRECOGNIZED CAPABILITY [%x]\n",BYTE(d,0));
			if(off==0x0)break;
		}
	}
	SetColor(0xff0000);
}
