#pragma once

#include <stdint.h>

void SYSCALL_INIT();
void SYSRET(uint64_t cs,uint64_t rip,uint64_t flags);
void __SYSRET(uint64_t rip,uint64_t flags);
