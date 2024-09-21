#pragma once
#include "stdint.h"
#include "base.h"


#define APIC_PROCESSOR_LOCAL					0
#define APIC_IO								    1
#define APIC_IO_INT_SOURCE_OVERRIDE				2
#define APIC_NMI								3
#define APIC_LOCAL_NMI							4
#define APIC_LOCAL_ADDRESS_OVERRIDE				5
#define APIC_LOCAL_X2_OVERRIDE					9





typedef struct {
	uint8_t type;
	uint8_t length;
}__attribute__((packed)) APIC_RECORD;

//0
typedef struct {
	APIC_RECORD record;
	uint8_t proc_id;
	uint8_t apic_id;
	uint32_t flags;
}__attribute__((packed)) APIC_PROCESSOR_LOCAL_RECORD;

//1
typedef struct{
	APIC_RECORD record;
	uint8_t ioapic_id;
	uint8_t res1;
	uint32_t ioapic_addr;
	uint32_t global_int_base;
}__attribute__((packed))APIC_IO_RECORD;

//2
typedef struct{
	APIC_RECORD record;
	uint8_t bus;
	uint8_t irq;
	uint32_t global_int_base;
	uint16_t flags;
}__attribute__((packed))APIC_IO_INT_SOURCE_OVERRIDE_RECORD;

//3
typedef struct{
	APIC_RECORD record;
	uint8_t nmiSource;
	uint8_t res;
	uint16_t flags;
	uint32_t global_int_base;
}__attribute__((packed))APIC_NMI_RECORD;

//4
typedef struct{
	APIC_RECORD record;
	uint8_t acpi_proc_id;
	uint16_t flags;
	uint8_t lint;
}__attribute__((packed))APIC_LOCAL_NMI_RECORD;

//5
typedef struct{
	APIC_RECORD record;
	uint16_t res;
	uint64_t phy_addr;
}__attribute__((packed))APIC_LOCAL_ADDRESS_OVERRIDE_RECORD;

//9
typedef struct{
	APIC_RECORD record;
	uint16_t res;
	uint32_t apic_id;
	uint32_t flags;
	uint32_t acpi_id;
}__attribute__((packed))APIC_LOCAL_X2_OVERRIDE_RECORD;

typedef struct{
	ACPISDTHeader h;
	uint32_t localAPICaddr;
	uint32_t flags;
	APIC_RECORD records[1];
} __attribute__((packed)) MADT;
