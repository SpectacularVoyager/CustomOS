#include "paging/paging.h"
#include "longmode/longmode.h"
#include "stdlib/stdio.h"
void __attribute__((cdecl)) load_kernel32(){
	LongMode_Setup();
	//printf("HEY\n");
	//PagingInit();
}
