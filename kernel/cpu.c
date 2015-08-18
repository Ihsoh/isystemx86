/**
	@File:			cpu.c
	@Author:		Ihsoh
	@Date:			2014-11-18
	@Description:
		提供获取CPU相关信息的功能。
*/

#include "cpu.h"
#include "types.h"
#include <ilib/string.h>

#include "fs/ifs1/fs.h"

#define	CACHE_INFO_LIST_LEN	0x100

static uint8 cpu_src_model = 0, cpu_src_ext_model = 0;
static uint8 cpu_src_family = 0, cpu_src_ext_family = 0;
static uint8 cpu_family = 0, cpu_model = 0, cpu_stepping = 0;
static uint8 cpu_type = 0;
static uint8 cpu_brand_id = 0, cpu_chunks = 0, cpu_count = 0; cpu_apic_id = 0;

static int8 vendor_id_string[13];
static int8 brand_string[49];
static struct CacheInfo cache_info_list[CACHE_INFO_LIST_LEN];
static struct CacheInfo * L1 = NULL;
static struct CacheInfo * L2 = NULL;
static struct CacheInfo * L3 = NULL;
uint32 cpu_feature_ecx = 0, cpu_feature_edx = 0;

/**
	@Function:		fill_cache_info
	@Access:		Private
	@Description:
		填充缓冲区信息。
	@Parameters:
		cache_info, struct CacheInfo *, OUT
			指向缓存信息的结构体。
		level, int32, IN
			值为1时表示L1，值为2时表示L2，值为3时表示L3。
		size, int32, IN
			缓存大小，单位为KB。
		way, int32, IN
			通道数。
		linesize, int32, IN
			吞吐量。
	@Return:	
*/
static
void
fill_cache_info(OUT struct CacheInfo * cache_info, 
				IN int32 level, 
				IN int32 size, 
				IN int32 way, 
				IN int32 linesize)
{
	cache_info->level = level;
	cache_info->size = size;
	cache_info->way = way;
	cache_info->linesize = linesize;
}

