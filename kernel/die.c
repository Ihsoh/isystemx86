/**
	@File:			die.c
	@Author:		Ihsoh
	@Date:			2014-1-30
	@Description:
		提供系统停机功能。
*/

#include <ilib/string.h>
#include "types.h"
#include "screen.h"
#include "die.h"
#include "kernel.h"
#include "log.h"
#include "apic.h"
#include "pic.h"
#include "lock.h"

/**
	@Function:		die
	@Access:		Public
	@Description:
		系统停机，并显示停机信息。
	@Parameters:
		info, struct die_info *, IN
			停机信息。
	@Return:	
*/
void
die(IN struct die_info * info)
{
	asm volatile ("cli");
	lock_level = 0x7fffffff;
	ApicStopTimer();
	PicMaskAll();
	LogWriteToDisk();
	uint16 us;
	ScrSetConsoleCursor(0, 0);
	ScrSetConsoleCharColor(CC_YELLOW | CBGC_RED);
	for(us = 0; us < ROW * COLUMN; us++)
		ScrPrintChar(' ');
	ScrSetConsoleCursor(0, 0);
	ScrPrintString(	"ISystem X86 Fault Error\n"
				"=======================\n\n");
	ScrPrintString("Code: ");
	printuihex(info->code);
	ScrPrintString(	"\n\n"
				"Infomation:\n    ");
	ScrPrintString(info->info);
	if(VesaIsEnabled())
		ScrFlushScreen();
	asm volatile ("hlt");
}
