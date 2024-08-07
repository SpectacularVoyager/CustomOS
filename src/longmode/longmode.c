#include "longmode.h"
#include "../paging/paging.h"
#include "../stdio.h"



char arr[12];
uint32_t cpu_feat[2];
int LongMode_Setup(){
	int cpuid=LongMode_CheckCPUID();
	if(cpuid==0)return 0;

	LongMode_GetVendor(arr);
	printf("%12s\n",arr);
	int b=LongMode_LongModeSupported();
	printf("LONG MODE SUPPORTED %d\n",b);
	if(b==0){return 0;}
	int a20status=LongMode_LongModeSupported();
	printf("A20 STATUS %d\n",a20status);
	if(a20status==0){
		LongMode_EnableA20();
		printf("A20 STATUS AFTER ENABLING %d\n",a20status);
	}
	Paging_Disable();

	return 1;
}
