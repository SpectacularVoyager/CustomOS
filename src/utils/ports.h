#pragma once
#include "stdint.h"

inline void rdmsr(uint32_t msr, uint32_t *lo, uint32_t *hi)
{
   asm volatile("rdmsr" : "=a"(*lo), "=d"(*hi) : "c"(msr));
}

inline void wrmsr(uint32_t msr, uint32_t lo, uint32_t hi)
{
   asm volatile("wrmsr" : : "a"(lo), "d"(hi), "c"(msr));
}
inline uint64_t RDMSR(uint32_t msr)
{
	uint32_t lo,hi;
   asm volatile("rdmsr" : "=a"(lo), "=d"(hi) : "c"(msr));
   return lo|(((uint64_t)hi)<<32);
}
inline void WRMSR(uint32_t msr,uint64_t val)
{
	uint32_t lo=(uint32_t)val;
	uint32_t hi=(uint32_t)(val>>32);
   asm volatile("wrmsr" : : "a"(lo), "d"(hi), "c"(msr));
}


inline unsigned char inb(unsigned short int port)
{
        unsigned char ret;
        asm volatile ("inb %%dx,%%al":"=a" (ret):"d"(port));
        return ret;
}

inline void outb(unsigned short int port, unsigned char value)
{
        asm volatile ("outb %%al,%%dx": :"d" (port), "a"(value));
}
inline unsigned short inw(unsigned short int port){
	unsigned short ret;
	asm volatile ("inw %%dx,%%ax":"=a" (ret):"d"(port));
	return ret;
}

inline void outw(unsigned short int port, unsigned char value)
{
        asm volatile ("outw %%ax,%%dx": :"d" (port), "a"(value));
}
inline unsigned int inportl(unsigned short port)
{
	unsigned long result;
	__asm__ __volatile__("inl %%dx, %%eax" : "=a" (result) : "dN" (port));
	return result;
}

inline void outportl(unsigned short port, unsigned long data)
{
	__asm__ __volatile__("outl %%eax, %%dx" : : "d" (port), "a" (data));
}

inline void io_wait()
{
        asm volatile ("outb %%al,%%dx": :"d" (0x80), "a"(0x0));
}
inline void enableInterrupts()
{
    __asm__ volatile ("sti");
						   
}
inline void disableInterrupts()
{
    __asm__ volatile ("cli");
						   
}
