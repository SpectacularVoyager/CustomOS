void PagingDisable();

void setCR0(unsigned long val);
void setCR3(unsigned long val);
void setCR4(unsigned long val);
unsigned long getCR0();
unsigned long getCR3();
unsigned long getCR4();

void PagingEnable();

void Paging_SetTables();

void PageEnsureAddress(void* data);

#define TABLE(x) ((uint64_t*)((uint64_t)x&(~0xff)))
#define PAGE(x) ((uint64_t*)((uint64_t)x&(~0x1fffff)))

#define PAGE_WIDTH 0x40000000L

#define PAGE_P2_SIZE 0x200000L

#define PAGE_PRESENT	1<<0
#define PAGE_READWRITE	1<<1
#define PAGE_USER		1<<2
