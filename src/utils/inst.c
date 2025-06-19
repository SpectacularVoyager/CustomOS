#include "inst.h"
#include "stdlib/stdio.h"

 int INST_READ(uint8_t* data){
	 printf("INSTRUCTION:\t");
	 
	 if(data[0]==INST_INT){
			printf("INT 0x%x\n",data[1]);
			return 2;
	 }else if(data[0]==INST_HALT){
			printf("HALT\n");
			return 1;
	 }else if(data[0]==0xF&&data[1]==0x5){
			printf("SYSCALL\n");
			return 0;
	 }else if(data[0]==0xF&&data[1]==0x7){
			printf("SYSRET\n");
			return 0;
	 }else if(data[0]==0x48&&data[1]==0xCF){
			printf("IRETQ\n");
			return 0;
	 }else{
			printf("[%x] %x %x %x\n",data[0],data[1],data[2],data[3]);
			return 0;
	 }
 }
