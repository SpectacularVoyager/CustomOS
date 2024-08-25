#include "atapio.h"
#include "../../utils/ports.h"
#include "../../utils/bit.h"
#include "../../stdlib/stdio.h"
#include <stdint.h>
uint16_t atapio_identify[256];
uint16_t data[256];
int atapio_lba48;
uint16_t convertFromPort(uint16_t x){

		return ((x&0xff)<<8)|((x>>8)&0xff);
}
unsigned int getLBA28Addresses(){
	return (data[61]<<16)|data[60];
	//kprintf("%")
}
unsigned long getLBA48Addresses(){
	uint16_t a1=data[100];
	uint16_t a2=data[101];
	uint16_t a3=data[102];
	uint16_t a4=data[103];
	unsigned long ret=a1|(unsigned long)a2<<16|(unsigned long)a3<<32|(unsigned long)a4<<48;
	return ret;
	//kprintf("%")
}
void PrintData(){
	int c=16;
	for(int i=0;i<256/c;i++){
		kprintf("\t");
		for(int j=0;j<c;j++){
			kprintf("0X%04X\t",data[i*16|j]);
		}
		kprintf("\n");
	}
}
void readStuff(uint16_t sectorcount,uint64_t lba){
	outb(ATAPIO_DRIVE_SELECT_PORT,0x40);
	outb(ATAPIO_SECTOR_COUNT_PORT,BYTE(sectorcount,1));
	outb(ATAPIO_LBA_LOW_PORT , BYTE(lba,3));
	outb(ATAPIO_LBA_MID_PORT , BYTE(lba,4));
	outb(ATAPIO_LBA_HIGH_PORT, BYTE(lba,5));
	outb(ATAPIO_SECTOR_COUNT_PORT,BYTE(sectorcount,0));
	outb(ATAPIO_LBA_LOW_PORT , BYTE(lba,0));
	outb(ATAPIO_LBA_MID_PORT , BYTE(lba,1));
	outb(ATAPIO_LBA_HIGH_PORT, BYTE(lba,2));
	outb(ATAPIO_COMMAND_PORT,ATAPIO_READ_SECTORS_EXT);
}
void ATAPIO_HANDLE_IRQ(registers* r){
	kprintf(TRACE "HARD DISK INT\n");
	for(int i=0;i<256;i++){
		//data[i]=inw(ATAPIO_DATA_PORT);
		int x=inw(ATAPIO_DATA_PORT);
		data[i]=convertFromPort(x);
	}
	PrintData();
}

void ATAPIO_Identify(int target){
	kprintf(INFO "IDENTIFYING ATAPIO\n");
	outb(ATAPIO_DRIVE_SELECT_PORT,target);
	outb(ATAPIO_SECTOR_COUNT_PORT,0);

	outb(ATAPIO_SECTOR_COUNT_PORT,0);
	outb(ATAPIO_LBA_LOW_PORT,0);
	outb(ATAPIO_LBA_MID_PORT,0);
	outb(ATAPIO_LBA_HIGH_PORT,0);

	outb(ATAPIO_COMMAND_PORT,ATAPIO_IDENTIFY_COMMAND);
	int in=inb(ATAPIO_COMMAND_PORT);
	//kprintf(INFO "READ BYTE %x\n",in);
	while(in&ATAPIO_STATUS_BUSY){
		int LBA_mid,LBA_high;
		if(inb(LBA_mid)){
			kprintf(ERROR "THE DEVICE IS NOT ATAPIO LBA MID IS NON ZERO %x\n",LBA_mid);
		}
		if(inb(LBA_high)){
			kprintf(ERROR "THE DEVICE IS NOT ATAPIO LBA HIGH IS NON ZERO %x\n",LBA_high);
		}
		in=inb(ATAPIO_COMMAND_PORT);
	}
	while(!(in&ATAPIO_STATUS_DRQ)|!(in&ATAPIO_STATUS_DRQ)){
		in=inb(ATAPIO_COMMAND_PORT);
	}
	if(in&ATAPIO_STATUS_ERROR){
		kprintf(ERROR "ERROR IDENITFIYING ATAPIO\n");
		return;
	}
	kprintf(INFO "ATAPIO DETECTED\n");
	kprintf(TRACE "READING VALUES\n");
	//for(int i=0;i<256;i++){
	//	atapio_identify[i]=inw(ATAPIO_DATA_PORT);
	//}
	kprintf(TRACE "READ 256 VALUES\n");
	kprintf(INFO "CAN READ %x LBA28 Addresses\n",getLBA28Addresses());
	kprintf(INFO "CAN READ %x LBA48 Addresses\n",getLBA48Addresses());
	if(BIT(atapio_identify[83],10)){
		kprintf(INFO "LBA 48 SUPPORTED\n");
		atapio_lba48=1;
	}
	//LBA SELECT
	//outb(ATAPIO_DRIVE_SELECT_PORT,ATAPIO_MASTER_SELECT|1<<6);
	readStuff(0,0);	
}
