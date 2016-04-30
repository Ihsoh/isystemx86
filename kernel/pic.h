/**
	@File:			pic.h
	@Author:		Ihsoh
	@Date:			2015-03-11
	@Description:
		Intel 8259A。
*/

#ifndef	_PIC_H_
#define	_PIC_H_

#include "types.h"

BOOL
PicInit(void);

BOOL
PicMask(IN uint8 irq);

BOOL
PicUnmask(IN uint8 irq);

BOOL
PicMaskAll(void);

BOOL
PicUnmaskAll(void);

#endif
