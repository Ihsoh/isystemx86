/**
	@File:			window.c
	@Author:		Ihsoh
	@Date:			2014-11-20
	@Description:
		提供窗体操作相关的功能
*/

#include "window.h"
#include "memory.h"
#include "image.h"
#include "enfont.h"
#include "mouse.h"
#include "config.h"

#include "bmp/bmp.h"

#include "utils/img.h"

#include "fs/ifs1/fs.h"

#include "window/control.h"

static struct CommonImage close_button;
static struct CommonImage close_button_hover;
static struct CommonImage close_button_blur;
static struct CommonImage hidden_button;
static struct CommonImage hidden_button_hover;
static struct CommonImage hidden_button_blur;

uint32 __window_title_bar_bgcolor		= 0xff000000;
uint32 __window_title_bar_bgcolor_nt	= 0xffffffff;
uint32 __window_border_color			= 0xff000000;
uint32 __window_border_color_nt			= 0xffffffff;

/**
	@Function:		_WinmgrLoadImage
	@Access:		Private
	@Description:
		加载图片。
	@Parameters:
		common_image, struct CommonImage *, OUT
			图片。
		path, int8 *, IN
			图片路径。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。		
*/
static
BOOL
_WinmgrLoadImage(	OUT struct CommonImage * common_image,
					IN int8 * path)
{
	FileObject * fptr = Ifs1OpenFile(path, FILE_MODE_READ);
	if(fptr == NULL)
		return FALSE;
	uchar * image_data = MemAlloc(flen(fptr));
	if(image_data == NULL)
		return FALSE;
	Ifs1ReadFile(fptr, image_data, flen(fptr));
	Ifs1CloseFile(fptr);
	if(!new_common_image(common_image, image_data))
	{
		MemFree(image_data);
		return FALSE;
	}
	MemFree(image_data);
	return TRUE;
}

/**
	@Function:		WinmgrInit
	@Access:		Public
	@Description:
		初始化窗体所需的资源。
	@Parameters:
	@Return:
		BOOL
			返回TRUE则成功，否则失败。		
*/
BOOL
WinmgrInit(void)
{
	// 加载窗体的配置。
	uint32 uiv = 0;
	if(config_gui_get_uint("TitleBarBackgroundColor", &uiv))
		__window_title_bar_bgcolor = uiv;
	uiv = 0;
	if(config_gui_get_uint("TitleBarBackgroundColorNotTop", &uiv))
		__window_title_bar_bgcolor_nt = uiv;
	uiv = 0;
	if(config_gui_get_uint("WindowBorderColor", &uiv))
		__window_border_color = uiv;
	uiv = 0;
	if(config_gui_get_uint("WindowBorderColorNotTop", &uiv))
		__window_border_color_nt = uiv;

	new_empty_image0(&close_button, CLOSE_BUTTON_WIDTH, CLOSE_BUTTON_HEIGHT);
	new_empty_image0(&close_button_hover, CLOSE_BUTTON_WIDTH, CLOSE_BUTTON_HEIGHT);
	new_empty_image0(&close_button_blur, CLOSE_BUTTON_WIDTH, CLOSE_BUTTON_HEIGHT);
	new_empty_image0(&hidden_button, HIDDEN_BUTTON_WIDTH, HIDDEN_BUTTON_HEIGHT);
	new_empty_image0(&hidden_button_hover, HIDDEN_BUTTON_WIDTH, HIDDEN_BUTTON_HEIGHT);
	new_empty_image0(&hidden_button_blur, HIDDEN_BUTTON_WIDTH, HIDDEN_BUTTON_HEIGHT);

	ASCCHAR imgfile[1024];
	IMGLBMPPtr bmpobj = NULL;
	config_gui_get_string("CloseButton", imgfile, sizeof(imgfile));
	bmpobj = BmpCreate(imgfile);
	if(bmpobj != NULL)
	{
		UtlConvertBmpToCommonImage(bmpobj, CLOSE_BUTTON_WIDTH, CLOSE_BUTTON_HEIGHT, &close_button);
		BmpDestroy(bmpobj);
	}

	config_gui_get_string("CloseButtonHover", imgfile, sizeof(imgfile));
	bmpobj = BmpCreate(imgfile);
	if(bmpobj != NULL)
	{
		UtlConvertBmpToCommonImage(bmpobj, CLOSE_BUTTON_WIDTH, CLOSE_BUTTON_HEIGHT, &close_button_hover);
		BmpDestroy(bmpobj);
	}

	config_gui_get_string("CloseButtonBlur", imgfile, sizeof(imgfile));
	bmpobj = BmpCreate(imgfile);
	if(bmpobj != NULL)
	{
		UtlConvertBmpToCommonImage(bmpobj, CLOSE_BUTTON_WIDTH, CLOSE_BUTTON_HEIGHT, &close_button_blur);
		BmpDestroy(bmpobj);
	}

	config_gui_get_string("HiddenButton", imgfile, sizeof(imgfile));
	bmpobj = BmpCreate(imgfile);
	if(bmpobj != NULL)
	{
		UtlConvertBmpToCommonImage(bmpobj, HIDDEN_BUTTON_WIDTH, HIDDEN_BUTTON_HEIGHT, &hidden_button);
		BmpDestroy(bmpobj);
	}

	config_gui_get_string("HiddenButtonHover", imgfile, sizeof(imgfile));
	bmpobj = BmpCreate(imgfile);
	if(bmpobj != NULL)
	{
		UtlConvertBmpToCommonImage(bmpobj, HIDDEN_BUTTON_WIDTH, HIDDEN_BUTTON_HEIGHT, &hidden_button_hover);
		BmpDestroy(bmpobj);
	}

	config_gui_get_string("HiddenButtonBlur", imgfile, sizeof(imgfile));
	bmpobj = BmpCreate(imgfile);
	if(bmpobj != NULL)
	{
		UtlConvertBmpToCommonImage(bmpobj, HIDDEN_BUTTON_WIDTH, HIDDEN_BUTTON_HEIGHT, &hidden_button_blur);
		BmpDestroy(bmpobj);
	}
}

