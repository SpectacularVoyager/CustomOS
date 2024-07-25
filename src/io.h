#pragma once
#include <stdint.h>

void __attribute__((cdecl)) outb(uint16_t port, uint8_t value);
uint8_t __attribute__((cdecl)) inb(uint16_t port);

void __attribute__((cdecl)) EnableInterrupts();
void __attribute__((cdecl)) DisableInterrupts();

void __attribute__((cdecl)) Panic();

void io_wait();

uint32_t inportl(uint16_t portid);

void outportl(uint16_t portid, uint32_t value);
