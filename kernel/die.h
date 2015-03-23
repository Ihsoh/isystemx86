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

	#define	DC_NOIMPLEMENT_INT		0x00000001
	#define	DI_NOIMPLEMENT_INT		"Not implemented interrupt(%d, 0x%X)"
	
	#define	DC_GP_INT				0x00000002
	#define	DI_GP_INT				"Global protected exception"

	#define	DC_INIT_TIMER			0x00000003
	#define	DI_INIT_TIMER			"Failed to init timer"

	#define	DC_INIT_KEYBOARD		0x00000004
	#define	DI_INIT_KEYBOARD		"Failed to init keyboard"

	#define	DC_INIT_IDE				0x00000005
	#define	DI_INIT_IDE				"Failed to init ide"

	#define	DC_INIT_SCALL			0x00000006
	#define	DI_INIT_SCALL			"Failed to init system call"

	#define	DC_INIT_SCREEN			0x00000007
	#define	DI_INIT_SCREEN			"Failed to init screen"

	#define	DC_INIT_FPU				0x00000008
	#define	DI_INIT_FPU				"Failed to init fpu"

	#define	DC_INIT_NOIMPL			0x00000009
	#define	DI_INIT_NOIMPL			"Failed to init NOIMPL"

	#define	DC_INIT_GP				0x0000000A
	#define	DI_INIT_GP				"Failed to init GP"
	
	#define	DC_INIT_MOUSE			0x0000000B
	#define	DI_INIT_MOUSE			"Failed to init mouse"

	#define	DC_INIT_PF				0x0000000C
	#define	DI_INIT_PF				"Failed to init PF"

	#define	DC_PF_INT				0x0000000D
	#define	DI_PF_INT				"Page Fault"

	#define	DC_INIT_TSKMGR			0x0000000E
	#define	DI_INIT_TSKMGR			"Failed to init task manager"

	#define DC_INIT_DIV_BY_0		0x0000000F
	#define	DI_INIT_DIV_BY_0		"Failed to init procedure to process exception of dividing by zero"

	#define	DC_DIV_BY_0 			0x00000010
	#define	DI_DIV_BY_0 			"Kernel causes a exception of dividing by zero"

	#define	DC_INIT_BOUND_CHK		0x00000011
	#define	DI_INIT_BOUND_CHK		"Failed to init procedure to process exception of bound check"

	#define	DC_BOUND_CHK			0x00000012
	#define	DI_BOUND_CHK			"Kernel causes a exception of bound check"

	#define	DC_INIT_INVALID_OPC		0x00000013
	#define	DI_INIT_INVALID_OPC		"Failed to init procedure to process exception of invalid opcode"

	#define	DC_INVALID_OPC 			0x00000014
	#define	DI_INVALID_OPC 			"Kernel causes a exception of invalid opcode"

	#define	DC_INIT_INVALID_TSS		0x00000015
	#define DI_INIT_INVALID_TSS		"Failed to init procedure to process exception of invalid tss"

	#define	DC_INVALID_TSS 			0x00000016
	#define	DI_INVALID_TSS 			"Kernel causes a exception of invalid tss"

	#define	DC_INIT_INVALID_SEG		0x00000017
	#define	DI_INIT_INVALID_SEG		"Failed to init procedure to process exception of non-existent segment"

	#define DC_INVALID_SEG 			0x00000018
	#define	DI_INVALID_SEG 			"Kernel causes a exception of non-existent segment"

	#define	DC_INIT_INVALID_STACK	0x00000019
	#define	DI_INIT_INVALID_STACK	"Failed to init procedure to process exception of invalid stack"

	#define	DC_INVALID_STACK 		0x0000001a
	#define	DI_INVALID_STACK 		"Kernel causes a exception of invalid stack"

	#define	DC_INIT_DOUBLE_FAULT	0x0000001b
	#define	DI_INIT_DOUBLE_FAULT	"Failed to init procedure to process exception of double fault"

	#define	DC_DOUBLE_FAULT_KNL		0x0000001c
	#define	DI_DOUBLE_FAULT_KNL		"Kernel causes a exception of double fault"

	#define	DC_DOUBLE_FAULT_TSK		0x0000001d
	#define	DI_DOUBLE_FAULT_TSK		"A task causes a exception of double fault, the id is %d, the name is '%s'"

#endif
