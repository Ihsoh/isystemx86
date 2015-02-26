/**
	@File:			serial.c
	@Author:		Ihsoh
	@Date:			2015-02-26
	@Description:
		串行通信。
*/

#include "serial.h"
#include "types.h"

#define SERIAL_PORT_A 0x3F8
#define SERIAL_PORT_B 0x2F8
#define SERIAL_PORT_C 0x3E8
#define SERIAL_PORT_D 0x2E8

#define SERIAL_IRQ_AC 4
#define SERIAL_IRQ_BD 3

BOOL
serial_init(void)
{
	return TRUE;
}
