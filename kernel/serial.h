/**
	@File:			serial.h
	@Author:		Ihsoh
	@Date:			2015-02-26
	@Description:
*/

#ifndef	_SERIAL_H_
#define	_SERIAL_H_

#include "types.h"

#define	PORT_COM1	0x3f8
#define	PORT_COM2	0x2f8
#define	PORT_COM3	0x3e8
#define	PORT_COM4	0x2e8

extern
BOOL
SrlInit(void);

extern
BOOL
SrlWrite(	IN uint16 port,
			IN int8 c);

extern
BOOL
SrlWriteBuffer(IN uint16 port,
					IN int8 * buffer,
					IN uint32 count);

extern
BOOL
SrlHasData(IN uint16 port);

extern
BOOL
SrlRead(IN uint16 port,
			OUT int8 * c);

extern
BOOL
SrlInit(void);

#endif
