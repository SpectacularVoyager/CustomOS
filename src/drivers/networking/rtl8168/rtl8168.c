#include "rtl8168.h"
#include "../../../stdlib/stdio.h"
void p(uint8_t* address){

	for(int i=0;i<8;i++){
		for(int j=0;j<32;j++){
			printf("%02x ",(address)[i*32+j]);
		}
		printf("\n");
	}
	printf("\n\n");
}
void RTL8168_INIT(PCI_device* device){
	PCIGeneralDevice nic;
	PCI_GetGeneralDevice(device,&nic);
	for(int i=0;i<6;i++){
		printf(INFO"BAR%d\t%x\n",i,nic.BAR[i]);
	}
	void* addr=(void*)nic.BAR[4];
	char* ptr=(char*)addr;
	uint64_t* mac=(uint64_t*)addr;
	printf(INFO "ADDRESS %p\n\n",addr);
	printf("%p\n",*mac);
	//for(int i=0;i<6;i++){
	//	//p((uint8_t*)nic.BAR[i]);
	//}
}
