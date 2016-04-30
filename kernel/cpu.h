/**
	@File:			cpu.h
	@Author:		Ihsoh
	@Date:			2014-11-18
	@Description:
*/

#ifndef	_CPU_H_
#define	_CPU_H_

#include "types.h"

struct CacheInfo
{
	int32 level;
	int32 size;
	int32 way;
	int32 linesize;
};

extern 
void
CpuInit(void);

extern
void
CpuGetVendorIdString(OUT int8 * buffer);

extern
void
CpuGetBrandString(OUT int8 * buffer);

extern
BOOL
CpuGetL1(OUT struct CacheInfo * cache_info);

extern
BOOL
CpuGetL2(OUT struct CacheInfo * cache_info);

extern
BOOL
CpuGetL3(OUT struct CacheInfo * cache_info);

extern
BOOL
CpuWriteInfoToFile(IN const int8 * path);


extern uint32 cpu_feature_ecx, cpu_feature_edx;

//==================== ECX ====================
#define	CPU_FEATURE_SSE3()	\
	((cpu_feature_ecx >> 0) & 0x00000001)

#define	CPU_FEATURE_PCLMUL()	\
	((cpu_feature_ecx >> 1) & 0x00000001)

#define	CPU_FEATURE_DTES64()	\
	((cpu_feature_ecx >> 2) & 0x00000001)

#define	CPU_FEATURE_MONITOR()	\
	((cpu_feature_ecx >> 3) & 0x00000001)

#define	CPU_FEATURE_DS_CPL()	\
	((cpu_feature_ecx >> 4) & 0x00000001)

#define	CPU_FEATURE_VMX()	\
	((cpu_feature_ecx >> 5) & 0x00000001)

#define	CPU_FEATURE_SMX()	\
	((cpu_feature_ecx >> 6) & 0x00000001)

#define	CPU_FEATURE_EST()	\
	((cpu_feature_ecx >> 7) & 0x00000001)

#define	CPU_FEATURE_TM2()	\
	((cpu_feature_ecx >> 8) & 0x00000001)

#define	CPU_FEATURE_SSSE3()	\
	((cpu_feature_ecx >> 9) & 0x00000001)

#define	CPU_FEATURE_CID()	\
	((cpu_feature_ecx >> 10) & 0x00000001)

#define	CPU_FEATURE_FMA()	\
	((cpu_feature_ecx >> 12) & 0x00000001)

#define	CPU_FEATURE_CX16()	\
	((cpu_feature_ecx >> 13) & 0x00000001)

#define	CPU_FEATURE_ETPRD()	\
	((cpu_feature_ecx >> 14) & 0x00000001)

#define	CPU_FEATURE_PDCM()	\
	((cpu_feature_ecx >> 15) & 0x00000001)

//ECX的位16为保留位。

#define	CPU_FEATURE_PCID()	\
	((cpu_feature_ecx >> 17) & 0x00000001)

#define	CPU_FEATURE_DCA()	\
	((cpu_feature_ecx >> 18) & 0x00000001)

#define	CPU_FEATURE_SSE4_1()	\
	((cpu_feature_ecx >> 19) & 0x00000001)

#define	CPU_FEATURE_SSE4_2()	\
	((cpu_feature_ecx >> 20) & 0x00000001)

#define	CPU_FEATURE_X2APIC()	\
	((cpu_feature_ecx >> 21) & 0x00000001)

#define	CPU_FEATURE_MOVBE()	\
	((cpu_feature_ecx >> 22) & 0x00000001)

#define	CPU_FEATURE_POPCNT()	\
	((cpu_feature_ecx >> 23) & 0x00000001)

#define	CPU_FEATURE_TSC_DEADLINE()	\
	((cpu_feature_ecx >> 24) & 0x00000001)

#define	CPU_FEATURE_AES()	\
	((cpu_feature_ecx >> 25) & 0x00000001)

#define	CPU_FEATURE_XSAVE()	\
	((cpu_feature_ecx >> 26) & 0x00000001)

