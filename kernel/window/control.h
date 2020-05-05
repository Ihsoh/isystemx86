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
#include "event.h"

#define	CONTROL_BUTTON			0x00000001
#define	CONTROL_LABEL			0x00000002
#define	CONTROL_LIST			0x00000003
#define	CONTROL_PROGRESS		0x00000004
#define	CONTROL_SCROLL			0x00000005
#define	CONTROL_EDIT			0x00000006
#define	CONTROL_DRIVER_LIST		0x00000007
#define	CONTROL_FILE_LIST		0x00000008
#define	CONTROL_VSCROLL			0x00000009

#define	CONTROL_ID_LIMIT	0x0000ffff

#define	IS_CONTROL_ID(_cid)	((_cid) <= CONTROL_ID_LIMIT)

#define	CONTROL_INVALID_ID	0xffffffff

#define	WINDOW_DEFBGCOLOR	0xffffffff
#define	WINDOW_BORDERCOLOR	0xff000000

#define	GET_CONTROL_WIDTH(_c)	\
	(CtrlGetWidth((_c)))

#define	GET_CONTROL_HEIGHT(_c)	\
	(CtrlGetHeight((_c)))

typedef void (* ControlEvent)(uint32 id, uint32 type, void * param);

typedef struct
{
	uint32	id;
	int32	type;
	int32	uwid;							// User Window ID。
	int32	uwcid;							// User Window Control ID。
} Control, * ControlPtr;

extern
void
CtrlDummyEvent(uint32 id, uint32 type, void * param);

extern
int32
CtrlNewControlId(void);

extern
uint32
CtrlGetWidth(IN void * c);

extern
uint32
CtrlGetHeight(IN void * c);

#endif
