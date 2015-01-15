/**
	@File:			stdio.h
	@Author:		Ihsoh
	@Date:			2014-1-19
	@Description:
*/

#ifndef	_STDLIB_H_
#define	_STDLIB_H_

#include <types.h>
extern void * malloc(uint num_bytes);
extern void * calloc(uint n, uint size);
extern void free(void * ptr);

#endif
