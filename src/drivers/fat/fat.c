#include "fat.h"
#include "drivers/gpt/gpt.h"
#include "stdlib/stdlib.h"
#include "stdlib/stdio.h"
#include "stdlib/string.h"
#include "drivers/ahci/ahci.h"
#include "utils/utils.h"
#define ADDRESS(lba) lba*GPT_SECTOR_SIZE
void FAT_READ_LONG_NAME(FAT_LONG_NAME* name,char ptr[13]){
	FORI(5){
		ptr[i]=name->name1[i];
	}
	FORI(6){
		ptr[i+5]=name->name2[i];
	}
	FORI(2){
		ptr[i+11]=name->name3[i];
	}
}
unsigned long FAT_SECTOR_FROM_CLUSTER(FAT32_FILESYSTEM* fs,unsigned long cluster){
	return fs->config->Base.SecPerCluster*cluster;
}
unsigned long FAT_DIR_SECTOR(FAT32_FILESYSTEM *fs,FAT_DIR* dir){
	unsigned long c= dir->FirstClusterLow|(dir->FirstClusterHigh<<16);
	c-=2;
	return FAT_SECTOR_FROM_CLUSTER(fs,c);
}
unsigned long FAT_DIRECTORY_SECTOR;
void parseAll(AHCI_HBA_PORT* port,int lba){
	FAT_STRUCT* files=malloc(GPT_SECTOR_SIZE*2);
	AHCI_READ(port,lba,2,(uint16_t*)files);
	int file=0;
	while(files[file].attr.byte1!=0)	{
		
		if(files[file].attr.byte1==0xE5){
			file++;
			continue;
		}
		if(files[file].attr.attr==FAT_ATTR_LONG_NAME){
			FAT_LONG_NAME* longname=&files[file].longname;
			int ord=longname->ord&(0x3F);
			char* fullname=malloc(ord*13);
			for(int i=0;i<ord;i++){
				FAT_READ_LONG_NAME(&longname[i],fullname+(13*(ord-1)));
			}
			file+=ord+1;
		}else{
		
			file++;
		}
	}
}
int FAT_READPART(FAT32_FILESYSTEM* fs,AHCI_HBA_PORT* port,GPT_PART_ENTRY* entry){
	int fat;
	int lba=entry->startLBA;	
	FAT* data=malloc(GPT_SECTOR_SIZE);
	AHCI_READ(port,lba,1,(uint16_t*)data);
	if(!((data->base.JumpBoot[0]==0xEB&&data->base.JumpBoot[2]==0x90)||data->base.JumpBoot[0]==0xE9)){
		return -1;
	}
	int rootdirclusters=CEILDIV((data->base.RootEntCnt*32), data->base.BytesPerSector);

	if(data->base.FATSz16!=0){
		fat=FAT16;
	}else{
		fat=FAT32;
	}
	unsigned int totalSec=GETOR(data->base.TotalSec16,data->base.TotalSec32);
	//MAYBE BUG IN THIS LINE we need FATSz not FATSz16
	unsigned int dataSec=totalSec-(data->base.ResSectorCount+(data->base.NumFat*data->base.FATSz16)+rootdirclusters);
	unsigned int clusterCount=dataSec/data->base.SecPerCluster;
	if(clusterCount<4095){
		fat=FAT12;
	}else if(clusterCount<65525){
		fat=FAT16;
	}else{
		fat=FAT32;
	}
	if(fat==FAT12||fat==FAT16){
		printf("FAT %d is not supported\n",fat);
		return -1;
	}
	FAT_32* fat32=&data->fat32;
	int N=fat32->RootSectorCluster;
	int FATOffset=(fat*N)/8;
	unsigned long fatSec=fat32->Base.ResSectorCount+FATOffset/fat32->Base.BytesPerSector;
	unsigned long fatEntOff=FATOffset%fat32->Base.BytesPerSector;
	//void* v=malloc(GPT_SECTOR_SIZE);
	//AHCI_READ(port,fatSec+lba,1,v);
	//unsigned int ptr=*(uint32_t*)&v[fatEntOff]& 0x0FFFFFFF;
	int datalba=lba+fatSec+fat32->FATSz32*fat32->Base.NumFat;
	parseAll(port,datalba);
	fs->lba_data=datalba;
	fs->port=port;
	fs->config=&data->fat32;
	fs->lba_fat=lba+fatSec;
	return 1;
}
