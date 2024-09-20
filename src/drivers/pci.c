#include "pci.h"
#include "../stdlib/stdio.h"
#include "../utils/ports.h"
PCI_device devices[100]={0};
int PCI_dev_count=0;
PCI_device* PCI_GetDevices(){
	return devices;
}
uint16_t PCI_GetDeviceCount(){
	return PCI_dev_count;
}
PCI_device* PCI_GetFromID(uint16_t vendor,uint16_t device_id){
	for(uint16_t i=0;i<PCI_GetDeviceCount();i++){
		if(devices[i].vendor_id==vendor&&devices[i].device_id==device_id)
			return &devices[i];
	}
	return 0;
}

PCI_device* PCI_GetFromType(int Class,int Subclass){
	for(uint16_t i=0;i<PCI_GetDeviceCount();i++){
		if(devices[i].class_id==Class&&devices[i].subclass_id==Subclass)
			return &devices[i];
	}
	return 0;
}

uint16_t PCI_ConfigReadWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset) {
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    uint16_t tmp = 0;
  
    // Create configuration address as per Figure 1
    address = (uint32_t)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));
  
    // Write out the address
    outportl(0xCF8, address);
    // Read in the data
    // (offset & 2) * 8) = 0 will choose the first word of the 32-bit register
    tmp = (uint16_t)((inportl(0xCFC) >> ((offset & 2) * 8)) & 0xFFFF);
    return tmp;
}

