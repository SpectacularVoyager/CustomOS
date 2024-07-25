#include "io.h"
#define UNUSED_PORT 0x80

void io_wait(){
	outb(UNUSED_PORT,0);
}

uint32_t inportl(uint16_t portid) {
  uint32_t ret;
  __asm__ __volatile__("inl %%dx, %%eax" : "=a"(ret) : "d"(portid));
  return ret;
}

void outportl(uint16_t portid, uint32_t value) {
  __asm__ __volatile__("outl %%eax, %%dx" : : "d"(portid), "a"(value));
}
