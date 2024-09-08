#include <stdint.h>

void Paging_Disable();
void PagingInit();

void PageSetup(uint64_t fb_index);

extern uint64_t p4_table[512];
extern uint64_t p3_table[512];
extern uint64_t p2_table[512];

void putPage(uint64_t addr,int size);

void PageIdentityMap(uint64_t addr);

typedef struct {
	uint64_t* pageDir;
	unsigned int page3_offset;
} PageTable;

typedef struct {
	PageTable table;
	unsigned int offset;
} Page;
void PageCreateEmpty(int);

Page PageAlloc();
