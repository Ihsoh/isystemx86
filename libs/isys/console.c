#include "console.h"
#include "types.h"

#include "ilib/ilib.h"

BOOL
ISYSPrintChar(	IN UINT32 chr,
				IN UINT32 to)
{
	switch(to)
	{
		case ISYS_TO_STDOUT:
		default:
			il_print_char(chr);
			break;
		case ISYS_TO_STDERR:
			il_print_err_char(chr);
			break;
	}
	return TRUE;
}

BOOL
ISYSPrintCharWithProperty(	IN UINT32 chr,
							IN UINT32 p,
							IN UINT32 to)
{
	switch(to)
	{
		case ISYS_TO_STDOUT:
		default:
			il_print_char_p(chr, p);
			break;
		case ISYS_TO_STDERR:
			il_print_err_char_p(chr, p);
			break;
	}
	return TRUE;
}

BOOL
ISYSPrintString(IN BYTEPTR str,
				IN UINT32 to)
{
	switch(to)
	{
		case ISYS_TO_STDOUT:
		default:
			il_print_str(str);
			break;
		case ISYS_TO_STDERR:
			il_print_err_str(str);
			break;
	}
	return TRUE;
}

BOOL
ISYSPrintStringWithProperty(IN BYTEPTR str,
							IN UINT32 p,
							IN UINT32 to)
{
	switch(to)
	{
		case ISYS_TO_STDOUT:
		default:
			il_print_str_p(str, p);
			break;
		case ISYS_TO_STDERR:
			il_print_err_str_p(str, p);
			break;
	}
	return TRUE;
}

BOOL
ISYSClearConsole(VOID)
{
	il_clear_screen();
	return TRUE;
}

BOOL
ISYSSetCaretPosition(	IN UINT32 x,
						IN UINT32 y)
{
	il_set_cursor(x, y);
	return TRUE;
}

BOOL
ISYSGetCaretPosition(	OUT UINT32PTR x,
						OUT UINT32PTR y)
{
	il_get_cursor(x, y);
	return TRUE;
}

BOOL
ISYSLockCaret(VOID)
{
	il_lock_cursor();
	return TRUE;
}

BOOL
ISYSUnlockCaret(VOID)
{
	il_unlock_cursor();
	return TRUE;
}

BOOL
ISYSWriteConsoleBuffer(	IN BYTEPTR buffer,
						IN UINT32 size)
{
	il_write_console_buffer(buffer, size);
	return TRUE;
}

BOOL
ISYSGetConsoleSize(	OUT UINT32PTR row,
					OUT UINT32PTR column)
{
	il_get_text_screen_size(row, column);
	return TRUE;
}

VOID
ISYSHideConsoleClock(VOID)
{
	il_set_clock(1);
}

VOID
ISYSShowConsoleClock(VOID)
{
	il_set_clock(0);
}
