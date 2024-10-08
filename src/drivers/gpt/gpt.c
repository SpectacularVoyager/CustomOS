#include "gpt.h"
#include "../ahci/ahci.h"
#include "../../stdlib/stdlib.h"
#include "../../stdlib/stdio.h"

void GPT_READ(){
	//GPT_BOOT_RECORD* record=malloc(sizeof(GPT_BOOT_RECORD));
	//AHCI_READ(0,1, (void*)record);
	//kprintf(INFO"BOOT %x [%x -> %x]\n",record->ostype,record->lba_start,record->lba_end);
	GPT_PART_HEADER* buffer=malloc(0x200);
	AHCI_READ(0x1,1,(void*)buffer);
	kprintf("%8s\t%p\n",buffer->signature,buffer->partition_entry_lba);

	int num=buffer->number_of_partition_entries*buffer->size_of_partition_entry;
	GPT_PART_ENTRY* entry=malloc(num+0x200);
	AHCI_READ(buffer->partition_entry_lba,2,(void*)entry);
	for(int i=0;i<5;i++){
		printf(INFO"GPT PART[%d]\t",i);
		for(int j=0;j<36;j++){
			printf("%c",(uint8_t)entry[i].name[j]);
		}
		printf("\n");
	}
}
