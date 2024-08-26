#include "iso.h"
#include <stdint.h>
#include "../../stdlib/stdio.h"
#include "../../stdlib/string.h"

void printPartitonName(char* name,int size){
	for(int i=0;i<size;i++){
		kprintf("%c",name[i*2]);
	}
	kprintf("\n");
}
void ISO_Init(){
	uint8_t data[512];
	ATAPIO_ReadBytes(1,1,(uint16_t*)data);
	GPTHeader header;
	memcpy(&header,data,sizeof(GPTHeader));
	kprintf(INFO "FOUND %d partitions at LBA %d\n",header.num_partition_entries,header.lba_partition_entries);
	ATAPIO_ReadBytes(1,header.lba_partition_entries,(uint16_t*)data);
	//HARD CODED
	for(int i=0;i<4;i++){
		GPTPartitionEntry* entry=(GPTPartitionEntry*)(data+i*header.size_partition_entry);
		kprintf_(INFO"NAME:\t");
		printPartitonName(entry->name,36);
		kprintf_(INFO"LBA 0x%x\n",entry->startingLBA);
	}

}
