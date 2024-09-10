#pragma once
#include "stdint.h"
#include "stdbool.h"

struct LongModeData{
	bool enabled;
	char arr[12];
	uint32_t cpu_feat[2];
};

int LongMode_CheckCPUID();

int LongMode_GetVendor(void* str);
int LongMode_GetFeatures();
bool LongMode_LongModeSupported();
bool LongMode_A20STATUS();
void LongMode_EnableA20();

int LongMode_Setup();
int LongMode_Enable();

enum {
    CPUID_FEAT_ECX_SSE3         = 1 << 0x00,
    CPUID_FEAT_ECX_PCLMUL       = 1 << 0x01,
    CPUID_FEAT_ECX_DTES64       = 1 << 0x02,
    CPUID_FEAT_ECX_MONITOR      = 1 << 0x03,
    CPUID_FEAT_ECX_DS_CPL       = 1 << 0x04,
    CPUID_FEAT_ECX_VMX          = 1 << 0x05,
    CPUID_FEAT_ECX_SMX          = 1 << 0x06,
    CPUID_FEAT_ECX_EST          = 1 << 0x07,
    CPUID_FEAT_ECX_TM2          = 1 << 0x08,
    CPUID_FEAT_ECX_SSSE3        = 1 << 0x09,
    CPUID_FEAT_ECX_CID          = 1 << 0x0A,
    CPUID_FEAT_ECX_SDBG         = 1 << 0x0B,
    CPUID_FEAT_ECX_FMA          = 1 << 0x0C,
    CPUID_FEAT_ECX_CX16         = 1 << 0x0D,
    CPUID_FEAT_ECX_XTPR         = 1 << 0x0E,
    CPUID_FEAT_ECX_PDCM         = 1 << 0x0F,
    CPUID_FEAT_ECX_PCID         = 1 << 0x10,
    CPUID_FEAT_ECX_DCA          = 1 << 0x11,
    CPUID_FEAT_ECX_SSE4_1       = 1 << 0x12,
    CPUID_FEAT_ECX_SSE4_2       = 1 << 0x13,
    CPUID_FEAT_ECX_X2APIC       = 1 << 0x14,
    CPUID_FEAT_ECX_MOVBE        = 1 << 0x15,
    CPUID_FEAT_ECX_POPCNT       = 1 << 0x16,
    CPUID_FEAT_ECX_TSC          = 1 << 0x17,
    CPUID_FEAT_ECX_AES          = 1 << 0x18,
    CPUID_FEAT_ECX_XSAVE        = 1 << 0x19,
    CPUID_FEAT_ECX_OSXSAVE      = 1 << 0x1A,
    CPUID_FEAT_ECX_AVX          = 1 << 0x1B,
    CPUID_FEAT_ECX_F16C         = 1 << 0x1C,
    CPUID_FEAT_ECX_RDRAND       = 1 << 0x1D,
    CPUID_FEAT_ECX_HYPERVISOR   = 1 << 0x1E,
                                         
    CPUID_FEAT_EDX_FPU          = 1 << 0x20,
    CPUID_FEAT_EDX_VME          = 1 << 0x21,
    CPUID_FEAT_EDX_DE           = 1 << 0x22,
    CPUID_FEAT_EDX_PSE          = 1 << 0x23,
    CPUID_FEAT_EDX_TSC          = 1 << 0x24,
    CPUID_FEAT_EDX_MSR          = 1 << 0x25,
    CPUID_FEAT_EDX_PAE          = 1 << 0x26,
    CPUID_FEAT_EDX_MCE          = 1 << 0x27,
    CPUID_FEAT_EDX_CX8          = 1 << 0x28,
    CPUID_FEAT_EDX_APIC         = 1 << 0x29,
    CPUID_FEAT_EDX_SEP          = 1 << 0x2B,
    CPUID_FEAT_EDX_MTRR         = 1 << 0x2C,
    CPUID_FEAT_EDX_PGE          = 1 << 0x2D,
    CPUID_FEAT_EDX_MCA          = 1 << 0x2E,
    CPUID_FEAT_EDX_CMOV         = 1 << 0x2F,
    CPUID_FEAT_EDX_PAT          = 1 << 0x30,
    CPUID_FEAT_EDX_PSE36        = 1 << 0x31,
    CPUID_FEAT_EDX_PSN          = 1 << 0x32,
    CPUID_FEAT_EDX_CLFLUSH      = 1 << 0x33,
    CPUID_FEAT_EDX_DS           = 1 << 0x34,
    CPUID_FEAT_EDX_ACPI         = 1 << 0x35,
    CPUID_FEAT_EDX_MMX          = 1 << 0x36,
    CPUID_FEAT_EDX_FXSR         = 1 << 0x37,
    CPUID_FEAT_EDX_SSE          = 1 << 0x38,
    CPUID_FEAT_EDX_SSE2         = 1 << 0x39,
    CPUID_FEAT_EDX_SS           = 1 << 0x3A,
    CPUID_FEAT_EDX_HTT          = 1 << 0x3B,
    CPUID_FEAT_EDX_TM           = 1 << 0x3C,
    CPUID_FEAT_EDX_IA64         = 1 << 0x3D,
    CPUID_FEAT_EDX_PBE          = 1 << 0x3E
};
