//Filename:		mouse.h
//Author:		Ihsoh
//Date:			2014-11-14
//Descriptor:	Mouse functions

#ifndef	_MOUSE_H_
#define	_MOUSE_H_

#include "types.h"

#define	SCALL_MOUSE			4

#define	SCALL_GET_MPOSITION		0
#define	SCALL_GET_MLBSTATE		1
#define	SCALL_GET_MRBSTATE		2

extern void get_mouse_position(int * x, int * y);
#define	il_get_mouse_position(x, y)	(get_mouse_position((x), (y)))

extern int is_mouse_left_button_down(void);
#define	il_is_mouse_left_button_down()	(is_mouse_left_button_down())

extern int is_mouse_right_button_down(void);
#define	il_is_mouse_right_button_down()	(is_mouse_right_button_down())

#endif
