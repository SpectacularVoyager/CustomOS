#include <stdint.h>

void Paging_Disable();
void PagingInit();

void PageSetup();

extern uint64_t p4_table[512];
extern uint64_t p3_table[512];
extern uint64_t p2_table[512];

void putPage(uint64_t addr,int size);

void PageIdentityMap(uint64_t addr);