/**
	@Function:		init_cache_info_list
	@Access:		Private
	@Description:
		初始化缓存信息列表。
	@Parameters:
	@Return:	
*/
static
void
init_cache_info_list(void)
{
	uint32 ui = 0;
	for(ui = 0; ui < CACHE_INFO_LIST_LEN; ui++)
		fill_cache_info(&cache_info_list[ui], 0, 0, 0, 0);

	fill_cache_info(&cache_info_list[0x06], 1, 8, 4, 32);
	fill_cache_info(&cache_info_list[0x08], 1, 16, 4, 32);
	fill_cache_info(&cache_info_list[0x09], 1, 32, 4, 64);
	fill_cache_info(&cache_info_list[0x0a], 1, 8, 2, 32);
	fill_cache_info(&cache_info_list[0x0c], 1, 16, 4, 32);
	fill_cache_info(&cache_info_list[0x0d], 1, 16, 4, 64);
	fill_cache_info(&cache_info_list[0x0e], 1, 24, 6, 64);

	fill_cache_info(&cache_info_list[0x1d], 2, 128, 2, 64);

	fill_cache_info(&cache_info_list[0x21], 2, 256, 8, 64);
	fill_cache_info(&cache_info_list[0x22], 3, 512, 4, 64);
	fill_cache_info(&cache_info_list[0x23], 3, 1024, 8, 64);
	fill_cache_info(&cache_info_list[0x24], 2, 1024, 16, 64);
	fill_cache_info(&cache_info_list[0x25], 3, 2048, 8, 64);
	fill_cache_info(&cache_info_list[0x29], 3, 4096, 8, 64);
	fill_cache_info(&cache_info_list[0x2c], 1, 32, 8, 64);

	fill_cache_info(&cache_info_list[0x30], 1, 32, 8, 64);

	fill_cache_info(&cache_info_list[0x41], 2, 128, 4, 32);
	fill_cache_info(&cache_info_list[0x42], 2, 256, 4, 32);
	fill_cache_info(&cache_info_list[0x43], 2, 512, 4, 32);
	fill_cache_info(&cache_info_list[0x44], 2, 1024, 4, 32);
	fill_cache_info(&cache_info_list[0x45], 2, 2048, 4, 32);
	fill_cache_info(&cache_info_list[0x46], 3, 4096, 4, 64);
	fill_cache_info(&cache_info_list[0x47], 3, 8192, 8, 64);
	fill_cache_info(&cache_info_list[0x48], 2, 3072, 12, 64);
	//0x49 - Cache
	//3rd-level cache: 4MB, 16-way set associative, 64-byte line size (Intel Xeon processor MP, Family 0FH, Model 06H);
	//2nd-level cache: 4 MByte, 16-way set associative, 64 byte line size
	fill_cache_info(&cache_info_list[0x4a], 3, 6144, 12, 64);
	fill_cache_info(&cache_info_list[0x4b], 3, 8192, 16, 64);
	fill_cache_info(&cache_info_list[0x4c], 3, 12288, 12, 64);
	fill_cache_info(&cache_info_list[0x4d], 3, 16384, 16, 64);
	fill_cache_info(&cache_info_list[0x4e], 3, 6144, 24, 64);

	fill_cache_info(&cache_info_list[0x60], 1, 16, 8, 64);
	fill_cache_info(&cache_info_list[0x66], 1, 8, 4, 64);
	fill_cache_info(&cache_info_list[0x67], 1, 16, 4, 64);
	fill_cache_info(&cache_info_list[0x68], 1, 32, 4, 64);

	fill_cache_info(&cache_info_list[0x78], 2, 1024, 4, 64);
	fill_cache_info(&cache_info_list[0x79], 2, 128, 8, 64);
	fill_cache_info(&cache_info_list[0x7a], 2, 256, 8, 64);
	fill_cache_info(&cache_info_list[0x7b], 2, 512, 8, 64);
	fill_cache_info(&cache_info_list[0x7c], 2, 1024, 8, 64);
	fill_cache_info(&cache_info_list[0x7d], 2, 2048, 8, 64);
	fill_cache_info(&cache_info_list[0x7f], 2, 512, 2, 64);

	fill_cache_info(&cache_info_list[0x80], 2, 512, 8, 64);
	fill_cache_info(&cache_info_list[0x82], 2, 256, 8, 32);
	fill_cache_info(&cache_info_list[0x83], 2, 512, 8, 32);
	fill_cache_info(&cache_info_list[0x84], 2, 1024, 8, 32);
	fill_cache_info(&cache_info_list[0x85], 2, 2048, 8, 32);
	fill_cache_info(&cache_info_list[0x86], 2, 512, 4, 64);
	fill_cache_info(&cache_info_list[0x87], 2, 1024, 8, 64);

	fill_cache_info(&cache_info_list[0xd0], 3, 512, 4, 64);
	fill_cache_info(&cache_info_list[0xd1], 3, 1024, 4, 64);
	fill_cache_info(&cache_info_list[0xd2], 3, 2048, 4, 64);
	fill_cache_info(&cache_info_list[0xd6], 3, 1024, 8, 64);
	fill_cache_info(&cache_info_list[0xd7], 3, 2048, 8, 64);
	fill_cache_info(&cache_info_list[0xd8], 3, 4096, 8, 64);
	fill_cache_info(&cache_info_list[0xdc], 3, 1536, 12, 64);
	fill_cache_info(&cache_info_list[0xdd], 3, 3072, 12, 64);
	fill_cache_info(&cache_info_list[0xde], 3, 6144, 12, 64);

	fill_cache_info(&cache_info_list[0xe2], 3, 2048, 16, 64);
	fill_cache_info(&cache_info_list[0xe3], 3, 4096, 16, 64);
	fill_cache_info(&cache_info_list[0xe4], 3, 8192, 16, 64);
	fill_cache_info(&cache_info_list[0xea], 3, 12288, 24, 64);
	fill_cache_info(&cache_info_list[0xeb], 3, 18432, 24, 64);
	fill_cache_info(&cache_info_list[0xec], 3, 24576, 24, 64);

}

