//Filename:		gui.c
//Author:		Ihsoh
//Date:			2015-10-05
//Descriptor:	GUI

#include "fs.h"
#include "types.h"
#include "sparams.h"
#include "gui.h"

BOOL
ILGIsValid(void)
{
	struct SParams sparams;
	system_call(SCALL_GUI, SCALL_GUI_IS_VALID, &sparams);
	return BOOL_SPARAM(sparams.param0);
}

int32
ILGCreateWindow(IN uint32		width,
				IN uint32		height,
				IN uint32		bgcolor,
				IN uint32		style,
				IN CASCTEXT		title)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(width);
	sparams.param1 = SPARAM(height);
	sparams.param2 = SPARAM(bgcolor);
	sparams.param3 = SPARAM(style);
	sparams.param4 = SPARAM(title);
	system_call(SCALL_GUI, SCALL_GUI_CREATE_WINDOW, &sparams);
	return INT32_SPARAM(sparams.param0);
}

BOOL
ILGCloseWindow(IN int32 wid)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(wid);
	system_call(SCALL_GUI, SCALL_GUI_CLOSE_WINDOW, &sparams);
	return BOOL_SPARAM(sparams.param0);
}

BOOL
ILGSetWindowState(	IN int32 wid,
					IN uint32 state)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(wid);
	sparams.param1 = SPARAM(state);
	system_call(SCALL_GUI, SCALL_GUI_SET_WINDOW_STATE, &sparams);
	return BOOL_SPARAM(sparams.param0);
}

BOOL
ILGGetWindowState(	IN int32 wid,
					IN uint32 * state)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(wid);
	sparams.param1 = SPARAM(state);
	system_call(SCALL_GUI, SCALL_GUI_GET_WINDOW_STATE, &sparams);
	return BOOL_SPARAM(sparams.param0);
}

BOOL
ILGSetWindowPosition(	IN int32 wid,
						IN int32 x,
						IN int32 y)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(wid);
	sparams.param1 = SPARAM(x);
	sparams.param2 = SPARAM(y);
	system_call(SCALL_GUI, SCALL_GUI_SET_WINDOW_POSITION, &sparams);
	return BOOL_SPARAM(sparams.param0);
}

BOOL
ILGGetWindowPosition(	IN int32 wid,
						IN int32 * x,
						IN int32 * y)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(wid);
	sparams.param1 = SPARAM(x);
	sparams.param2 = SPARAM(y);
	system_call(SCALL_GUI, SCALL_GUI_GET_WINDOW_POSITION, &sparams);
	return BOOL_SPARAM(sparams.param0);
}

BOOL
ILGSetWindowSize(	IN int32 wid,
					IN uint32 width,
					IN uint32 height)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(wid);
	sparams.param1 = SPARAM(width);
	sparams.param2 = SPARAM(height);
	system_call(SCALL_GUI, SCALL_GUI_SET_WINDOW_SIZE, &sparams);
	return BOOL_SPARAM(sparams.param0);
}

BOOL
ILGGetWindowSize(	IN int32 wid,
					IN uint32 * width,
					IN uint32 * height)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(wid);
	sparams.param1 = SPARAM(width);
	sparams.param2 = SPARAM(height);
	system_call(SCALL_GUI, SCALL_GUI_GET_WINDOW_SIZE, &sparams);
	return BOOL_SPARAM(sparams.param0);
}

BOOL
ILGSetWindowTitle(	IN int32 wid,
					IN CASCTEXT title)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(wid);
	sparams.param1 = SPARAM(title);
	system_call(SCALL_GUI, SCALL_GUI_SET_WINDOW_TITLE, &sparams);
	return BOOL_SPARAM(sparams.param0);
}

BOOL
ILGGetWindowTitle(	IN int32 wid,
					IN ASCTEXT title)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(wid);
	sparams.param1 = SPARAM(title);
	system_call(SCALL_GUI, SCALL_GUI_GET_WINDOW_TITLE, &sparams);
	return BOOL_SPARAM(sparams.param0);
}
