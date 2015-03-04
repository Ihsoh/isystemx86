//Filename:		sparams.c
//Author:		Ihsoh
//Date:			2014-8-1
//Descriptor:	System parameters

#include "types.h"
#include <sparams.h>

void system_call(uint func, uint sub_func, struct SParams * sparams)
{
	/*uint addr = (uint)sparams;
	uint f = (func << 16) | (sub_func & 0xFFFF);
	int tid = *(int *)(0x01000000 + 0);
	
	if(sparams != NULL)	
		sparams->tid = tid;

	//向程序的信息头写入系统调用的相关信息
	*(uint *)(0x01000000 + KB(4) + 0) = f;
	*(uint *)(0x01000000 + KB(4) + 4) = addr;
	asm volatile ("int	$0x90");*/
	uint addr = (uint)sparams;
	uint eax = (func << 16) | (sub_func & 0xFFFF);
	int tid = *(int *)(0x01000000 + 0);
	sparams->tid = tid;
	asm volatile(
		"movl	%0, %%eax\n\t"
		"movl	%1, %%ecx\n\t"
		"movl	%2, %%edx\n\t"
		"int	$0x90\n\t"
		:
		:"r"(eax), "r"(tid), "r"(addr)
		:"eax", "ecx", "edx"
	);
}
