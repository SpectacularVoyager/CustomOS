#pragma once
#include "stdint.h"
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

inline unsigned long inportl(unsigned short port)
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
    __asm__ volatile ("sti"); // Completely hangs the computer
						   
}
