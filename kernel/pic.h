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
pic_init(void);

BOOL
pic_mask(IN uint8 irq);

BOOL
pic_unmask(IN uint8 irq);

BOOL
pic_mask_all(void);

BOOL
pic_unmask_all(void);

#endif
