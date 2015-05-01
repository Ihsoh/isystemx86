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
serial_init(void);

extern
BOOL
serial_write(	IN uint16 port,
				IN int8 c);

extern
BOOL
serial_write_buffer(IN uint16 port,
					IN int8 * buffer,
					IN uint32 count);

extern
BOOL
serial_has_data(IN uint16 port);

extern
BOOL
serial_read(IN uint16 port,
			OUT int8 * c);

extern
BOOL
serial_init(void);

#endif
