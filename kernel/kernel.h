/**
	@File:			kernel.h
	@Author:		Ihsoh
	@Date:			2014-7-26
	@Description:
*/

#ifndef	_KERNEL_H_
#define	_KERNEL_H_

#include "lock.h"

#define	SCALL_SCREEN		0
#define	SCALL_KEYBOARD		1
#define	SCALL_FS			2
#define	SCALL_SYSTEM		3
#define	SCALL_MOUSE			4

extern
BOOL
kernel_is_knltask(void);

extern
int32
kernel_get_current_tid(void);

extern
void
get_mouse_position(	OUT int32 * x,
					OUT int32 * y);

extern
BOOL
is_mouse_left_button_down(void);

extern
BOOL
is_mouse_right_button_down(void);

extern
void
enable_flush_screen(void);

extern
void
disable_flush_screen(void);

extern
void
shutdown_system(void);

extern
void
reboot_system(void);

DEFINE_LOCK_EXTERN(kernel)

#endif
