#include "paging.h"
#include "utils/utils.h"
#include <stdlib/string.h>
#include <stdlib/stdio.h>

__attribute__((aligned(4096))) unsigned long pagetable_4[512];
__attribute__((aligned(4096))) unsigned long pagetable_3[512];
__attribute__((aligned(4096))) unsigned long pagetable_2[512];

extern volatile uint64_t p4_table[512];
extern volatile uint64_t p3_table[512];
extern volatile uint64_t p2_table[512];

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
    setCR3(((unsigned long)p4_table&(~0xFFF)));
    // setCR4(getCR4()|1<<5);                      // ENABLE PAE
    //setCR0(getCR0()|1<<31);                     //PAGING ENABLE
}
void PagingDisable(){

    //setCR0(getCR0()&(~(1<<31)));                     //PAGING ENABLE
}
#define PAGETABLE(addr,ops) ((((unsigned long)(addr)&(~0xFFF)))|(ops))

void Paging_SetTables(){
        // LOGVALD(p3_table[0]);
        // LOGVALD(p3_table[1]);
    memset(p4_table,0,4096);
    //memset(p3_table,0,4096);
    p4_table[0]=PAGETABLE(p3_table, 0b111);
    p3_table[0]=PAGETABLE(p2_table, 0b111);
    // for(size_t i=0;i<512;i++){
    //     p2_table[i]=(0x200000*i)|0b10000111;
    // }
}
