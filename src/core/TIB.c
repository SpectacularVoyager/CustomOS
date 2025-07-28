#include "TIB.h"
#include "utils/ports.h"
#include "stdlib/string.h"
TIB tib;
void setFS(){
	memset(&tib,1,sizeof(TIB));
	WRMSR(0xC0000100,(uint64_t)&tib);
	
}
