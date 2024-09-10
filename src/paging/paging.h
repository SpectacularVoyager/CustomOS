#include <stdint.h>

#define PAGE_WIDTH 0x40000000

#define PAGE_PRESENT	1<<0
#define PAGE_READWRITE	1<<1
#define PAGE_USER		1<<2


void Paging_Disable();
void PagingInit();

void PageSetup(uint64_t fb_index);

extern uint64_t p4_table[512];
extern uint64_t p3_table[512];
extern uint64_t p2_table[512];

void AllocatePage(int p,unsigned long address);
