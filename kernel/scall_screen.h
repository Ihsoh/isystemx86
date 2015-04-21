/**
	@File:			scall_screen.h
	@Author:		Ihsoh
	@Date:			2014-7-28
	@Description:
*/

#ifndef	_SCALL_SCREEN_
#define	_SCALL_SCREEN_

#include "types.h"
#include "sparams.h"

#define	SCALL_PRINT_CHAR			0
#define	SCALL_PRINT_CHAR_P			1
#define	SCALL_PRINT_STR				2
#define	SCALL_PRINT_STR_P			3
#define	SCALL_CLEAR_SCREEN			4
#define	SCALL_SET_CURSOR			5
#define	SCALL_GET_CURSOR			6
#define	SCALL_VESA_IS_VALID			7
#define	SCALL_GET_SCREEN_SIZE		8
#define	SCALL_SET_SCREEN_PIXEL		9
#define	SCALL_GET_SCREEN_PIXEL		10
#define	SCALL_DRAW_SCREEN_IMAGE		11
#define	SCALL_GET_TEXT_SCREEN_SIZE	12
#define	SCALL_LOCK_CURSOR			13
#define	SCALL_UNLOCK_CURSOR			14
#define	SCALL_ENABLE_FLUSH_SCREEN	15
#define	SCALL_DISABLE_FLUSH_SCREEN	16
#define	SCALL_SET_TARGET_SCREEN		17

#define	SCALL_CREATE_WINDOW			18
#define	SCALL_DESTROY_WINDOW		19
#define	SCALL_SHOW_WINDOW			20
#define	SCALL_HIDE_WINDOW			21
#define	SCALL_WINDOW_GET_KEY		22
#define	SCALL_DRAW_WINDOW			23

#define	SCALL_PRINT_ERR_CHAR		24
#define	SCALL_PRINT_ERR_CHAR_P		25
#define	SCALL_PRINT_ERR_STR			26
#define	SCALL_PRINT_ERR_STR_P		27

extern
void
system_call_screen(	IN uint32 func,
					IN uint32 base,
					IN OUT struct SParams * sparams);

#endif
