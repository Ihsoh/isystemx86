#ifndef	_LIB_H_
#define	_LIB_H_

#include "types.h"

void *
encl_lib_memcpy(	OUT void * dst,
					IN const void * src,
					IN uint32 count);

void *
encl_lib_memset(OUT void * s, 
				IN int32 ch,
				IN uint32 n);

#endif
