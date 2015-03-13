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
init_cpu(void);

extern
void
get_vendor_id_string(OUT int8 * buffer);

extern
void
get_brand_string(OUT int8 * buffer);

extern
BOOL
get_cpu_L1(OUT struct CacheInfo * cache_info);

extern
BOOL
get_cpu_L2(OUT struct CacheInfo * cache_info);

extern
BOOL
get_cpu_L3(OUT struct CacheInfo * cache_info);

extern
BOOL
cpu_write_to_file(IN const int8 * path);


extern uint32 cpu_feature_ecx, cpu_feature_edx;

//==================== ECX ====================
#define	cpu_feature_sse3()	\
	((cpu_feature_ecx >> 0) & 0x00000001)

#define	cpu_feature_pclmul()	\
	((cpu_feature_ecx >> 1) & 0x00000001)

#define	cpu_feature_dtes64()	\
	((cpu_feature_ecx >> 2) & 0x00000001)

#define	cpu_feature_monitor()	\
	((cpu_feature_ecx >> 3) & 0x00000001)

#define	cpu_feature_ds_cpl()	\
	((cpu_feature_ecx >> 4) & 0x00000001)

#define	cpu_feature_vmx()	\
	((cpu_feature_ecx >> 5) & 0x00000001)

#define	cpu_feature_smx()	\
	((cpu_feature_ecx >> 6) & 0x00000001)

#define	cpu_feature_est()	\
	((cpu_feature_ecx >> 7) & 0x00000001)

#define	cpu_feature_tm2()	\
	((cpu_feature_ecx >> 8) & 0x00000001)

#define	cpu_feature_ssse3()	\
	((cpu_feature_ecx >> 9) & 0x00000001)

#define	cpu_feature_cid()	\
	((cpu_feature_ecx >> 10) & 0x00000001)

#define	cpu_feature_fma()	\
	((cpu_feature_ecx >> 12) & 0x00000001)

#define	cpu_feature_cx16()	\
	((cpu_feature_ecx >> 13) & 0x00000001)

#define	cpu_feature_etprd()	\
	((cpu_feature_ecx >> 14) & 0x00000001)

#define	cpu_feature_pdcm()	\
	((cpu_feature_ecx >> 15) & 0x00000001)

//ECX的位16为保留位。

#define	cpu_feature_pcid()	\
	((cpu_feature_ecx >> 17) & 0x00000001)

#define	cpu_feature_dca()	\
	((cpu_feature_ecx >> 18) & 0x00000001)

#define	cpu_feature_sse4_1()	\
	((cpu_feature_ecx >> 19) & 0x00000001)

#define	cpu_feature_sse4_2()	\
	((cpu_feature_ecx >> 20) & 0x00000001)

#define	cpu_feature_x2apic()	\
	((cpu_feature_ecx >> 21) & 0x00000001)

#define	cpu_feature_movbe()	\
	((cpu_feature_ecx >> 22) & 0x00000001)

#define	cpu_feature_popcnt()	\
	((cpu_feature_ecx >> 23) & 0x00000001)

#define	cpu_feature_tsc_deadline()	\
	((cpu_feature_ecx >> 24) & 0x00000001)

#define	cpu_feature_aes()	\
	((cpu_feature_ecx >> 25) & 0x00000001)

#define	cpu_feature_xsave()	\
	((cpu_feature_ecx >> 26) & 0x00000001)

#define	cpu_feature_osxsave()	\
	((cpu_feature_ecx >> 27) & 0x00000001)

#define	cpu_feature_avx()	\
	((cpu_feature_ecx >> 28) & 0x00000001)

#define	cpu_feature_f16c()	\
	((cpu_feature_ecx >> 29) & 0x00000001)

#define	cpu_feature_rdrand()	\
	((cpu_feature_ecx >> 30) & 0x00000001)

//ECX的位31为未被使用。

//==================== EDX ====================
#define	cpu_feature_fpu()	\
	((cpu_feature_edx >> 0) & 0x00000001)

#define	cpu_feature_vme()	\
	((cpu_feature_edx >> 1) & 0x00000001)

#define	cpu_feature_de()	\
	((cpu_feature_edx >> 2) & 0x00000001)

#define	cpu_feature_pse()	\
	((cpu_feature_edx >> 3) & 0x00000001)

#define	cpu_feature_tsc()	\
	((cpu_feature_edx >> 4) & 0x00000001)

#define	cpu_feature_msr()	\
	((cpu_feature_edx >> 5) & 0x00000001)

#define	cpu_feature_pae()	\
	((cpu_feature_edx >> 6) & 0x00000001)

#define	cpu_feature_mce()	\
	((cpu_feature_edx >> 7) & 0x00000001)

#define	cpu_feature_cxb()	\
	((cpu_feature_edx >> 8) & 0x00000001)

#define	cpu_feature_apic()	\
	((cpu_feature_edx >> 9) & 0x00000001)

//EDX的位10为保留位。

#define	cpu_feature_sep()	\
	((cpu_feature_edx >> 11) & 0x00000001)

#define	cpu_feature_mtrr()	\
	((cpu_feature_edx >> 12) & 0x00000001)

#define	cpu_feature_pge()	\
	((cpu_feature_edx >> 13) & 0x00000001)

#define	cpu_feature_mca()	\
	((cpu_feature_edx >> 14) & 0x00000001)

#define	cpu_feature_cmov()	\
	((cpu_feature_edx >> 15) & 0x00000001)

#define	cpu_feature_pat()	\
	((cpu_feature_edx >> 16) & 0x00000001)

#define	cpu_feature_pse36()	\
	((cpu_feature_edx >> 17) & 0x00000001)

#define	cpu_feature_pn()	\
	((cpu_feature_edx >> 18) & 0x00000001)

#define	cpu_feature_clf()	\
	((cpu_feature_edx >> 19) & 0x00000001)

//EDX的位20为保留位。

#define	cpu_feature_dtes()	\
	((cpu_feature_edx >> 21) & 0x00000001)

#define	cpu_feature_acpi()	\
	((cpu_feature_edx >> 22) & 0x00000001)

#define	cpu_feature_mmx()	\
	((cpu_feature_edx >> 23) & 0x00000001)

#define	cpu_feature_fxsr()	\
	((cpu_feature_edx >> 24) & 0x00000001)

#define	cpu_feature_sse()	\
	((cpu_feature_edx >> 25) & 0x00000001)

#define	cpu_feature_sse2()	\
	((cpu_feature_edx >> 26) & 0x00000001)

#define	cpu_feature_ss()	\
	((cpu_feature_edx >> 27) & 0x00000001)

#define	cpu_feature_htt()	\
	((cpu_feature_edx >> 28) & 0x00000001)

#define	cpu_feature_tm1()	\
	((cpu_feature_edx >> 29) & 0x00000001)

//EDX的位30为保留位。

#define	cpu_feature_pbe()	\
	((cpu_feature_edx >> 31) & 0x00000001)

#endif
