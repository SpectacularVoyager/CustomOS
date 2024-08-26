#include "../atapio/atapio.h"
#include "stdint.h"
#include <stdint.h>


void ISO_Init();


typedef struct fat_BS
{
	unsigned char 		bootjmp[3];
	unsigned char 		oem_name[8];
	unsigned short 	    bytes_per_sector;
	unsigned char		sectors_per_cluster;
	unsigned short		reserved_sector_count;
	unsigned char		table_count;
	unsigned short		root_entry_count;
	unsigned short		total_sectors_16;
	unsigned char		media_type;
	unsigned short		table_size_16;
	unsigned short		sectors_per_track;
	unsigned short		head_side_count;
	unsigned int 		hidden_sector_count;
	unsigned int 		total_sectors_32;
	
	//this will be cast to it's specific type once the driver actually knows what type of FAT this is.
	unsigned char		extended_section[54];
	
}__attribute__((packed)) fat_BS_t;

typedef struct {
    uint8_t  signature[8];                    // 0x0: Signature, "EFI PART"
    uint32_t revision;                        // 0x8: GPT Revision
    uint32_t header_size;                     // 0xC: Header size
    uint32_t crc32_header;                    // 0x10: CRC32 checksum of the GPT header
    uint32_t reserved;                        // 0x14: Reserved
    uint64_t lba_self;                        // 0x18: The LBA containing this header
    uint64_t lba_alternate;                   // 0x20: The LBA of the alternate GPT header
    uint64_t lba_first_usable;                // 0x28: The first usable block
    uint64_t lba_last_usable;                 // 0x30: The last usable block
    uint8_t  disk_guid[16];                   // 0x38: GUID of the disk
    uint64_t lba_partition_entries;           // 0x48: Starting LBA of the GUID Partition Entry array
    uint32_t num_partition_entries;           // 0x50: Number of Partition Entries
    uint32_t size_partition_entry;            // 0x54: Size of each Partition Entry
    uint32_t crc32_partition_entries;         // 0x58: CRC32 of the Partition Entry array
    uint8_t  reserved2[420];                  // 0x5C: Reserved (should be zeroed)
}__attribute__((packed)) GPTHeader;

typedef struct{
	uint8_t type[16];
	uint8_t uniqueGUID[16];
	uint64_t startingLBA;
	uint64_t endingLBA;
	uint64_t attributes;
	uint8_t name[72];
}__attribute__((packed)) GPTPartitionEntry;