/**
	@Function:		WinmgrUninit
	@Access:		Public
	@Description:
		销毁窗体所需的资源。
	@Parameters:
	@Return:
*/
void
WinmgrUninit(void)
{
	destroy_common_image(&close_button);
	destroy_common_image(&close_button_hover);
	destroy_common_image(&hidden_button);
	destroy_common_image(&hidden_button_hover);
}

/**
	@Function:		WinmgrRenderWindow
	@Access:		Public
	@Description:
		渲染窗体。
	@Parameters:
		window, struct Window *, IN
			指向窗体结构体的指针。
		top, BOOL, IN
			TRUE 则为置顶窗体，否则不是。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。		
*/
BOOL
WinmgrRenderWindow(	IN struct Window * window,
					IN BOOL top)
{
	if(window == NULL)
		return FALSE;

	// 从隐藏的状态过渡到显示的状态时引发WINDOW_EVENT_SHOW事件。
	if(	window->old_state == WINDOW_STATE_HIDDEN
		&& window->state == WINDOW_STATE_SHOW)
		WinmgrDispatchEvent(window, WINDOW_EVENT_SHOW, NULL);
	// 从显示的状态过渡到隐藏的状态时引发WINDOW_EVENT_HIDDEN事件。
	else if(window->old_state == WINDOW_STATE_SHOW
			&& window->state == WINDOW_STATE_HIDDEN)
		WinmgrDispatchEvent(window, WINDOW_EVENT_HIDDEN, NULL);
	// 更新旧状态。
	window->old_state = window->state;
	if(	window->state == WINDOW_STATE_CLOSED
		|| window->state == WINDOW_STATE_HIDDEN)
		return FALSE;

	// 窗体对象中is_top为FALSE，但渲染窗体时窗体在最顶层，
	// 说明窗体从底层过渡到顶层，发送WINDOW_EVENT_FOCUS事件。
	if(!window->is_top && top)
	{
		WinmgrDispatchEvent(	window,
								WINDOW_EVENT_FOCUS,
								NULL);
		window->is_top = TRUE;
	}
	// 窗体对象中is_top为TRUE，但渲染窗体时窗体不在最顶层，
	// 说明窗体从顶层过渡到底层，发送WINDOW_EVENT_UNFOCUS事件。
	else if(window->is_top && !top)
	{
		WinmgrDispatchEvent(	window,
								WINDOW_EVENT_UNFOCUS,
								NULL);
		window->is_top = FALSE;
	}

	uint32 wstyle = window->style;
	BOOL has_title_bar = !(wstyle & WINDOW_STYLE_NO_TITLE);

	if(has_title_bar)
	{
		ImagePtr title_bar = &window->title_bar;

		// 填充标题栏。
		uint32 title_bar_bgcolor = top ? TITLE_BAR_BGCOLOR : TITLE_BAR_BGCOLOR_NT;
		rect_common_image(title_bar, 0, 0, window->width, TITLE_BAR_HEIGHT, title_bar_bgcolor);

		//关闭按钮
		if(wstyle & WINDOW_STYLE_CLOSE)
			if(top)
				if(window->over_close_button)
					draw_common_image(	title_bar, 
										&close_button_hover, 
										window->width - CLOSE_BUTTON_WIDTH, 
										0, 
										CLOSE_BUTTON_WIDTH, 
										CLOSE_BUTTON_HEIGHT);
				else		
					draw_common_image(	title_bar, 
										&close_button,
										window->width - CLOSE_BUTTON_WIDTH, 
										0, 
										CLOSE_BUTTON_WIDTH, 
										CLOSE_BUTTON_HEIGHT);
			else
				draw_common_image(	title_bar, 
									&close_button_blur, 
									window->width - CLOSE_BUTTON_WIDTH, 
									0, 
									CLOSE_BUTTON_WIDTH, 
									CLOSE_BUTTON_HEIGHT);
		else
			if(top)
				draw_common_image(	title_bar, 
									&close_button, 
									window->width - CLOSE_BUTTON_WIDTH,
									0, 
									CLOSE_BUTTON_WIDTH,
									CLOSE_BUTTON_HEIGHT);
			else
				draw_common_image(	title_bar, 
									&close_button_blur, 
									window->width - CLOSE_BUTTON_WIDTH,
									0, 
									CLOSE_BUTTON_WIDTH,
									CLOSE_BUTTON_HEIGHT);

		//隐藏按钮
		if(wstyle & WINDOW_STYLE_MINIMIZE)
			if(top)
				if(window->over_hidden_button)
					draw_common_image(	title_bar,
										&hidden_button_hover,
										window->width - CLOSE_BUTTON_WIDTH - HIDDEN_BUTTON_WIDTH,
										0,
										HIDDEN_BUTTON_WIDTH,
										HIDDEN_BUTTON_HEIGHT);
				else	
					draw_common_image(	title_bar,
										&hidden_button,
										window->width - CLOSE_BUTTON_WIDTH - HIDDEN_BUTTON_WIDTH,
										0,
										HIDDEN_BUTTON_WIDTH,
										HIDDEN_BUTTON_HEIGHT);
			else
				draw_common_image(	title_bar,
									&hidden_button_blur,
									window->width - CLOSE_BUTTON_WIDTH - HIDDEN_BUTTON_WIDTH,
									0,
									HIDDEN_BUTTON_WIDTH,
									HIDDEN_BUTTON_HEIGHT);
		else
			if(top)
				draw_common_image(	title_bar,
									&hidden_button,
									window->width - CLOSE_BUTTON_WIDTH - HIDDEN_BUTTON_WIDTH,
									0,
									HIDDEN_BUTTON_WIDTH,
									HIDDEN_BUTTON_HEIGHT);
			else
				draw_common_image(	title_bar,
									&hidden_button_blur,
									window->width - CLOSE_BUTTON_WIDTH - HIDDEN_BUTTON_WIDTH,
									0,
									HIDDEN_BUTTON_WIDTH,
									HIDDEN_BUTTON_HEIGHT);

		//画标题
		text_common_image(	title_bar,
							10,
							TITLE_BAR_HEIGHT / 2 - ENFONT_HEIGHT / 2,
							EnfntGetFontDataPtr(),
							window->title,
							(window->width - 10 - CLOSE_BUTTON_WIDTH - HIDDEN_BUTTON_WIDTH) / ENFONT_WIDTH,
							0xff000000);
	}

	// 如果窗体被锁定，则不发送WINDOW_EVENT_PAINT事件。
	if(!window->locked)
		WinmgrDispatchEvent(window, WINDOW_EVENT_PAINT, NULL);

	return TRUE;	
}

