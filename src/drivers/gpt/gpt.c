#include "gpt.h"
#include "stdlib/stdlib.h"
#include "stdlib/stdio.h"
#include "stdlib/string.h"
#include "utils/utils.h"
#include "paging/paging.h"


int check128(unsigned char* data){
	int v=0;
	for(int j=0;j<16;j++){
		if(data[j]!=0)v=1;
	}
	return v==0;
}
void GPT_PrintPartName(uint16_t* name){
		printf("GPT PART\t");
		for(int j=0;j<36;j++){
			printf("%c",name[j]);
		}
		printf("\n");
}

int GPT_READ(AHCI_HBA_PORT* port,GPT_DATA* data){
	GPT_PART_HEADER* header=malloc(sizeof(GPT_PART_HEADER));
	AHCI_READ(port,0x1,1,(uint16_t*)MemoryPhysical(header));

	int num=header->number_of_partition_entries*header->size_of_partition_entry;
	GPT_PART_ENTRY* entry=malloc(num*0x80);
	AHCI_READ(port,header->partition_entry_lba,4,(void*)entry);
	if(!(strncmp("EFI PART",(char*)header->signature,8)==0)){
		printf("COULD NOT READ PART[MISSING SIGNATURE EFI PART]\n");
		hexdump(&header->signature,8,8);
		return 0;
	}
	int n_entries=0;
	for(int i=0;i<10;i++){
		if(check128(entry[i].type))continue;
		//printf(INFO"GPT PART[%d]\t",i);
		for(int j=0;j<36;j++){
			//printf("%c",(uint8_t)entry[i].name[j]);
		}
		n_entries++;
		//printf("\n");
	}
	data->header=header;
	data->entries=entry;
	data->nEntries=n_entries;
	
	return 1;
}
