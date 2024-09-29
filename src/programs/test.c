#include "../stdlib/stdio.h"
#include "../graphics/graphics.h"
#include "../drivers/pci.h"
#include "../devices/keyboard.h"
#include "../disk/iso/iso.h"
#include "../drivers/acpi/acpi.h"
#include "../drivers/apic/apic.h"

#include "../drivers/networking/rtl8168/rtl8168.h"
char* cpuid_flags[62]={
    "CPUID_FEAT_ECX_SSE3         ",
    "CPUID_FEAT_ECX_PCLMUL       ",
    "CPUID_FEAT_ECX_DTES64       ",
    "CPUID_FEAT_ECX_MONITOR      ",
    "CPUID_FEAT_ECX_DS_CPL       ",
    "CPUID_FEAT_ECX_VMX          ",
    "CPUID_FEAT_ECX_SMX          ",
    "CPUID_FEAT_ECX_EST          ",
    "CPUID_FEAT_ECX_TM2          ",
    "CPUID_FEAT_ECX_SSSE3        ",
    "CPUID_FEAT_ECX_CID          ",
    "CPUID_FEAT_ECX_SDBG         ",
    "CPUID_FEAT_ECX_FMA          ",
    "CPUID_FEAT_ECX_CX16         ",
    "CPUID_FEAT_ECX_XTPR         ",
    "CPUID_FEAT_ECX_PDCM         ",
    "CPUID_FEAT_ECX_PCID         ",
    "CPUID_FEAT_ECX_DCA          ",
    "CPUID_FEAT_ECX_SSE4_1       ",
    "CPUID_FEAT_ECX_SSE4_2       ",
    "CPUID_FEAT_ECX_X2APIC       ",
    "CPUID_FEAT_ECX_MOVBE        ",
    "CPUID_FEAT_ECX_POPCNT       ",
    "CPUID_FEAT_ECX_TSC          ",
    "CPUID_FEAT_ECX_AES          ",
    "CPUID_FEAT_ECX_XSAVE        ",
    "CPUID_FEAT_ECX_OSXSAVE      ",
    "CPUID_FEAT_ECX_AVX          ",
    "CPUID_FEAT_ECX_F16C         ",
    "CPUID_FEAT_ECX_RDRAND       ",
    "CPUID_FEAT_ECX_HYPERVISOR   ",
    "CPUID_FEAT_EDX_FPU          ",
    "CPUID_FEAT_EDX_VME          ",
    "CPUID_FEAT_EDX_DE           ",
    "CPUID_FEAT_EDX_PSE          ",
    "CPUID_FEAT_EDX_TSC          ",
    "CPUID_FEAT_EDX_MSR          ",
    "CPUID_FEAT_EDX_PAE          ",
    "CPUID_FEAT_EDX_MCE          ",
    "CPUID_FEAT_EDX_CX8          ",
	"",
    "CPUID_FEAT_EDX_APIC         ",
    "CPUID_FEAT_EDX_SEP          ",
    "CPUID_FEAT_EDX_MTRR         ",
    "CPUID_FEAT_EDX_PGE          ",
    "CPUID_FEAT_EDX_MCA          ",
    "CPUID_FEAT_EDX_CMOV         ",
    "CPUID_FEAT_EDX_PAT          ",
    "CPUID_FEAT_EDX_PSE36        ",
    "CPUID_FEAT_EDX_PSN          ",
    "CPUID_FEAT_EDX_CLFLUSH      ",
    "CPUID_FEAT_EDX_DS           ",
    "CPUID_FEAT_EDX_ACPI         ",
    "CPUID_FEAT_EDX_MMX          ",
    "CPUID_FEAT_EDX_FXSR         ",
    "CPUID_FEAT_EDX_SSE          ",
    "CPUID_FEAT_EDX_SSE2         ",
    "CPUID_FEAT_EDX_SS           ",
    "CPUID_FEAT_EDX_HTT          ",
    "CPUID_FEAT_EDX_TM           ",
    "CPUID_FEAT_EDX_IA64         ",
    "CPUID_FEAT_EDX_PBE          "
};
void testMain(){
	kprintf(INFO "EY\n");
	while(1);
}

void DebugKeyboardHandler(KeyCode code){
	if(code.type=KEY_TYPE_ASCII && code.pressed){
		printf("%c",code.val);
	}
	APIC_SEND_EOI();
}
void Debug(){
	SetColor(0xFF<<8);
	printf(INFO "HELLO WORLD!!!\n");
	PCI_device* devices=PCI_GetDevices();
	printf(TRACE "DETECTED %d devices\n",PCI_GetDeviceCount());
	for(uint16_t i=0;i<PCI_GetDeviceCount();i++){
		//PCI_Device_Print(&devices[i]);
	}
	KeyboardSetProcess(DebugKeyboardHandler);
	for(int i=0;i<PCI_GetDeviceCount();i++){
		if(i%4==0)printf("\n");
		printf("[%04X %04X]{%02X %02X %02X}\t",devices[i].vendor_id,devices[i].device_id,devices[i].class_id,devices[i].subclass_id,devices[i].progIF);	
		//PCI_Device_Print(&devices[i]);
	}

	//PCI_device* nic=PCI_GetFromID(0x10EC, 0x8168);
	//ISO_Init();	
	//SwapBuffers();
	//if(nic)
	//	RTL8168_INIT(nic);
	while(1);
}
