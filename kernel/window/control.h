/**
	@File:			control.h
	@Author:		Ihsoh
	@Date:			2015-08-30
	@Description:
		窗体控件。
*/

#ifndef	_WINDOW_CONTROL_H_
#define	_WINDOW_CONTROL_H_

#include "../types.h"

#define	CONTROL_BUTTON		1
#define	CONTROL_LABEL		2
#define	CONTROL_LIST		3

#define	CONTROL_ID_LIMIT	0x0000ffff

#define	IS_CONTROL_ID(_cid)	((_cid) <= CONTROL_ID_LIMIT)

#define	WINDOW_DEFBGCOLOR	0xffffffff

typedef void (* ControlEvent)(uint32 id, uint32 type, void * param);

typedef struct
{
	uint32	id;
	int32	type;
} Control, * ControlPtr;

extern
void
__dummy_event(uint32 id, uint32 type, void * param);

extern
int32
__new_control_id(void);

#endif
