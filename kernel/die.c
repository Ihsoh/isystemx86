/**
	@File:			die.c
	@Author:		Ihsoh
	@Date:			2014-1-30
	@Description:
		提供系统停机功能。
*/

#include <string.h>
#include "types.h"
#include "screen.h"
#include "die.h"
#include "kernel.h"
#include "log.h"

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
	enable_flush_screen();
	asm volatile ("cli");
	write_log_to_disk();
	uint16 us;
	set_cursor(0, 0);
	set_char_color(CC_YELLOW | CBGC_RED);
	for(us = 0; us < ROW * COLUMN; us++)
		print_char(' ');
	set_cursor(0, 0);
	print_str(	"ISystem X86 Fault Error\n"
				"=======================\n\n");
	print_str("Code: ");
	printuihex(info->code);
	print_str(	"\n\n"
				"Infomation:\n    ");
	print_str(info->info);
	if(vesa_is_valid())
		flush_screen();
	asm volatile ("hlt");
}
