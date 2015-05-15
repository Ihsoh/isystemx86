/**
	@File:			kmpool.h
	@Author:		Ihsoh
	@Date:			2015-05-14
	@Description:
		内核的内存池。
*/

#ifndef	_KMPOOL_H_
#define	_KMPOOL_H_

#include "types.h"

extern
BOOL
kmpool_init(void);

extern
void *
kmpool_malloc(IN uint32 size);

extern
void *
kmpool_calloc(	IN uint32 count,
				IN uint32 size);

extern
void
kmpool_free(IN void * ptr);

#endif
