/**
	@File:			message.c
	@Author:		Ihsoh
	@Date:			2015-08-15
	@Description:
		非模态消息窗口。
*/

#include "message.h"
#include "../window.h"
#include "../image.h"
#include "../enfont.h"

#include <ilib/string.h>

#define	_TPADDING	20
#define	_BPADDING	20
#define	_LPADDING	20
#define	_RPADDING	20
#define	_MAX_CHAR	MESSAGE_BOX_MAX_CHAR
#define	_MAX_LINE	MESSAGE_BOX_MAX_LINE
#define	_WIDTH		(_MAX_CHAR * ENFONT_WIDTH + _LPADDING + _RPADDING) 
#define	_HEIGHT		(_MAX_LINE * ENFONT_HEIGHT + _TPADDING + _BPADDING)

/**
	@Function:		_message_window_event
	@Access:		Public
	@Description:
		空的事件处理函数。
	@Parameters:
		window, WindowPtr, IN
			引发事件的窗体。
		params, struct WindowEventParams *, IN
			事件附带的参数。
	@Return:
*/
static
void
_message_window_event(	IN WindowPtr window,
						IN struct WindowEventParams * params)
{
	if(params->event_type == WINDOW_EVENT_PAINT)
	{
		
	}
}

/**
	@Function:		message_window_show
	@Access:		Public
	@Description:
		显示一个新的消息窗口。最多支持157个字符。
	@Parameters:
		title, CASCTEXT, IN
			消息窗口的标题。
		text, CASCTEXT, IN
			消息文本。最多支持157个字符。
			超出后面会附带“...”。
		style, uint32, IN
			消息窗口的样式。
			MESSAGE_WINDOW_STYLE_CENTER：居中显示。
			MESSAGE_WINDOW_STYLE_TOP：置顶显示。
		x, int32, IN
			窗口X坐标。如果样式指定了MESSAGE_WINDOW_STYLE_CENTER，该值无效。
		y, int32, IN
			窗口Y坐标。如果样式指定了MESSAGE_WINDOW_STYLE_CENTER，该值无效。
		color, uint32, IN
			文字颜色。
		bgcolor, uint32, IN
			窗口背景颜色。
	@Return:
		WindowPtr
*/
WindowPtr
message_window_show(IN CASCTEXT title,
					IN CASCTEXT text,
					IN uint32 style,
					IN int32 x,
					IN int32 y,
					IN uint32 color,
					IN uint32 bgcolor)
{
	if(title == NULL || text == NULL)
		return NULL;
	struct Window * window = NULL;
	window = create_window(	_WIDTH,
							_HEIGHT,
							bgcolor,
							WINDOW_STYLE_MINIMIZE | WINDOW_STYLE_CLOSE,
							title,
							_message_window_event);
	if(window == NULL)
		return NULL;
	struct CommonImage * workspace = &(window->workspace);
	if(!rect_common_image(workspace, 0, 0, _WIDTH, _HEIGHT, bgcolor))
	{
		destroy_window(window);
		return FALSE;
	}

	uint32 len = strlen(text);
	if(len <= _MAX_CHAR)
		text_common_image(	workspace,
							_LPADDING, 
							_TPADDING, 
							get_enfont_ptr(),
							text,
							len,
							color);
	else if(len <= _MAX_CHAR * 2)
	{
		ASCCHAR line0[_MAX_CHAR];
		ASCCHAR line1[_MAX_CHAR];
		memcpy_safe(line0, sizeof(line0), text, _MAX_CHAR);
		memcpy_safe(line1, sizeof(line1), text + _MAX_CHAR, len - _MAX_CHAR);
		uint8 * enfont = get_enfont_ptr();
		text_common_image(	workspace,
							_LPADDING, 
							_TPADDING, 
							enfont,
							line0,
							_MAX_CHAR,
							color);
		text_common_image(	workspace,
							_LPADDING, 
							_TPADDING + ENFONT_HEIGHT, 
							enfont,
							line1,
							len - _MAX_CHAR,
							color);
	}
	else if(len <= _MAX_CHAR * 3)
	{
		ASCCHAR line0[_MAX_CHAR];
		ASCCHAR line1[_MAX_CHAR];
		ASCCHAR line2[_MAX_CHAR];
		memcpy_safe(line0, sizeof(line0), text, _MAX_CHAR);
		memcpy_safe(line1, sizeof(line1), text + _MAX_CHAR, _MAX_CHAR);
		memcpy_safe(line2, sizeof(line2), text + _MAX_CHAR * 2, len - _MAX_CHAR * 2);
		uint8 * enfont = get_enfont_ptr();
		text_common_image(	workspace,
							_LPADDING, 
							_TPADDING, 
							enfont,
							line0,
							_MAX_CHAR,
							color);
		text_common_image(	workspace,
							_LPADDING, 
							_TPADDING + ENFONT_HEIGHT, 
							enfont,
							line1,
							_MAX_CHAR,
							color);
		text_common_image(	workspace,
							_LPADDING, 
							_TPADDING + ENFONT_HEIGHT * 2, 
							enfont,
							line2,
							len - _MAX_CHAR * 2,
							color);
	}
	else if(len <= _MAX_CHAR * 4)
	{
		ASCCHAR line0[_MAX_CHAR];
		ASCCHAR line1[_MAX_CHAR];
		ASCCHAR line2[_MAX_CHAR];
		ASCCHAR line3[_MAX_CHAR];
		memcpy_safe(line0, sizeof(line0), text, _MAX_CHAR);
		memcpy_safe(line1, sizeof(line1), text + _MAX_CHAR, _MAX_CHAR);
		memcpy_safe(line2, sizeof(line2), text + _MAX_CHAR * 2, _MAX_CHAR);
		memcpy_safe(line3, sizeof(line3), text + _MAX_CHAR * 3, len - _MAX_CHAR * 3);
		uint8 * enfont = get_enfont_ptr();
		text_common_image(	workspace,
							_LPADDING, 
							_TPADDING, 
							enfont,
							line0,
							_MAX_CHAR,
							color);
		text_common_image(	workspace,
							_LPADDING, 
							_TPADDING + ENFONT_HEIGHT, 
							enfont,
							line1,
							_MAX_CHAR,
							color);
		text_common_image(	workspace,
							_LPADDING, 
							_TPADDING + ENFONT_HEIGHT * 2, 
							enfont,
							line2,
							_MAX_CHAR,
							color);
		text_common_image(	workspace,
							_LPADDING, 
							_TPADDING + ENFONT_HEIGHT * 3, 
							enfont,
							line3,
							len - _MAX_CHAR * 3,
							color);
	}
	else
	{
		ASCCHAR line0[_MAX_CHAR];
		ASCCHAR line1[_MAX_CHAR];
		ASCCHAR line2[_MAX_CHAR];
		ASCCHAR line3[_MAX_CHAR];
		memcpy_safe(line0, sizeof(line0), text, _MAX_CHAR);
		memcpy_safe(line1, sizeof(line1), text + _MAX_CHAR, _MAX_CHAR);
		memcpy_safe(line2, sizeof(line2), text + _MAX_CHAR * 2, len - _MAX_CHAR * 2);
		uint8 * enfont = get_enfont_ptr();
		text_common_image(	workspace,
							_LPADDING, 
							_TPADDING, 
							enfont,
							line0,
							_MAX_CHAR,
							color);
		text_common_image(	workspace,
							_LPADDING, 
							_TPADDING + ENFONT_HEIGHT, 
							enfont,
							line1,
							_MAX_CHAR,
							color);
		text_common_image(	workspace,
							_LPADDING, 
							_TPADDING + ENFONT_HEIGHT * 2, 
							enfont,
							line2,
							_MAX_CHAR,
							color);
		uint32 ui;
		for(ui = 0; ui < _MAX_CHAR - 3 && ui < len - _MAX_CHAR * 3; ui++)
			line3[ui] = text[_MAX_CHAR * 3 + ui];
		line3[ui++] = '.';
		line3[ui++] = '.';
		line3[ui++] = '.';
		text_common_image(	workspace,
							_LPADDING, 
							_TPADDING + ENFONT_HEIGHT * 3, 
							enfont,
							line3,
							ui,
							color);
	}

	window->state = WINDOW_STATE_SHOW;
	if(style & MESSAGE_WINDOW_STYLE_CENTER)
	{
		x = vesa_get_width() / 2 - _WIDTH / 2;
		y = vesa_get_height() / 2 - _HEIGHT / 2;
	}
	window->x = x;
	window->y = y;
	if(style & MESSAGE_WINDOW_STYLE_TOP)
		set_top_window(window);
	return window;
}
