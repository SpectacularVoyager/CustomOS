#include "iso.h"
#include <stdint.h>
#include "../../stdlib/stdio.h"

int startAddr=0x10 * 0x800;

void ISO_Init(){
	uint8_t data[512];
	ATAPIO_ReadBytes(1,0x8000/0x200,(uint16_t*)data);
	kprintf(INFO "TYPE %x\n",data[0]);
	kprintf(INFO "IDENT %5s\n",&data[1]);
	kprintf(INFO "LPATH %x\n",*((uint32_t*)&data[140]));
	kprintf(INFO "MPATH %x\n",*((uint32_t*)&data[148]));
}
