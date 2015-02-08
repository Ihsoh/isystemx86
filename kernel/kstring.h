/**
	@File:			kstring.h
	@Author:		Ihsoh
	@Date:			2015-2-10
	@Description:
*/

#ifndef	_KSTRING_H_
#define	_KSTRING_H_

#include "types.h"

extern
int8 *
split_string(	OUT int8 * dest, 
				IN const int8 * src,
				IN int8 spt,
				IN int32 max);

#endif
