#include "ext2.h"
#include "drivers/gpt/gpt.h"
#include "stdlib/stdlib.h"
#include "utils/utils.h"

int EXT2_READPART(void* fs,AHCI_HBA_PORT* port,GPT_PART_ENTRY* entry){

	// int lba=0x40;	
	int lba=entry->startLBA+2;	
	EXT2_SUPERBLOCK* superblock=malloc(GPT_SECTOR_SIZE);
	AHCI_READ(port,lba,1,(uint16_t*)superblock);
	if(superblock->magic!=0xEF53)return -1;
	
	//hexdump(superblock,512,0x20);
	return 0;
}
