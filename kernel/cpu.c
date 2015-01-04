/**
	@File:			cpu.c
	@Author:		Ihsoh
	@Date:			2014-11-18
	@Description:
		提供获取CPU相关信息的功能。
*/

#include "cpu.h"
#include "types.h"
#include <string.h>

#define	CACHE_INFO_LIST_LEN	0x100

static int8 vendor_id_string[13];
static int8 brand_string[49];
static struct CacheInfo cache_info_list[CACHE_INFO_LIST_LEN];
static struct CacheInfo * L1 = NULL;
static struct CacheInfo * L2 = NULL;
static struct CacheInfo * L3 = NULL;
uint32 cpu_feature_ecx, cpu_feature_edx;

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
	fill_cache_info(&cache_info_list[0x0a], 1, 8, 2, 32);
	fill_cache_info(&cache_info_list[0x0c], 1, 16, 4, 32);
	fill_cache_info(&cache_info_list[0x2c], 1, 32, 8, 64);
	fill_cache_info(&cache_info_list[0x30], 1, 32, 8, 64);
	fill_cache_info(&cache_info_list[0x60], 1, 16, 8, 64);
	fill_cache_info(&cache_info_list[0x66], 1, 8, 4, 64);
	fill_cache_info(&cache_info_list[0x67], 1, 16, 4, 64);
	fill_cache_info(&cache_info_list[0x68], 1, 32, 4, 64);
	fill_cache_info(&cache_info_list[0x39], 2, 128, 4, 64);
	fill_cache_info(&cache_info_list[0x3b], 2, 128, 2, 64);
	fill_cache_info(&cache_info_list[0x3c], 2, 256, 4, 64);
	fill_cache_info(&cache_info_list[0x41], 2, 128, 4, 32);
	fill_cache_info(&cache_info_list[0x42], 2, 256, 4, 32);
	fill_cache_info(&cache_info_list[0x43], 2, 512, 4, 32);
	fill_cache_info(&cache_info_list[0x44], 2, 1024, 4, 32);
	fill_cache_info(&cache_info_list[0x45], 2, 2048, 4, 32);
	fill_cache_info(&cache_info_list[0x79], 2, 128, 4, 64);
	fill_cache_info(&cache_info_list[0x7a], 2, 256, 8, 64);
	fill_cache_info(&cache_info_list[0x7b], 2, 512, 8, 64);
	fill_cache_info(&cache_info_list[0x7c], 2, 1024, 8, 64);
	fill_cache_info(&cache_info_list[0x82], 2, 256, 8, 32);
	fill_cache_info(&cache_info_list[0x83], 2, 512, 8, 32);
	fill_cache_info(&cache_info_list[0x84], 2, 1024, 8, 32);
	fill_cache_info(&cache_info_list[0x85], 2, 2048, 8, 32);
	fill_cache_info(&cache_info_list[0x86], 2, 512, 4, 64);
	fill_cache_info(&cache_info_list[0x87], 2, 1024, 8, 64);
	fill_cache_info(&cache_info_list[0x22], 3, 512, 4, 64);
	fill_cache_info(&cache_info_list[0x23], 3, 1024, 8, 64);
	fill_cache_info(&cache_info_list[0x25], 3, 2048, 8, 64);
	fill_cache_info(&cache_info_list[0x29], 3, 4096, 8, 64);
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
	uint32 ui;

	//获取制造商标识字符串
	asm volatile (
		"pushal\n\t"
		"movl	$0, %%eax\n\t"
		"cpuid\n\t"
		"movl	%%ebx, %0\n\t"
		"movl	%%ecx, %1\n\t"
		"movl	%%edx, %2\n\t"
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
			"movl	%%eax, %1\n\t"
			"movl	%%ebx, %2\n\t"
			"movl	%%ecx, %3\n\t"
			"movl	%%edx, %4\n\t"
			"popal\n\t"
		:
		:"m"(param), "m"(eax), "m"(ebx), "m"(ecx), "m"(edx));
		memcpy(brand_string + ui * 16 + 0, &eax, 4);
		memcpy(brand_string + ui * 16 + 4, &ebx, 4);
		memcpy(brand_string + ui * 16 + 8, &ecx, 4);
		memcpy(brand_string + ui * 16 + 12, &edx, 4);
	}

	//获取CPU支持的特性
	asm volatile (
		"pushal\n\t"
		"movl	$1, %%eax\n\t"
		"cpuid\n\t"
		"movl	%%ecx, %0\n\t"
		"movl	%%edx, %1\n\t"
		"popal\n\t"
	:
	:"m"(ecx), "m"(edx));
	cpu_feature_ecx = ecx;
	cpu_feature_edx = edx;

	//获取CPU缓存信息
	init_cache_info_list();
	asm volatile (
			"pushal\n\t"
			"movl	$2, %%eax\n\t"
			"cpuid\n\t"
			"movl	%%eax, %0\n\t"
			"movl	%%ebx, %1\n\t"
			"movl	%%ecx, %2\n\t"
			"movl	%%edx, %3\n\t"
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
		}
	}
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
