#pragma once
#include "stdint.h"
#include "stdbool.h"

#define CONFIG_ADDRESS		0x0CFC

#define PCI_MAX_BUSES 256
#define PCI_MAX_DEVICES 32
#define PCI_MAX_FUNCTIONS 8

#define COMBINE_WORD(msb, lsb) (((uint32_t)(msb) << 16) | (lsb))

#define EXPORT_BYTE(target, first)                                             \
  ((first) ? ((target) & ~0xFF00) : (((target) & ~0x00FF) >> 8))
void PCI_Initiate();


typedef struct{
  uint16_t bus;
  uint16_t slot;
  uint16_t function;

  uint16_t vendor_id;
  uint16_t device_id;

  uint16_t command;
  uint16_t status;

  uint8_t revision;
  uint8_t progIF; // programming interface
  uint8_t subclass_id;
  uint8_t class_id;

  uint8_t cacheLineSize;
  uint8_t latencyTimer;
  uint8_t headerType;
  uint8_t bist;
} PCI_device;

typedef struct{
	PCI_device base;
	uint32_t BAR[6];
	uint32_t CISPointer;
	uint16_t subsystem_id;
	uint16_t subsystem_vendor_id;
	uint32_t expansion_rom_address;
	uint8_t capabilities_pointer;
	uint8_t max_latency;
	uint8_t min_grant;
	uint8_t interript_pin;
	uint8_t interrupt_line;
}PCIGeneralDevice;

void PCI_ReadDevice(PCI_device* device,uint16_t bus,uint8_t slot,uint8_t function);

PCI_device* PCI_GetDevices();

void PCI_Device_Print(PCI_device* d);
PCI_device* PCI_GetFromID(uint16_t vendor,uint16_t device_id);
uint16_t PCI_GetDeviceCount();

void PCI_GetGeneralDevice(PCI_device *device, PCIGeneralDevice *out);

void PCI_Refresh(PCI_device* device);

void PCI_ConfigWriteWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset,uint32_t val);

void PCI_DeviceConfigWriteWord(PCI_device* d, uint8_t offset,uint32_t val);


void* PCI_GetMMIO(PCI_device* device,void* base);

#define PCI_VENDOR_ID 0x00
#define PCI_DEVICE_ID 0x02
#define PCI_COMMAND 0x04
#define PCI_STATUS 0x06
#define PCI_REVISION_ID 0x08
#define PCI_PROG_IF 0x09
#define PCI_SUBCLASS 0x0a
#define PCI_CLASS 0x0b
#define PCI_CACHE_LINE_SIZE 0x0c
#define PCI_LATENCY_TIMER 0x0d
#define PCI_HEADER_TYPE 0x0e
#define PCI_BIST 0x0f
#define PCI_BAR0 0x10
#define PCI_BAR1 0x14
#define PCI_BAR2 0x18
#define PCI_BAR3 0x1C
#define PCI_BAR4 0x20
#define PCI_BAR5 0x24
#define PCI_SECONDARY_BUS 0x09
#define PCI_SYSTEM_VENDOR_ID 0x2C
#define PCI_SYSTEM_ID 0x2E
#define PCI_EXP_ROM_BASE_ADDR 0x30
#define PCI_CAPABILITIES_PTR 0x34
#define PCI_INTERRUPT_LINE 0x3C
#define PCI_MIN_GRANT 0x3E
// PCI Class codes
#define PCI_CLASS_CODE_UNCLASSIFIED 0x0
#define PCI_CLASS_CODE_MASS_STORAGE_CONTROLLER 0x1
#define PCI_CLASS_CODE_NETWORK_CONTROLLER 0x2
#define PCI_CLASS_CODE_DISPLAY_CONTROLLER 0x3
#define PCI_CLASS_CODE_MULTIMEDIA_CONTROLLER 0x4
#define PCI_CLASS_CODE_MEMORY_CONTROLLER 0x5
#define PCI_CLASS_CODE_BRIDGE_CONTROLLER 0x6
#define PCI_CLASS_CODE_SIMPLE_COMM_CONTROLLER 0x7
#define PCI_CLASS_CODE_BASE_SYSTEM_PERIPHERAL 0x8
#define PCI_CLASS_CODE_INPUT_DEVICE_CONTROLLER 0x9
#define PCI_CLASS_CODE_DOCKING_SYSTEM 0xA
#define PCI_CLASS_CODE_PROCESSOR 0xB
#define PCI_CLASS_CODE_SERIAL_BUS_CONTROLLER 0xC
#define PCI_CLASS_CODE_WIRELESS_CONTROLLER 0xD
#define PCI_CLASS_CODE_INTELLIGENT_CONTROLLER 0xE
#define PCI_CLASS_CODE_SATELLITE_COMM_CONTROLLER 0xF
#define PCI_CLASS_CODE_ENCRYPTION_CONTROLLER 0x10
#define PCI_CLASS_CODE_SIGNAL_PROCESSING_CONTROLLER 0x11
#define PCI_CLASS_CODE_PROCESSING_ACCELERATOR 0x12
#define PCI_CLASS_CODE_NON_ESSENTIAL_INSTRUMENTATION 0x13
#define PCI_CLASS_CODE_COPROCESSOR 0x40
#define PCI_CLASS_CODE_UNASSIGNED 0xFF