/**
	@Function:		WinmgrGetKey
	@Access:		Public
	@Description:
		获取指定窗体的按键缓冲区的按键。
	@Parameters:
		window, struct Window *, IN
			指向窗体结构体的指针。
	@Return:
		uint8
			键值。		
*/
uint8
WinmgrGetKey(IN struct Window * window)
{
	if(window == NULL || !WINMGR_HAS_KEY(window))
		return 0;
	uint8 key = window->key_buffer[0];
	uint32 ui;
	for(ui = 0; ui < window->key_count - 1; ui++)
		window->key_buffer[ui] = window->key_buffer[ui + 1];
	window->key_count--;
	return key;
}

/**
	@Function:		WinmgrPeekKey
	@Access:		Public
	@Description:
		获取指定窗体的按键缓冲区的按键。
		但不从按键缓冲区中取出按键。
	@Parameters:
		window, struct Window *, IN
			指向窗体结构体的指针。
	@Return:
		uint8
			键值。		
*/
uint8
WinmgrPeekKey(IN struct Window * window)
{
	if(window == NULL || !WINMGR_HAS_KEY(window))
		return 0;
	uint8 key = window->key_buffer[0];
	return key;
}

/**
	@Function:		WinmgrClearKeyBuffer
	@Access:		Public
	@Description:
		清除指定窗体的按键缓冲区内的按键。
	@Parameters:
		window, WindowPtr, IN
			指向窗体结构体的指针。
	@Return:		
*/
void
WinmgrClearKeyBuffer(IN WindowPtr window)
{
	if(window == NULL)
		return;
	window->key_count = 0;
}

