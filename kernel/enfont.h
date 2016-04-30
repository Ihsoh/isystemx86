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

#define	ENFONT_WIDTH	(__enfont_width)
#define	ENFONT_HEIGHT	(__enfont_height)

extern uint32 __enfont_width;
extern uint32 __enfont_height;

extern
uint8 *
EnfntGetFontDataPtr(void);

extern
uint8 *
EnfntGetFontData(IN uint8 chr);

extern
uint8 *
EnfntGetFontXDataPtr(void);

extern
void
EnfntInit(void);

extern
BOOL
EnfntIsEnabled(void);

#endif
