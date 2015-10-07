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

BOOL
ILGHasKey(	IN int32 wid,
			OUT uint32 * key)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(wid);
	sparams.param1 = SPARAM(key);
	system_call(SCALL_GUI, SCALL_GUI_HAS_KEY, &sparams);
	return BOOL_SPARAM(sparams.param0);
}

BOOL
ILGGetKey(	IN int32 wid,
			OUT uint32 * key)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(wid);
	sparams.param1 = SPARAM(key);
	system_call(SCALL_GUI, SCALL_GUI_GET_KEY, &sparams);
	return BOOL_SPARAM(sparams.param0);
}

BOOL
ILGGetMouse(IN int32 wid,
			OUT int32 * x,
			OUT int32 * y,
			OUT uint32 * button)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(wid);
	sparams.param1 = SPARAM(x);
	sparams.param2 = SPARAM(y);
	sparams.param3 = SPARAM(button);
	system_call(SCALL_GUI, SCALL_GUI_GET_MOUSE, &sparams);
	return BOOL_SPARAM(sparams.param0);
}

BOOL
ILGFocused(IN int32 wid)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(wid);
	system_call(SCALL_GUI, SCALL_GUI_FOCUSED, &sparams);
	return BOOL_SPARAM(sparams.param0);
}

BOOL
ILGFocus(IN int32 wid)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(wid);
	system_call(SCALL_GUI, SCALL_GUI_FOCUS, &sparams);
	return BOOL_SPARAM(sparams.param0);
}

BOOL
ILGSetPixel(IN int32 wid,
			IN int32 x,
			IN int32 y,
			IN uint32 pixel)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(wid);
	sparams.param1 = SPARAM(x);
	sparams.param2 = SPARAM(y);
	sparams.param3 = SPARAM(pixel);
	system_call(SCALL_GUI, SCALL_GUI_SET_PIXEL, &sparams);
	return BOOL_SPARAM(sparams.param0);
}

BOOL
ILGGetPixel(IN int32 wid,
			IN int32 x,
			IN int32 y,
			OUT uint32 * pixel)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(wid);
	sparams.param1 = SPARAM(x);
	sparams.param2 = SPARAM(y);
	sparams.param3 = SPARAM(pixel);
	system_call(SCALL_GUI, SCALL_GUI_GET_PIXEL, &sparams);
	return BOOL_SPARAM(sparams.param0);
}

BOOL
ILGDrawRect(IN int32 wid,
			IN int32 x,
			IN int32 y,
			IN int32 width,
			IN int32 height,
			IN uint32 pixel)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(wid);
	sparams.param1 = SPARAM(x);
	sparams.param2 = SPARAM(y);
	sparams.param3 = SPARAM(width);
	sparams.param4 = SPARAM(height);
	sparams.param5 = SPARAM(pixel);
	system_call(SCALL_GUI, SCALL_GUI_DRAW_RECT, &sparams);
	return BOOL_SPARAM(sparams.param0);
}

BOOL
ILGDrawImage(	IN int32 wid,
				IN int32 x,
				IN int32 y,
				IN int32 width,
				IN int32 height,
				IN ImagePtr image)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(wid);
	sparams.param1 = SPARAM(x);
	sparams.param2 = SPARAM(y);
	sparams.param3 = SPARAM(width);
	sparams.param4 = SPARAM(height);
	sparams.param5 = SPARAM(image);
	system_call(SCALL_GUI, SCALL_GUI_DRAW_IMAGE, &sparams);
	return BOOL_SPARAM(sparams.param0);
}

BOOL
ILGDrawText(IN int32 wid,
			IN int32 x,
			IN int32 y,
			IN CASCTEXT text,
			IN uint32 color)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(wid);
	sparams.param1 = SPARAM(x);
	sparams.param2 = SPARAM(y);
	sparams.param3 = SPARAM(text);
	sparams.param4 = SPARAM(color);
	system_call(SCALL_GUI, SCALL_GUI_DRAW_TEXT, &sparams);
	return BOOL_SPARAM(sparams.param0);
}

BOOL
ILGDrawLine(IN int32 wid,
			IN int32 startx,
			IN int32 starty,
			IN int32 endx,
			IN int32 endy,
			IN uint32 color)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(wid);
	sparams.param1 = SPARAM(startx);
	sparams.param2 = SPARAM(starty);
	sparams.param3 = SPARAM(endx);
	sparams.param4 = SPARAM(endy);
	sparams.param4 = SPARAM(color);
	system_call(SCALL_GUI, SCALL_GUI_DRAW_LINE, &sparams);
	return BOOL_SPARAM(sparams.param0);
}

BOOL
ILGGetMessage(	IN int32 wid,
				OUT int32 * cid,
				OUT uint32 * type,
				OUT void ** data)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(wid);
	sparams.param1 = SPARAM(cid);
	sparams.param2 = SPARAM(type);
	sparams.param3 = SPARAM(data);
	system_call(SCALL_GUI, SCALL_GUI_GET_MESSAGE, &sparams);
	return BOOL_SPARAM(sparams.param0);
}

BOOL
ILGSetText(	IN int32 wid,
			IN int32 cid,
			IN CASCTEXT text)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(wid);
	sparams.param1 = SPARAM(cid);
	sparams.param2 = SPARAM(text);
	system_call(SCALL_GUI, SCALL_GUI_SET_TEXT, &sparams);
	return BOOL_SPARAM(sparams.param0);
}

BOOL
ILGGetText(	IN int32 wid,
			IN int32 cid,
			OUT ASCTEXT text,
			IN uint32 bufsz)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(wid);
	sparams.param1 = SPARAM(cid);
	sparams.param2 = SPARAM(text);
	sparams.param3 = SPARAM(bufsz);
	system_call(SCALL_GUI, SCALL_GUI_GET_TEXT, &sparams);
	return BOOL_SPARAM(sparams.param0);
}

BOOL
ILGNewButton(	IN int32 wid,
				IN int32 x,
				IN int32 y,
				IN CASCTEXT text,
				OUT uint32 * cid)
{
	struct SParams sparams;
	sparams.param0 = SPARAM(wid);
	sparams.param1 = SPARAM(x);
	sparams.param2 = SPARAM(y);
	sparams.param3 = SPARAM(text);
	sparams.param4 = SPARAM(cid);
	system_call(SCALL_GUI, SCALL_GUI_NEW_BUTTON, &sparams);
	return BOOL_SPARAM(sparams.param0);
}