void PCI_DeviceConfigWriteWord(PCI_device* d, uint8_t offset,uint32_t val) {
    uint32_t address;
    uint32_t lbus  = (uint32_t)d->bus;
    uint32_t lslot = (uint32_t)d->slot;
    uint32_t lfunc = (uint32_t)d->function;
    uint16_t tmp = 0;
  
    // Create configuration address as per Figure 1
    address = (uint32_t)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));
  
    // Write out the address
    outportl(0xCF8, address);
    // Read in the data
    // (offset & 2) * 8) = 0 will choose the first word of the 32-bit register
	outportl(0xCFC,val);
}
void PCI_ConfigWriteWord(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset,uint32_t val) {
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t lslot = (uint32_t)slot;
    uint32_t lfunc = (uint32_t)func;
    uint16_t tmp = 0;
  
    // Create configuration address as per Figure 1
    address = (uint32_t)((lbus << 16) | (lslot << 11) |
              (lfunc << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));
  
    // Write out the address
    outportl(0xCF8, address);
    // Read in the data
    // (offset & 2) * 8) = 0 will choose the first word of the 32-bit register
	outportl(0xCFC,val);
}
uint16_t PCI_CheckVendor(uint8_t bus, uint8_t slot,uint8_t function) {
    uint16_t vendor, device;
    if ((vendor = PCI_ConfigReadWord(bus, slot, function, 0)) != 0xFFFF) {
       device = PCI_ConfigReadWord(bus, slot, function, 2);
    } return (vendor);
}
void PCI_Device_Print(PCI_device* d){
	if(!d){
		kprintf("DEVICE:NULL\n");
		return;
	}
	kprintf("-----------------[DEVICE 0x%04X 0x%04X]-----------------\n",d->vendor_id,d->device_id);
	//kprintf("VENDOR 0x%X DEVICE 0x%X\n",d->vendor_id,d->device_id);
	kprintf("\tLOCATION [%d][%d][%d]\n",d->bus&0xFF,d->slot&0xFF,d->function&0xFF);
	kprintf("\tCLASS : 0X%02X  SUBCLASS : 0X%02X  PROG_IF : 0X%02X\n",d->class_id,d->subclass_id,d->progIF);
	kprintf("\tHEADER TYPE 0X%02X\n",d->headerType);
	kprintf("--------------------------------------------------------\n",d->vendor_id,d->device_id);
}
void PCI_Initiate(){
	PCI_dev_count=0;
  for (uint16_t bus = 0; bus < PCI_MAX_BUSES; bus++) {
    for (uint8_t slot = 0; slot < PCI_MAX_DEVICES; slot++) {
      for (uint8_t function = 0; function < PCI_MAX_FUNCTIONS; function++) {
		  uint16_t vendor=PCI_CheckVendor(bus,slot,function);
		  if(vendor==0xFFFF){
			continue;
		  }
		  PCI_device device;
		  PCI_ReadDevice(&device,bus,slot,function);
		  //PCI_Device_Print(&device);
		  devices[PCI_dev_count]=device;
		  PCI_dev_count++;
	  }
	}
  }
}
void PCI_Refresh(PCI_device* device){
	PCI_ReadDevice(device,device->bus,device->slot,device->function);	
}
void PCI_ReadDevice(PCI_device* device,uint16_t bus,uint8_t slot,uint8_t function){
	device->bus=bus;
	device->slot=slot;
	device->function=function;

	device->vendor_id=PCI_ConfigReadWord(bus,slot,function,PCI_VENDOR_ID);
	device->device_id=PCI_ConfigReadWord(bus,slot,function,PCI_DEVICE_ID);
	device->status=PCI_ConfigReadWord(bus,slot,function,PCI_STATUS);
	device->command=PCI_ConfigReadWord(bus,slot,function,PCI_COMMAND);

	uint16_t revprogif=PCI_ConfigReadWord(bus,slot,function,PCI_REVISION_ID);
	device->revision=revprogif&0xFF;
	device->progIF=(revprogif>>8)&0xFF;

	uint16_t subclassClass=PCI_ConfigReadWord(bus,slot,function,PCI_SUBCLASS);
	device->subclass_id=subclassClass&0xFF;
	device->class_id=(subclassClass>>8)&0xFF;

	uint16_t cachelatency=PCI_ConfigReadWord(bus,slot,function,PCI_CACHE_LINE_SIZE);
	device->cacheLineSize=cachelatency&0xFF;
	device->latencyTimer=(cachelatency>>8)&0xFF;

	uint16_t headerbist=PCI_ConfigReadWord(bus,slot,function,PCI_HEADER_TYPE);
	device->headerType=headerbist&0xFF;
	device->bist=(headerbist>>8)&0xFF;
}
void* PCI_GetMMIO(PCI_device* device,void* base){
	return base+((device->bus) << 20 | device->slot << 15 | device->function << 12);
}
void PCI_GetGeneralDevice(PCI_device *device, PCIGeneralDevice *out) {
	//FOR BAR 0-6
	for (int i = 0; i < 6; i++)
		out->BAR[i] =
			COMBINE_WORD(PCI_ConfigReadWord(device->bus, device->slot, device->function,
						PCI_BAR0 + 4 * i + 2),
					PCI_ConfigReadWord(device->bus, device->slot, device->function,
						PCI_BAR0 + 4 * i));

	out->subsystem_vendor_id = PCI_ConfigReadWord(
			device->bus, device->slot, device->function, PCI_SYSTEM_VENDOR_ID);
	out->subsystem_id = PCI_ConfigReadWord(device->bus, device->slot, device->function,
			PCI_SYSTEM_ID);

	out->expansion_rom_address =
		COMBINE_WORD(PCI_ConfigReadWord(device->bus, device->slot, device->function,
					PCI_EXP_ROM_BASE_ADDR + 2),
				PCI_ConfigReadWord(device->bus, device->slot, device->function,
					PCI_EXP_ROM_BASE_ADDR));

	out->capabilities_pointer =
		EXPORT_BYTE(PCI_ConfigReadWord(device->bus, device->slot, device->function,
					PCI_CAPABILITIES_PTR),
				true);

	uint32_t interruptLine_interruptPIN = PCI_ConfigReadWord(
			device->bus, device->slot, device->function, PCI_INTERRUPT_LINE);
	out->interrupt_line = EXPORT_BYTE(interruptLine_interruptPIN, true);
	out->interript_pin = EXPORT_BYTE(interruptLine_interruptPIN, false);

	uint32_t minGrant_maxLatency = PCI_ConfigReadWord(
			device->bus, device->slot, device->function, PCI_MIN_GRANT);
	out->min_grant = EXPORT_BYTE(minGrant_maxLatency, true);
	out->max_latency = EXPORT_BYTE(minGrant_maxLatency, false);
	out->base=*device;
}
