#include "atapio.h"
#include "../io.h"
#include "../stdio.h"
#include "../irq.h"

void ATAPIO_Identify(){
	outb(0x1f6,0xA0);

	outb(0x1f2,0x00);
	outb(0x1f3,0x00);
	outb(0x1f4,0x00);
	outb(0x1f5,0x00);

	outb(0x1f7,0xEC);

	uint8_t in=inb(0x1f7);
	if(in==0){
		printf("THE DRIVE DOES NOT EXIST\n");
	}else{
		while(1){
			in=inb(0x1f7);
			if((in&ATAPIO_STATUS_BUSY)==0){
				break;
			}
		}
	}
	uint8_t LBA_mid=inb(0x1f4);
	uint8_t LBA_high=inb(0x1f5);
	printf("0x%02X%02X\n",LBA_high,LBA_mid);
	if(LBA_mid==0||LBA_high==0){
		printf("The disk is not ATA\n");
		return;
	}
}

void ATA_PrimaryDiskHandler(Registers* r){
	printf("IRQ 14\n");
}
void ATAPIO_AttachIRQHandler(){
	IRQ_RegisterHandler(14,ATA_PrimaryDiskHandler);
}