/**
	@Function:		init_cpu
	@Access:		Public
	@Description:
		获取CPU信息。
	@Parameters:
	@Return:
*/
void 
init_cpu(void)
{
	uint32 eax, ebx, ecx, edx;
	uint32 ui, ui1;

	//获取制造商标识字符串
	asm volatile (
		"pushal\n\t"
		"movl	$0, %%eax\n\t"
		"cpuid\n\t"
		"pushl 	%%ebx\n\t"
		"pushl 	%%ecx\n\t"
		"pushl 	%%edx\n\t"
		"popl 	%2\n\t"
		"popl 	%1\n\t"
		"popl 	%0\n\t"
		"popal\n\t"
	:
	:"m"(ebx), "m"(ecx), "m"(edx));
	memcpy(vendor_id_string + 0, &ebx, 4);
	memcpy(vendor_id_string + 4, &edx, 4);
	memcpy(vendor_id_string + 8, &ecx, 4);
	vendor_id_string[12] = '\0';

	//获取CPU商标信息
	uint32 param = 0x80000002;
	for(ui = 0; ui < 3; ui++, param++)
	{
		asm volatile (
			"pushal\n\t"
			"movl	%0, %%eax\n\t"
			"cpuid\n\t"
			"pushl 	%%eax\n\t"
			"pushl 	%%ebx\n\t"
			"pushl 	%%ecx\n\t"
			"pushl 	%%edx\n\t"
			"popl 	%4\n\t"
			"popl 	%3\n\t"
			"popl 	%2\n\t"
			"popl 	%1\n\t"
			"popal\n\t"
		:
		:"m"(param), "m"(eax), "m"(ebx), "m"(ecx), "m"(edx));
		memcpy(brand_string + ui * 16 + 0, &eax, 4);
		memcpy(brand_string + ui * 16 + 4, &ebx, 4);
		memcpy(brand_string + ui * 16 + 8, &ecx, 4);
		memcpy(brand_string + ui * 16 + 12, &edx, 4);
	}

	//获取CPU支持的特性。
	//通过 CPUID EAX=1。
	asm volatile (
			"pushal\n\t"
			"movl	$1, %%eax\n\t"
			"cpuid\n\t"
			"pushl 	%%eax\n\t"
			"pushl 	%%ebx\n\t"
			"pushl 	%%ecx\n\t"
			"pushl 	%%edx\n\t"
			"popl 	%3\n\t"
			"popl 	%2\n\t"
			"popl 	%1\n\t"
			"popl 	%0\n\t"
			"popal\n\t"
		:
		:"m"(eax), "m"(ebx), "m"(ecx), "m"(edx));
	cpu_stepping = (uint8)(eax & 0x0000000f);
	cpu_src_model = (uint8)((eax & 0x000000f0) >> 4);
	cpu_src_family = (uint8)((eax & 0x00000f00) >> 8);
	cpu_type = (uint8)((eax & 0x00003000) >> 12);
	cpu_src_ext_model = (uint8)((eax & 0x000f0000) >> 16);
	cpu_src_ext_family = (uint8)((eax & 0x0ff00000) >> 20);
	if(cpu_src_family != 0x0f)
		cpu_family = cpu_src_family;
	else
		cpu_family = cpu_src_ext_family + cpu_src_family;
	if(cpu_src_family == 0x06 || cpu_src_family == 0x0f)
		cpu_model = (cpu_src_ext_model << 4) + cpu_src_model;
	else
		cpu_model = cpu_src_model;
	cpu_brand_id = (uint8)ebx;
	cpu_chunks = (uint8)(ebx >> 8);
	cpu_count = (uint8)(ebx >> 16);
	cpu_apic_id = (uint8)(ebx >> 24);
	cpu_feature_ecx = ecx;
	cpu_feature_edx = edx;

	//获取CPU缓存信息，通过 CPUID EAX=2。
	init_cache_info_list();
	asm volatile (
			"pushal\n\t"
			"movl	$2, %%eax\n\t"
			"cpuid\n\t"
			"pushl 	%%eax\n\t"
			"pushl 	%%ebx\n\t"
			"pushl 	%%ecx\n\t"
			"pushl 	%%edx\n\t"
			"popl 	%3\n\t"
			"popl 	%2\n\t"
			"popl 	%1\n\t"
			"popl 	%0\n\t"
			"popal\n\t"
		:
		:"m"(eax), "m"(ebx), "m"(ecx), "m"(edx));
	uint8 cache_bytes[16];
	memcpy(cache_bytes + 0, &eax, 4);
	memcpy(cache_bytes + 4, &ebx, 4);
	memcpy(cache_bytes + 8, &ecx, 4);
	memcpy(cache_bytes + 12, &edx, 4);
	for(ui = 1; ui < 16; ui++)
	{
		switch(cache_info_list[cache_bytes[ui]].level)
		{
			case 1:
				L1 = &cache_info_list[cache_bytes[ui]];
				break;
			case 2:
				L2 = &cache_info_list[cache_bytes[ui]];
				break;
			case 3:
				L3 = &cache_info_list[cache_bytes[ui]];
				break;
			default:
				
				break;
		}
	}

	//获取CPU缓存信息，通过 CPUID EAX=4。
	for(ui = 0; ui < 10; ui++)
	{
		for(ui1 = 0; ui1 < 0x1000; ui1++)
		{
			asm volatile (
				"pushal\n\t"
				"movl	$4, %%eax\n\t"
				"cpuid\n\t"
				"pushl 	%%eax\n\t"
				"pushl 	%%ebx\n\t"
				"pushl 	%%ecx\n\t"
				"pushl 	%%edx\n\t"
				"popl 	%3\n\t"
				"popl 	%2\n\t"
				"popl 	%1\n\t"
				"popl 	%0\n\t"
				"popal\n\t"
			:
			:"m"(eax), "m"(ebx), "m"(ecx), "m"(edx), "c"(ui));
			if(eax & 0x1f)
				break;
		}
		if(ui1 != 0x1000)
		{
			uint32 size = 	(((ebx & 0xffc00000) >> 22) + 1)
							* (((ebx & 0x003ff000) >> 12) + 1)
							* (((ebx & 0x00000fff) >> 0) + 1)
							* (ecx + 1);
		}
	}

	//获取CPU L2信息，通过 CPUID EAX=0x80000006。
	asm volatile (
			"pushal\n\t"
			"movl	$0x80000006, %%eax\n\t"
			"cpuid\n\t"
			"pushl 	%%eax\n\t"
			"pushl 	%%ebx\n\t"
			"pushl 	%%ecx\n\t"
			"pushl 	%%edx\n\t"
			"popl 	%3\n\t"
			"popl 	%2\n\t"
			"popl 	%1\n\t"
			"popl 	%0\n\t"
			"popal\n\t"
		:
		:"m"(eax), "m"(ebx), "m"(ecx), "m"(edx));
	uint32 L2_size_kb = (ecx & 0xffff0000) >> 16;
}

