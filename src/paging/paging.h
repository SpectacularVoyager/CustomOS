#include <stdint.h>
/** DOES NOT WORK WITH P4 PAGING */
#define PAGE_WIDTH 0x40000000L

#define PAGE_P2_SIZE 0x200000L

#define PAGE_PRESENT	1<<0
#define PAGE_READWRITE	1<<1
#define PAGE_USER		1<<2


void Paging_Disable();
void PagingInit();

void PageSetup(uint64_t fb_index);

extern volatile uint64_t p4_table[512];
extern volatile uint64_t p3_table[512];
extern volatile uint64_t p2_table[512];


void AllocatePage(int p,unsigned long address,unsigned int flags);

void PageRemap(int n,uint64_t offset,uint64_t address,int flags);

void MemoryRemap(uint64_t memory,uint64_t address,int flags);

void UnmapPage(uint64_t page);

void PageIdentity(int p1,int p2,int flags);

void* PageGetEmpty(int pages,int flags);

void* MemoryPhysical(void* p_addr);

void* PageAllocate();

void* PageAllocateN(unsigned int n);

void PageDealloc(void* page);

void InitAllocator();

typedef struct {
	int p;
	int count;
} AllotPageTable;
