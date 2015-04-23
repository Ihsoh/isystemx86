#include "lib.h"
#include "types.h"

void *
encl_lib_memcpy(OUT void * dst,
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

void *
encl_lib_memset(OUT void * s, 
				IN int32 ch,
				IN uint32 n)
{
	uint8 * carr = (uint8*)s;
	uint32 ui;
	for(ui = 0; ui < n; ui++)
		carr[ui] = ch;
	return s;
}
