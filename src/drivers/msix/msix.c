#include "msix.h"
#include "utils/bit.h"
#include "stdlib/stdio.h"

void MSI_INIT(void* data,PCIGeneralDevice* device){
	uint32_t d=U32(data);
	if(BYTE(d,0)!=0x11){
		printf("MSIX NOT FOUND:%x\t\n",d);
	}
	uint16_t msgctrl=WORD(d,0);
	unsigned int tableNum=(msgctrl&0x3ff)+1;
	printf("MSGCTRL:\t%x\n",msgctrl);
	printf("TABLE:\t%x\n",U32(data+0x4));
	printf("SIZE:\t%x\n",tableNum);
	uint32_t addr=U32(data+0x4);
	uint32_t pba=U32(data+0x8);
	MSIX_Table* mem=(void*)(BAR_ADDR(device->BAR[addr&0x7])+(addr&(~0x7)));
	uint32_t* mempba=(void*)(BAR_ADDR(device->BAR[pba&0x7])+(pba&(~0x7)));
	printf("MEM:\t%p\t%p\n",mem,mempba);
}
