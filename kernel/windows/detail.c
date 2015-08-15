/**
	@File:			detail.c
	@Author:		Ihsoh
	@Date:			2015-08-14
	@Description:
		显示文件或文件夹的详细信息。
*/

#include "detail.h"
#include "../fs/ifs1/fs.h"

#include <ilib/string.h>

static struct Window * _window;
static ASCCHAR path[1024];

#define	WIDTH		200
#define	HEIGHT		320

void
detail_window_event(IN struct Window * window,
					IN struct WindowEventParams * params)
{

}

BOOL
detail_window_init(void)
{
	_window = create_window(WIDTH,
							HEIGHT,
							0xffffffff,
							WINDOW_STYLE_MINIMIZE | WINDOW_STYLE_CLOSE,
							"Detail",
							detail_window_event);
	if(_window == NULL)
		return FALSE;
	return TRUE;
}

BOOL
detail_window_show(IN CASCTEXT path)
{
	if(strlen(path) >= 1024)
		return FALSE;
	
	return TRUE;
}
