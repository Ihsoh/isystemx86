/**
	@File:			klib.h
	@Author:		Ihsoh
	@Date:			2014-12-12
	@Description:
*/

#ifndef	_KLIB_H_
#define	_KLIB_H_

#include "types.h"

extern
BOOL
point_in_rect(	IN int32 point_x,
				IN int32 point_y, 
				IN int32 rect_x,
				IN int32 rect_y,
				IN uint32 rect_w,
				IN uint32 rect_h);

#endif
