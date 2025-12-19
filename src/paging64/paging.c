#include "paging.h"
#include "utils/utils.h"
#include <stdlib/string.h>
#include <stdlib/stdio.h>

__attribute__((aligned(4096))) unsigned long pagetable_4[512];
__attribute__((aligned(4096))) unsigned long pagetable_3[512];
__attribute__((aligned(4096))) unsigned long pagetable_2[512];

void setCR0(unsigned long val){
    asm __volatile__("mov %0,%%cr0"::"r"(val):"memory");
}
void setCR3(unsigned long val){
    asm __volatile__("mov %0,%%cr3"::"r"(val):"memory");
}
void setCR4(unsigned long val){
    asm __volatile__("mov %0,%%cr4"::"r"(val):"memory");
}
unsigned long getCR0(){
    unsigned long val;
    asm __volatile__("mov %%cr0,%0":"=r"(val)::"memory");
    return val;
}
unsigned long getCR3(){
    unsigned long val;
    asm __volatile__("mov %%cr3,%0":"=r"(val)::"memory");
    return val;
}
unsigned long getCR4(){
    unsigned long val;
    asm __volatile__("mov %%cr4,%0":"=r"(val)::"memory");
    return val;
}


void PagingEnable(){
    LOGVALD(getCR3());
    LOGVALD(((unsigned long)pagetable_4&(~0xFFF)));
    //setCR3(((unsigned long)pagetable_4&(~0xFFF)));
    // setCR4(getCR4()|1<<5);                      // ENABLE PAE
    // setCR0(getCR0()|1<<31);                     //PAGING ENABLE
}
void PagingDisable(){

    //setCR0(getCR0()&(~(1<<31)));                     //PAGING ENABLE
}
#define PAGETABLE(addr,ops) ((((unsigned long)(addr)&(~0xFFF)))|(ops))


void Paging_SetTables(){
    memset(pagetable_4,0,4096);
    memset(pagetable_3,0,4096);
    memset(pagetable_2,0,4096);
    LOGVALD(pagetable_2);
    LOGVALD(pagetable_3);
    LOGVALD(pagetable_4);
    pagetable_4[0]=PAGETABLE(pagetable_3, 0b111);
    pagetable_4[511]=PAGETABLE(pagetable_4, 0b11);
    pagetable_3[0]=PAGETABLE(pagetable_2, 0b111);
    // pagetable_2[0]=0b111;
    for(size_t i=0;i<512;i++){
        pagetable_2[i]=(0x200000*i)|0b111;
    }
}
