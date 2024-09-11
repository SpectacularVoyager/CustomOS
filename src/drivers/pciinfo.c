#include "../stdlib/stdio.h"

char* PCI_INFO_VENDOR(int vendor){
	if(vendor==0x8086){
		return "Intel Corporation";
	}
	if(vendor==0x10EC){
		return " Realtek Semiconductor Co., Ltd.";
	}
	return 0;
}
char* PCI_INFO_DEVICE(int vendor,int device){
	if(vendor==0x8086){
		
	}
	if(vendor==0x10EC){
	}
	return 0;
}
