#include "longmode.h"
#include "../paging/paging.h"
#include "../stdlib/stdio.h"



char arr[12];
uint32_t cpu_feat[2];
int LongMode_Setup(){
	//int cpuid=LongMode_CheckCPUID();
	//if(cpuid==0)return 0;

	//LongMode_GetVendor(arr);
	//int b=LongMode_LongModeSupported();
	//if(b==0){return 0;}
	//int a20status=LongMode_LongModeSupported();
	//if(a20status==0){
	//	LongMode_EnableA20();
	//}
	//Paging_Disable();

	return 1;
}
