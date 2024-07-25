#include "stdint.h"
// This should go outside any function..
extern __attribute__((cdecl)) void loadPageDirectory(uint32_t* page);
extern __attribute__((cdecl)) void enablePaging();
void Paging_Init();
