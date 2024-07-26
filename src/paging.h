#include "stdint.h"
// This should go outside any function..
extern __attribute__((cdecl)) void loadPageDirectory(void * page);
extern __attribute__((cdecl)) void enablePaging();
void Paging_Init();
