/**
	@File:			mouse.h
	@Author:		Ihsoh
	@Date:			2015-05-12
	@Description:
		提供鼠标的功能。
*/

#ifndef	_MOUSE_H_
#define	_MOUSE_H_

#include "types.h"

#define	MOUSE_BUTTON_NONE	0x00000000
#define	MOUSE_BUTTON_LEFT	0x00000001
#define	MOUSE_BUTTON_RIGHT	0x00000002
#define	MOUSE_BUTTON_MIDDLE	0x00000004

extern
BOOL
MouseInit(void);

#endif
