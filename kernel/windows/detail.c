/**
	@File:			detail.c
	@Author:		Ihsoh
	@Date:			2015-08-14
	@Description:
		显示文件或文件夹的详细信息。
*/

#include "detail.h"
#include "../fs/ifs1/fs.h"

#include "message.h"

#include <ilib/string.h>

static struct Window * _window;
static ASCCHAR path[1024];

#define	_WIDTH		400
#define	_HEIGHT		200

static
void
_detail_window_event(IN struct Window * window,
					IN struct WindowEventParams * params)
{
	if(params->event_type == WINDOW_EVENT_PAINT)
	{
	}
}

BOOL
detail_window_init(void)
{
	_window = create_window(_WIDTH,
							_HEIGHT,
							0xffffffff,
							WINDOW_STYLE_MINIMIZE | WINDOW_STYLE_NO_WMGR,
							"Detail",
							_detail_window_event);
	if(_window == NULL)
		return FALSE;
	return TRUE;
}

BOOL
detail_window_show(IN CASCTEXT path)
{
	if(_window == NULL || strlen(path) >= 1024)
		return FALSE;
	if(!Ifs1Exists(path))
	{
		message_window_show(	"The path does not exists!",
								"The path does not exists!",
								MESSAGE_WINDOW_STYLE_CENTER
								| MESSAGE_WINDOW_STYLE_TOP,
								0,
								0,
								0xff000000,
								0xffffffff);
		return FALSE;
	}
	struct CommonImage * workspace = &_window->workspace;
	rect_common_image(workspace, 0, 0, _WIDTH, _HEIGHT, 0xffffffff);
	uint8 * enfont = get_enfont_ptr();
	uint32 len = strlen(path);
	if(path[len - 1] == '/')
	{
		text_common_image(	workspace,
							20, 
							20, 
							enfont,
							"Directory",
							strlen("Directory"),
							0xff0000ff);
		UtlCopyString(_window->title, sizeof(_window->title), "Directory Detail");
		if(len == 4)
			text_common_image(	workspace,
								20, 
								40, 
								enfont,
								path,
								len,
								0xff000000);
		else
		{
			ASCCHAR name[1024];
			int32 pos;
			for(pos = len - 2; pos >= 0; pos--)
				if(path[pos] == '/')
					break;
			UtlCopyString(name, sizeof(name), path + pos + 1);
			text_common_image(	workspace,
								20, 
								40, 
								enfont,
								name,
								strlen(name),
								0xff000000);
		}
		ASCCHAR buffer[1024];
		ASCCHAR buffer1[1024];
		UtlCopyString(buffer, sizeof(buffer), "Item Count: ");
		uint32 count = Ifs1GetItemCount(path);
		UtlConcatString(buffer, sizeof(buffer), uitos(buffer1, count));
		text_common_image(	workspace,
							20, 
							60, 
							enfont,
							buffer,
							strlen(buffer),
							0xff000000);
	}
	else
	{
		text_common_image(	workspace,
							20, 
							20, 
							enfont,
							"File",
							strlen("File"),
							0xff00ff00);
		UtlCopyString(_window->title, sizeof(_window->title), "File Detail");
		ASCCHAR name[1024];
		int32 pos;
		for(pos = len - 1; pos >= 0; pos--)
			if(path[pos] == '/')
				break;
		UtlCopyString(name, sizeof(name), path + pos + 1);
		text_common_image(	workspace,
							20, 
							40, 
							enfont,
							name,
							strlen(name),
							0xff000000);
		FileObjectPtr fptr = Ifs1OpenFile(path, FILE_MODE_READ);
		uint32 len_b = flen(fptr);
		uint32 len_kb = len_b / 1024;
		uint32 len_mb = len_kb / 1024;
		Ifs1CloseFile(fptr);
		ASCCHAR buffer[1024];
		ASCCHAR buffer1[1024];
		UtlCopyString(buffer, sizeof(buffer), "Length: ");
		UtlConcatString(buffer, sizeof(buffer), uitos(buffer1, len_b));
		UtlConcatString(buffer, sizeof(buffer), "B, ");
		UtlConcatString(buffer, sizeof(buffer), uitos(buffer1, len_kb));
		UtlConcatString(buffer, sizeof(buffer), "KB, ");
		UtlConcatString(buffer, sizeof(buffer), uitos(buffer1, len_mb));
		UtlConcatString(buffer, sizeof(buffer), "MB");
		text_common_image(	workspace,
							20, 
							60, 
							enfont,
							buffer,
							strlen(buffer),
							0xff000000);

	}
	_window->state = WINDOW_STATE_SHOW;
	return TRUE;
}