/**
	@Function:		WinmgrDispatchEvent
	@Access:		Public
	@Description:
		分发窗体事件。
	@Parameters:
		window, WindowPtr, IN
			指向窗体结构体的指针。
		type, int32, IN
			消息类型。
		data, void *, IN
			事件附加数据。
	@Return:
		uint8
			键值。		
*/
void
WinmgrDispatchEvent(IN WindowPtr window,
					IN int32 type,
					IN void * data)
{
	if(	window == NULL
		|| window->event == NULL)
		return;
	uint32 wstyle = window->style;
	BOOL has_title_bar = !(wstyle & WINDOW_STYLE_NO_TITLE);
	int32 x, y;
	KnlGetMousePosition(&x, &y);
	x = x - window->x;
	if(has_title_bar)
		y = y - window->y - TITLE_BAR_HEIGHT;
	else
		y = y - window->y;
	uint32 mouse_button = MOUSE_BUTTON_NONE;
	if(KnlIsMouseLeftButtonDown())
		mouse_button |= MOUSE_BUTTON_LEFT;
	if(KnlIsMouseRightButtonDown())
		mouse_button |= MOUSE_BUTTON_RIGHT;
	struct WindowEventParams params;
	params.wid = window->id;
	params.event_type = type;
	params.mouse_x = x;
	params.mouse_y = y;
	params.mouse_button = mouse_button;
	params.screen = &window->workspace;
	params.data = data;
	params.window = window;
	window->event(window, &params);
}

void
WinmgrFocusControl(	IN WindowPtr window,
					IN uint32 cid)
{
	if(window == NULL)
		return;
	window->focused_ctrl = cid;
}

void
WinmgrUnfocusControl(IN WindowPtr window)
{
	if(window == NULL)
		return;
	window->focused_ctrl = CONTROL_INVALID_ID;
}
