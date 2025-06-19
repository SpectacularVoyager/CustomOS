#include "Userspace.h"
#include "utils/utils.h"
#include "utils/ports.h"
#include <stdio.h>
#include "MSR.h"
void SYSCALL_INIT(){
	uint64_t val=RDMSR(MSR_IA32_EFER);
	val|=1;
	WRMSR(MSR_IA32_EFER,val);
}


void SYSRET(uint64_t cs,uint64_t rip,uint64_t flags){
	uint64_t val=RDMSR(MSR_IA32_STAR);
	// val&=(uint64_t)(~0xFFFFL)<<32;
	// val|=cs<<32;
	val=0;
	WRMSR(MSR_IA32_STAR,val);
	LOGVALD(RDMSR(MSR_IA32_STAR));
	printf("SYSRET");
	__SYSRET(rip,flags);
}
