/**
	@File:			message.h
	@Author:		Ihsoh
	@Date:			2015-08-15
	@Description:
		非模态消息窗体。
*/

#ifndef	_WINDOWS_MESSAGE_H_
#define	_WINDOWS_MESSAGE_H_

#include "../types.h"
#include "../window.h"

#define	MESSAGE_WINDOW_STYLE_NONE		0x00000000
#define	MESSAGE_WINDOW_STYLE_CENTER		0x00000001
#define	MESSAGE_WINDOW_STYLE_TOP		0x00000002

extern
WindowPtr
message_window_show(IN CASCTEXT title,
					IN CASCTEXT text,
					IN uint32 style,
					IN int32 x,
					IN int32 y,
					IN uint32 color,
					IN uint32 bgcolor);

#endif
