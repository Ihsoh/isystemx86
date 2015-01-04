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
void init_cpu(void);

extern
void get_vendor_id_string(OUT int8 * buffer);

extern
void get_brand_string(OUT int8 * buffer);

extern
BOOL get_cpu_L1(OUT struct CacheInfo * cache_info);

extern
BOOL get_cpu_L2(OUT struct CacheInfo * cache_info);

extern
BOOL get_cpu_L3(OUT struct CacheInfo * cache_info);


extern uint32 cpu_feature_ecx, cpu_feature_edx;

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

//EDX Bit 10 Reserved

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

//EDX Bit 19~22 Reserved

#define	cpu_feature_mmx()	\
	((cpu_feature_edx >> 23) & 0x00000001)

#define	cpu_feature_fxsr()	\
	((cpu_feature_edx >> 24) & 0x00000001)

#define	cpu_feature_xmm()	\
	((cpu_feature_edx >> 25) & 0x00000001)

//EDX Bit 26~31 Reserved

#define	cpu_feature_hyper_threading()	\
	((cpu_feature_edx >> 28) & 0x00000001)

#define	cpu_feature_speed_sted()	\
	((cpu_feature_ecx >> 7) & 0x00000001)

#endif
