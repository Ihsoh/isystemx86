//Filename:		mouse.h
//Author:		Ihsoh
//Date:			2014-11-14
//Descriptor:	Mouse functions

#ifndef	_MOUSE_H_
#define	_MOUSE_H_

#include "types.h"

#include "../../kernel/syscall/syscall.h"

extern void get_mouse_position(int * x, int * y);
#define	il_get_mouse_position(x, y)	(get_mouse_position((x), (y)))
#define	ILGetMousePosition(x, y)	(get_mouse_position((x), (y)))

extern int is_mouse_left_button_down(void);
#define	il_is_mouse_left_button_down()	(is_mouse_left_button_down())
#define	ILGetLeftMouseButtonStatus()	(is_mouse_left_button_down())

extern int is_mouse_right_button_down(void);
#define	il_is_mouse_right_button_down()	(is_mouse_right_button_down())
#define	ILGetRightMouseButtonStatus()	(is_mouse_right_button_down())

#endif
