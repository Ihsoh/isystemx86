/**
	@File:			stdlib.c
	@Author:		Ihsoh
	@Date:			2014-1-19
	@Description:
		提供内存分配、释放等杂项功能。
*/

#include "stdlib.h"
#include "types.h"
#include "system.h"

void * malloc(uint num_bytes)
{
	return il_allocm(num_bytes);
}

void * calloc(uint n, uint size)
{
	return il_allocm(n * size);
}

void free(void * ptr)
{
	il_freem(ptr);
}