#define	CPU_FEATURE_OSXSAVE()	\
	((cpu_feature_ecx >> 27) & 0x00000001)

#define	CPU_FEATURE_AVX()	\
	((cpu_feature_ecx >> 28) & 0x00000001)

#define	CPU_FEATURE_F16C()	\
	((cpu_feature_ecx >> 29) & 0x00000001)

#define	CPU_FEATURE_RDRAND()	\
	((cpu_feature_ecx >> 30) & 0x00000001)

//ECX的位31为未被使用。

//==================== EDX ====================
#define	CPU_FEATURE_FPU()	\
	((cpu_feature_edx >> 0) & 0x00000001)

#define	CPU_FEATURE_VME()	\
	((cpu_feature_edx >> 1) & 0x00000001)

#define	CPU_FEATURE_DE()	\
	((cpu_feature_edx >> 2) & 0x00000001)

#define	CPU_FEATURE_PSE()	\
	((cpu_feature_edx >> 3) & 0x00000001)

#define	CPU_FEATURE_TSC()	\
	((cpu_feature_edx >> 4) & 0x00000001)

#define	CPU_FEATURE_MSR()	\
	((cpu_feature_edx >> 5) & 0x00000001)

#define	CPU_FEATURE_PAE()	\
	((cpu_feature_edx >> 6) & 0x00000001)

#define	CPU_FEATURE_MCE()	\
	((cpu_feature_edx >> 7) & 0x00000001)

#define	CPU_FEATURE_CXB()	\
	((cpu_feature_edx >> 8) & 0x00000001)

#define	CPU_FEATURE_APIC()	\
	((cpu_feature_edx >> 9) & 0x00000001)

//EDX的位10为保留位。

#define	CPU_FEATURE_SEP()	\
	((cpu_feature_edx >> 11) & 0x00000001)

#define	CPU_FEATURE_MTRR()	\
	((cpu_feature_edx >> 12) & 0x00000001)

#define	CPU_FEATURE_PGE()	\
	((cpu_feature_edx >> 13) & 0x00000001)

#define	CPU_FEATURE_MCA()	\
	((cpu_feature_edx >> 14) & 0x00000001)

#define	CPU_FEATURE_CMOV()	\
	((cpu_feature_edx >> 15) & 0x00000001)

#define	CPU_FEATURE_PAT()	\
	((cpu_feature_edx >> 16) & 0x00000001)

#define	CPU_FEATURE_PSE36()	\
	((cpu_feature_edx >> 17) & 0x00000001)

#define	CPU_FEATURE_PN()	\
	((cpu_feature_edx >> 18) & 0x00000001)

#define	CPU_FEATURE_CLF()	\
	((cpu_feature_edx >> 19) & 0x00000001)

//EDX的位20为保留位。

#define	CPU_FEATURE_DTES()	\
	((cpu_feature_edx >> 21) & 0x00000001)

#define	CPU_FEATURE_ACPI()	\
	((cpu_feature_edx >> 22) & 0x00000001)

#define	CPU_FEATURE_MMX()	\
	((cpu_feature_edx >> 23) & 0x00000001)

#define	CPU_FEATURE_FXSR()	\
	((cpu_feature_edx >> 24) & 0x00000001)

#define	CPU_FEATURE_SSE()	\
	((cpu_feature_edx >> 25) & 0x00000001)

#define	CPU_FEATURE_SSE2()	\
	((cpu_feature_edx >> 26) & 0x00000001)

#define	CPU_FEATURE_SS()	\
	((cpu_feature_edx >> 27) & 0x00000001)

#define	CPU_FEATURE_HTT()	\
	((cpu_feature_edx >> 28) & 0x00000001)

#define	CPU_FEATURE_TM1()	\
	((cpu_feature_edx >> 29) & 0x00000001)

//EDX的位30为保留位。

#define	CPU_FEATURE_PBE()	\
	((cpu_feature_edx >> 31) & 0x00000001)

#endif
