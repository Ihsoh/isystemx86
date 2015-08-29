/**
	@File:			control.h
	@Author:		Ihsoh
	@Date:			2015-08-30
	@Description:
		窗体控件。
*/

#ifndef	_WINDOW_CONTROL_H_
#define	_WINDOW_CONTROL_H_

#include "../types.h"

#define	WINDOW_DEFBGCOLOR	0xffffffff

typedef void (* ControlEvent)(int32 id, uint32 type, void * param);

extern
void
__dummy_event(int32 id, uint32 type, void * param);

#endif
