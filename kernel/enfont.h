/**
	@File:			enfont.h
	@Author:		Ihsoh
	@Date:			2014-7-24
	@Description:
		英文字体。
*/

#ifndef	_ENFONT_H_
#define	_ENFONT_H_

#include "types.h"

#define	ENFONT_WIDTH	8
#define	ENFONT_HEIGHT	16

extern
uint8 *
get_enfont_ptr(void);

extern
uint8 *
get_enfont(IN uint8 chr);

extern
uint8 *
get_enfontx_ptr(void);

extern
void
init_enfont(void);

extern
BOOL
enfontx_enabled(void);

#endif
