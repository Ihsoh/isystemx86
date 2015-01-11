/**
	@File:			die.h
	@Author:		Ihsoh
	@Date:			2014-1-30
	@Description:
*/

#ifndef	_DIE_H_
#define	_DIE_H_

#include "types.h"

#define	MAX_INFO_LENGTH 200

struct die_info
{
	uint32	code;
	int8	info[MAX_INFO_LENGTH];
};

extern
void
die(IN struct die_info * info);

#define	fill_info(info, c, txt)	{\
										(info).code = (c);\
										strcpy((info).info, (txt));\
									}

	#define	DC_NOIMPLEMENT_INT	0x00000001
	#define	DI_NOIMPLEMENT_INT	"No implement interrupt"
	
	#define	DC_GP_INT			0x00000002
	#define	DI_GP_INT			"Global protected exception"

	#define	DC_INIT_TIMER		0x00000003
	#define	DI_INIT_TIMER		"Failed to init timer"

	#define	DC_INIT_KEYBOARD	0x00000004
	#define	DI_INIT_KEYBOARD	"Failed to init keyboard"

	#define	DC_INIT_IDE			0x00000005
	#define	DI_INIT_IDE			"Failed to init ide"

	#define	DC_INIT_SCALL		0x00000006
	#define	DI_INIT_SCALL		"Failed to init system call"

	#define	DC_INIT_SCREEN		0x00000007
	#define	DI_INIT_SCREEN		"Failed to init screen"

	#define	DC_INIT_FPU			0x00000008
	#define	DI_INIT_FPU			"Failed to init fpu"

	#define	DC_INIT_NOIMPL		0x00000009
	#define	DI_INIT_NOIMPL		"Failed to init NOIMPL"

	#define	DC_INIT_GP			0x0000000A
	#define	DI_INIT_GP			"Failed to init GP"
	
	#define	DC_INIT_MOUSE		0x0000000B
	#define	DI_INIT_MOUSE		"Failed to init mouse"

	#define	DC_INIT_PF			0x0000000C
	#define	DI_INIT_PF			"Failed to init PF"

	#define	DC_PF_INT			0x0000000D
	#define	DI_PF_INT			"Page Fault"

	#define	DC_INIT_TSKMGR		0x0000000E
	#define	DI_INIT_TSKMGR		"Failed to init task manager"

#endif

