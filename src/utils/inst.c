#include "inst.h"
#include "stdlib/stdio.h"

 int INST_READ(uint8_t* data){
	 printf("INSTRUCTION:\t");
    switch (data[0]) {
        case INST_INT:
			printf("INT 0x%x\n",data[1]);
			return 2;
			break;
        case INST_HALT:
			printf("HALT\n");
			return 1;
			break;
					   
        default: 
			printf("[%x] %x %x %x\n",data[0],data[1],data[2],data[3]);
			return 0;
			break;
    }
 }
