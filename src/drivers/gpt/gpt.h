#include "stdint.h"
#include "../ahci/ahci.h"
/**
 *	REFER TO UEFI.org
 *	https://uefi.org/specs/UEFI/2.10/05_GUID_Partition_Table_Format.html
 */
#define GPT_SECTOR_SIZE 0x200
#define GPT_PROTECTIVE	0xEE
int GPT_READ(AHCI_HBA_PORT* port);

typedef struct {
	uint8_t boot;
	uint8_t chs_start[3];
	uint8_t ostype;
	uint8_t chs_end[3];
	uint32_t lba_start;
	uint32_t lba_end;
} __attribute__((packed)) GPT_BOOT_RECORD;

typedef struct {
    uint8_t signature[8];					     // 8 bytes: "EFI PART"
    uint32_t revision;                           // 4 bytes: GPT Revision
    uint32_t header_size;                        // 4 bytes: Header size
    uint32_t crc32_header;                       // 4 bytes: CRC32 checksum of the GPT header
    uint32_t reserved;                           // 4 bytes: Reserved
    uint64_t this_lba;                           // 8 bytes: The LBA containing this header
    uint64_t alternate_lba;                      // 8 bytes: The LBA of the alternate GPT header
    uint64_t first_usable_lba;					 // 8 bytes: The first usable block
    uint64_t last_usable_lba;                    // 8 bytes: The last usable block
    uint8_t disk_guid[16];						 // 16 bytes: GUID of the disk
    uint64_t partition_entry_lba;                // 8 bytes: Starting LBA of the Partition Entry array
    uint32_t number_of_partition_entries;        // 4 bytes: Number of Partition Entries
    uint32_t size_of_partition_entry;            // 4 bytes: Size of each entry in the Partition Entry array
    uint32_t crc32_partition_entries;            // 4 bytes: CRC32 of the Partition Entry array
    uint8_t reserved_2[0x200 - 0x5C];            // Blocksize-0x5C: Reserved (should be zeroed)
} __attribute__((packed)) GPT_PART_HEADER;

typedef struct {
	uint8_t type[16];
	uint8_t guid[16];
	uint64_t startLBA;
	uint64_t endLBA;
	uint64_t attributes;
	uint16_t name[36];
} __attribute__((packed))GPT_PART_ENTRY;

typedef struct{
	GPT_BOOT_RECORD* boot;
	GPT_PART_HEADER* header;
	GPT_PART_ENTRY* entries;
	int nEntries;
} GPT_DATA;
