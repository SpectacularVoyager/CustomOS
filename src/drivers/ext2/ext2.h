#include "drivers/ahci/ahci.h"
#include "drivers/gpt/gpt.h"

int EXT2_READPART(void* fs,AHCI_HBA_PORT* port,GPT_PART_ENTRY* entry);

typedef struct {
    uint32_t inodesCount;
    uint32_t blocksCount;
    uint32_t rBlocksCount;
    uint32_t freeBlocksCount;
    uint32_t freeInodesCount;
    uint32_t firstDataBlock;
    uint32_t logBlockSize;
    uint32_t logFragSize;
    uint32_t blocksPerGroup;
    uint32_t fragsPerGroup;
    uint32_t inodesPerGroup;
    uint32_t lastMountTime;
    uint32_t lastWriteTime;
    uint16_t mountCount;
    uint16_t maxMountCount;
    uint16_t magic;
    uint16_t state;
    uint16_t errors;
    uint16_t minorRevLevel;
    uint32_t lastFsckTime;
    uint32_t fsckInterval;
    uint32_t osId;
    uint32_t majorRevLevel;
    uint16_t resUid;
    uint16_t resGid;
}__attribute__((packed)) EXT2_SUPERBLOCK;