/**
	@Function:		get_vendor_id_string
	@Access:		Public
	@Description:
		获取CPU制造商名称。
	@Parameters:
		buffer, int8 *, OUT
			指向用于储存CPU制造商名称的缓冲区。
	@Return:
*/
void
get_vendor_id_string(OUT int8 * buffer)
{
	strcpy(buffer, vendor_id_string);
}

/**
	@Function:		get_brand_string
	@Access:		Public
	@Description:
		获取CPU商标。
	@Parameters:
		buffer, int8 *, OUT
			指向用于储存CPU商标的缓冲区。
	@Return:	
*/
void
get_brand_string(OUT int8 * buffer)
{
	strcpy(buffer, brand_string);
}

/**
	@Function:		get_cpu_L1
	@Access:		Public
	@Description:
		获取CPU的L1缓存信息。
	@Parameters:
		cache_info, struct CacheInfo *, OUT
			指向缓存信息结构体的指针。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。	
*/
BOOL
get_cpu_L1(OUT struct CacheInfo * cache_info)
{
	if(L1 == NULL)
		return FALSE;
	memcpy(cache_info, L1, sizeof(struct CacheInfo));
	return TRUE;
}

/**
	@Function:		get_cpu_L2
	@Access:		Public
	@Description:
		获取CPU的L2缓存信息。
	@Parameters:
		cache_info, struct CacheInfo *, OUT
			指向缓存信息结构体的指针。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。	
*/
BOOL
get_cpu_L2(OUT struct CacheInfo * cache_info)
{
	if(L2 == NULL)
		return FALSE;
	memcpy(cache_info, L2, sizeof(struct CacheInfo));
	return TRUE;
}

/**
	@Function:		get_cpu_L3
	@Access:		Public
	@Description:
		获取CPU的L3缓存信息。
	@Parameters:
		cache_info, struct CacheInfo *, OUT
			指向缓存信息结构体的指针。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。	
*/
BOOL
get_cpu_L3(OUT struct CacheInfo * cache_info)
{
	if(L3 == NULL)
		return FALSE;
	memcpy(cache_info, L3, sizeof(struct CacheInfo));
	return TRUE;
}

/**
	@Function:		cpu_write_to_file
	@Access:		Public
	@Description:
		把 CPU 信息写入文件。
	@Parameters:
		path, const int8 *, IN
			文件路径。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。	
*/
BOOL
cpu_write_to_file(IN const int8 * path)
{
	FILE * fptr = open_file(path, FILE_MODE_WRITE | FILE_MODE_APPEND);
	if(fptr == NULL)
		return FALSE;
	int8 buffer[64 * 1024];
	sprintf_s(	buffer,
				sizeof(buffer),
				"{\n"
				"  VendorName:\"%s\",\n"
				"  BrandName:\"%s\",\n"
				"  Feature0Hex:\"%X\",\n"
				"  Feature1Hex:\"%X\",\n"
				"  Type:\"%X\",\n"
				"  BrandID:\"%X\",\n"
				"  Chunks:\"%X\",\n"
				"  Count:\"%X\",\n"
				"  APICID:\"%X\",\n"
				"  Family:\"%X\",\n"
				"  Model:\"%X\",\n"
				"  Stepping:\"%X\",\n"
				"}\n",
				vendor_id_string,
				brand_string,
				cpu_feature_ecx,
				cpu_feature_edx,
				cpu_type,
				cpu_brand_id,
				cpu_chunks,
				cpu_count,
				cpu_apic_id,
				cpu_family,
				cpu_model,
				cpu_stepping);
	write_file(fptr, buffer, strlen(buffer));
	close_file(fptr);
	return TRUE;
}
