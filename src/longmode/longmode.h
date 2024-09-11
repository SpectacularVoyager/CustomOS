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

#define    CPUID_FEAT_ECX_SSE3         1L << 0x00,
#define    CPUID_FEAT_ECX_PCLMUL       1L << 0x01,
#define    CPUID_FEAT_ECX_DTES64       1L << 0x02,
#define    CPUID_FEAT_ECX_MONITOR      1L << 0x03,
#define    CPUID_FEAT_ECX_DS_CPL       1L << 0x04,
#define    CPUID_FEAT_ECX_VMX          1L << 0x05,
#define    CPUID_FEAT_ECX_SMX          1L << 0x06,
#define    CPUID_FEAT_ECX_EST          1L << 0x07,
#define    CPUID_FEAT_ECX_TM2          1L << 0x08,
#define    CPUID_FEAT_ECX_SSSE3        1L << 0x09,
#define    CPUID_FEAT_ECX_CID          1L << 0x0A,
#define    CPUID_FEAT_ECX_SDBG         1L << 0x0B,
#define    CPUID_FEAT_ECX_FMA          1L << 0x0C,
#define    CPUID_FEAT_ECX_CX16         1L << 0x0D,
#define    CPUID_FEAT_ECX_XTPR         1L << 0x0E,
#define    CPUID_FEAT_ECX_PDCM         1L << 0x0F,
#define    CPUID_FEAT_ECX_PCID         1L << 0x10,
#define    CPUID_FEAT_ECX_DCA          1L << 0x11,
#define    CPUID_FEAT_ECX_SSE4_1       1L << 0x12,
#define    CPUID_FEAT_ECX_SSE4_2       1L << 0x13,
#define    CPUID_FEAT_ECX_X2APIC       1L << 0x14,
#define    CPUID_FEAT_ECX_MOVBE        1L << 0x15,
#define    CPUID_FEAT_ECX_POPCNT       1L << 0x16,
#define    CPUID_FEAT_ECX_TSC          1L << 0x17,
#define    CPUID_FEAT_ECX_AES          1L << 0x18,
#define    CPUID_FEAT_ECX_XSAVE        1L << 0x19,
#define    CPUID_FEAT_ECX_OSXSAVE      1L << 0x1A,
#define    CPUID_FEAT_ECX_AVX          1L << 0x1B,
#define    CPUID_FEAT_ECX_F16C         1L << 0x1C,
#define    CPUID_FEAT_ECX_RDRAND       1L << 0x1D,
#define    CPUID_FEAT_ECX_HYPERVISOR   1L << 0x1E,

#define    CPUID_FEAT_EDX_FPU          1L << 0x20,
#define    CPUID_FEAT_EDX_VME          1L << 0x21,
#define    CPUID_FEAT_EDX_DE           1L << 0x22,
#define    CPUID_FEAT_EDX_PSE          1L << 0x23,
#define    CPUID_FEAT_EDX_TSC          1L << 0x24,
#define    CPUID_FEAT_EDX_MSR          1L << 0x25,
#define    CPUID_FEAT_EDX_PAE          1L << 0x26,
#define    CPUID_FEAT_EDX_MCE          1L << 0x27,
#define    CPUID_FEAT_EDX_CX8          1L << 0x28,
#define    CPUID_FEAT_EDX_APIC         1L << 0x29,
#define    CPUID_FEAT_EDX_SEP          1L << 0x2B,
#define    CPUID_FEAT_EDX_MTRR         1L << 0x2C,
#define    CPUID_FEAT_EDX_PGE          1L << 0x2D,
#define    CPUID_FEAT_EDX_MCA          1L << 0x2E,
#define    CPUID_FEAT_EDX_CMOV         1L << 0x2F,
#define    CPUID_FEAT_EDX_PAT          1L << 0x30,
#define    CPUID_FEAT_EDX_PSE36        1L << 0x31,
#define    CPUID_FEAT_EDX_PSN          1L << 0x32,
#define    CPUID_FEAT_EDX_CLFLUSH      1L << 0x33,
#define    CPUID_FEAT_EDX_DS           1L << 0x34,
#define    CPUID_FEAT_EDX_ACPI         1L << 0x35,
#define    CPUID_FEAT_EDX_MMX          1L << 0x36,
#define    CPUID_FEAT_EDX_FXSR         1L << 0x37,
#define    CPUID_FEAT_EDX_SSE          1L << 0x38,
#define    CPUID_FEAT_EDX_SSE2         1L << 0x39,
#define    CPUID_FEAT_EDX_SS           1L << 0x3A,
#define    CPUID_FEAT_EDX_HTT          1L << 0x3B,
#define    CPUID_FEAT_EDX_TM           1L << 0x3C,
#define    CPUID_FEAT_EDX_IA64         1L << 0x3D,
#define    CPUID_FEAT_EDX_PBE          1L << 0x3E
