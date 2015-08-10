#include "mouse.h"
#include "types.h"

#include "ilib/ilib.h"

BOOL
ISYSGetMousePosition(	OUT INT32PTR x,
						OUT INT32PTR y)
{
	get_mouse_position(x, y);
	return TRUE;
}

UINT32
ISYSGetMouseButtonStatus(VOID)
{
	UINT32 status = ISYS_MBUTTON_STATUS_NONE;
	if(il_is_mouse_left_button_down())
		status |= ISYS_MBUTTON_STATUS_LEFT;
	if(il_is_mouse_right_button_down())
		status |= ISYS_MBUTTON_STATUS_RIGHT;
	return status;
}
