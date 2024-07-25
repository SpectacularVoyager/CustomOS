#include "8254x.h"
#include "pci.h"
#include <stdint.h>
#include <stdbool.h>
void NIC_WRITE(uint32_t addr,uint32_t val){
	*(uint32_t *)(addr ) = val; // writes "val" to an MMIO address
}
uint32_t NIC_READ(uint32_t addr){
	return *(uint32_t *)(addr); // reads "val" from an MMIO address
}
bool NIC_EEPROM_EXISTS(uint32_t ioaddr);

uint32_t NIC_EEPROM_READ(uint32_t ioaddr,uint8_t addr);

void NIC_8254X_Init(PCI_device* devices,NIC_8254X* nic){
	NIC_8254X device;
	nic->base=PCI_GetFromID(0x8086,0x100E);
	if(!nic->base){
		printf("DEVICE 8254X NOT FOUND\n");
		return;
	}
	PCI_GetGeneralDevice(nic->base,nic->NIC);
	uint32_t ioaddr= nic->NIC->BAR[0] & (~0x3);
	nic->io_addr=ioaddr;

	//PRINT DEVICE
	PCI_DeviceConfigWriteWord(nic->base,PCI_COMMAND,nic->base->command|4);
	PCI_Refresh(nic->base);
	PCI_Device_Print(nic->base);	
	printf("\tINTERUPT:\t%d\n",nic->NIC->interrupt_line);
	uint32_t val = NIC_READ(ioaddr); // reads "val" from an MMIO address
	NIC_WRITE(ioaddr,val|0x4000000);
	printf("\tBAR:\t0X%08X",nic->NIC->BAR[0]);

	printf("EEPROM:%d\n",NIC_EEPROM_EXISTS(ioaddr));
	uint8_t mac[6];
	uint32_t temp;
	temp = NIC_EEPROM_EXISTS( 0);
	mac[0] = temp &0xff;
	mac[1] = temp >> 8;
	temp = NIC_EEPROM_EXISTS( 1);
	mac[2] = temp &0xff;
	mac[3] = temp >> 8;
	temp = NIC_EEPROM_EXISTS( 2);
	mac[4] = temp &0xff;
	mac[5] = temp >> 8;

	for(int i=0;i<6;i++){
		printf("%2x::",inb(ioaddr+i));
	}
	printf("\n");
}
bool NIC_EEPROM_EXISTS(uint32_t ioaddr){
	bool eeprom_exists;
	NIC_WRITE(ioaddr+0x14, 0x1); 
    for(int i = 0; i < 1000 && ! eeprom_exists; i++)
    {
            //uint32_t val = NIC_READ(ioaddr+0x14);
            uint32_t val = NIC_READ(ioaddr+0x14);
            if(val & 0x10)
                    eeprom_exists = true;
            else
                    eeprom_exists = false;
    }
    return eeprom_exists;
}
uint32_t NIC_EEPROM_READ(uint32_t ioaddr,uint8_t addr){
	uint16_t data = 0;
	uint32_t tmp = 0;
	NIC_WRITE( ioaddr+0x14, (1) | ((uint32_t)(addr) << 8) );
	while( !((tmp = NIC_READ(0x14+ioaddr)) & (1 << 4)) );
	return (uint16_t)((tmp >> 16) & 0xFFFF);
}
