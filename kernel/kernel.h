/**
	@File:			kernel.h
	@Author:		Ihsoh
	@Date:			2014-7-26
	@Description:
*/

#ifndef	_KERNEL_H_
#define	_KERNEL_H_

#include "syscall/syscall.h"
#include "lock.h"

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
shutdown_system(void);

extern
void
reboot_system(void);

extern
BOOL
reset_syscall(IN int32 tid);

extern
void
free_syscall(IN int32 tid);

extern
BOOL
get_ide_signal(IN BOOL primary);

#define GET_ALL_IDE_SIGNAL() (get_ide_signal(TRUE) || get_ide_signal(FALSE))

extern
void
clear_ide_signal(IN BOOL primary);

#define CLEAR_ALL_IDE_SIGNAL() { clear_ide_signal(TRUE); clear_ide_signal(FALSE); }

extern
void
reset_ide_signal(IN BOOL primary);

#define RESET_ALL_IDE_SIGNAL() { reset_ide_signal(TRUE); reset_ide_signal(FALSE); }

DEFINE_LOCK_EXTERN(kernel)

#endif
