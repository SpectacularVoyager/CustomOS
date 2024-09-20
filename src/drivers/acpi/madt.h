#pragma once
#include "stdint.h"
#include "base.h"


typedef struct {
	uint8_t type;
	uint8_t length;
} APIC_Record;
typedef struct{
	ACPISDTHeader h;
	uint32_t localAPICaddr;
	uint32_t flags;
} __attribute__((packed)) MADT;





