#include "iso.h"
#include <stdint.h>
#include "../../stdlib/stdio.h"
#include "../../stdlib/string.h"
#include "../../utils/bit.h"

GPTPartitionEntry* entries[4];
void printPartitonName(char* name,int size){
	for(int i=0;i<size;i++){
		printf("%c",name[i*2]);
	}
	printf("\n");
}
void ISO_Init(){
	uint8_t data[512];
	ATAPIO_ReadBytes(1,1,(uint16_t*)data);
	GPTHeader header;
	memcpy(&header,data,sizeof(GPTHeader));
	printf(INFO "FOUND %d partitions at LBA %d\n",header.num_partition_entries,header.lba_partition_entries);
	ATAPIO_ReadBytes(1,header.lba_partition_entries,(uint16_t*)data);

	//HARD CODED
	for(volatile int i=0;i<4;i++){
		entries[i]=(GPTPartitionEntry*)(data+i*header.size_partition_entry);
		//kprintf(TRACE"EY\n");
	}
	for(int i=0;i<4;i++){
		printf_(INFO"NAME:\t");
		printPartitonName(entries[i]->name,36);
		printf_(INFO"LBA 0x%x\n",entries[i]->startingLBA);
	}
	ATAPIO_ReadBytes(1,entries[1]->startingLBA,(uint16_t*)data);
	fat_BS_t* fat=(fat_BS_t*)data;
	printf(INFO"OEM:\t%08s\n",fat->oem_name);
	printf(INFO"OFFSET:\t%x\n",fat->reserved_sector_count);


	long rootDirSectors=DIV_CEIL(fat->root_entry_count*32,fat->bytes_per_sector);
	long fatSecNum=fat->reserved_sector_count;
}
