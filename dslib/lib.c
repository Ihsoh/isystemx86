#include "lib.h"
#include "types.h"

void *
dsl_lib_memcpy(	OUT void * dst,
				IN const void * src,
				IN uint32 count)
{
	void * r = dst;
	while(count--)
	{
		*(uint8 *)dst = *(uint8 *)src;
		dst = (uint8 *)dst + 1;
		src = (uint8 *)src + 1;
	}
	return r;
}
