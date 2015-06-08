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
#include "ifs1fs.h"

#define	CLOSE_BUTTON_NS_IMAGE_PATH	"DA:/isystem/resources/images/close_button_ns.img0"
#define	CLOSE_BUTTON_S_IMAGE_PATH	"DA:/isystem/resources/images/close_button_s.img0"
#define	HIDDEN_BUTTON_NS_IMAGE_PATH	"DA:/isystem/resources/images/hidden_button_ns.img0"
#define	HIDDEN_BUTTON_S_IMAGE_PATH	"DA:/isystem/resources/images/hidden_button_s.img0"

static struct CommonImage close_button_ns;
static struct CommonImage close_button_s;
static struct CommonImage hidden_button_ns;
static struct CommonImage hidden_button_s;

/**
	@Function:		load_image
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
load_image(	OUT struct CommonImage * common_image,
			IN int8 * path)
{
	FILE * fptr = fopen(path, FILE_MODE_READ);
	if(fptr == NULL)
		return FALSE;
	uchar * image_data = alloc_memory(flen(fptr));
	if(image_data == NULL)
		return FALSE;
	fread(fptr, image_data, flen(fptr));
	fclose(fptr);
	if(!new_common_image(common_image, image_data))
	{
		free_memory(image_data);
		return FALSE;
	}
	free_memory(image_data);
	return TRUE;
}

/**
	@Function:		init_window_resources
	@Access:		Public
	@Description:
		初始化窗体所需的资源。
	@Parameters:
	@Return:
		BOOL
			返回TRUE则成功，否则失败。		
*/
BOOL
init_window_resources(void)
{
	return 	load_image(&close_button_ns, CLOSE_BUTTON_NS_IMAGE_PATH)
			&& load_image(&close_button_s, CLOSE_BUTTON_S_IMAGE_PATH)
			&& load_image(&hidden_button_ns, HIDDEN_BUTTON_NS_IMAGE_PATH)
			&& load_image(&hidden_button_s, HIDDEN_BUTTON_S_IMAGE_PATH);
}

/**
	@Function:		destroy_window_resources
	@Access:		Public
	@Description:
		销毁窗体所需的资源。
	@Parameters:
	@Return:
*/
void
destroy_window_resources(void)
{
	destroy_common_image(&close_button_ns);
	destroy_common_image(&close_button_s);
	destroy_common_image(&hidden_button_ns);
	destroy_common_image(&hidden_button_s);
}

/**
	@Function:		render_window
	@Access:		Public
	@Description:
		渲染窗体。
	@Parameters:
		window, struct Window *, IN
			指向窗体结构体的指针。
		image, struct CommonImage *, OUT
			指向图片缓冲区。
		top, BOOL, IN
			TRUE 则为置顶窗体，否则不是。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。		
*/
BOOL
render_window(	IN struct Window * window,
				OUT struct CommonImage * image,
				IN BOOL top)
{
	if(window == NULL || image == NULL)
		return FALSE;
	uint32 title_bar_bgcolor = top ? TITLE_BAR_BGCOLOR : TITLE_BAR_BGCOLOR_NT;
	rect_common_image(image, 0, 0, window->width, TITLE_BAR_HEIGHT, title_bar_bgcolor);

	//关闭按钮
	if(window->has_close_button)
		if(window->over_close_button)
			draw_common_image(	image, 
								&close_button_s, 
								window->width - CLOSE_BUTTON_WIDTH, 
								0, 
								CLOSE_BUTTON_WIDTH, 
								CLOSE_BUTTON_HEIGHT);
		else		
			draw_common_image(	image, 
								&close_button_ns, 
								window->width - CLOSE_BUTTON_WIDTH, 
								0, 
								CLOSE_BUTTON_WIDTH, 
								CLOSE_BUTTON_HEIGHT);
	else
		draw_common_image(	image, 
							&close_button_ns, 
							window->width - CLOSE_BUTTON_WIDTH,
							0, 
							CLOSE_BUTTON_WIDTH,
							CLOSE_BUTTON_HEIGHT);

	//隐藏按钮
	if(window->has_hidden_button)
		if(window->over_hidden_button)
			draw_common_image(	image,
								&hidden_button_s,
								window->width - CLOSE_BUTTON_WIDTH - HIDDEN_BUTTON_WIDTH,
								0,
								HIDDEN_BUTTON_WIDTH,
								HIDDEN_BUTTON_HEIGHT);
		else	
			draw_common_image(	image,
								&hidden_button_ns,
								window->width - CLOSE_BUTTON_WIDTH - HIDDEN_BUTTON_WIDTH,
								0,
								HIDDEN_BUTTON_WIDTH,
								HIDDEN_BUTTON_HEIGHT);
	else
		draw_common_image(	image,
							&hidden_button_ns,
							window->width - CLOSE_BUTTON_WIDTH - HIDDEN_BUTTON_WIDTH,
							0,
							HIDDEN_BUTTON_WIDTH,
							HIDDEN_BUTTON_HEIGHT);

	//画标题
	text_common_image(	image,
						10,
						TITLE_BAR_HEIGHT / 2 - ENFONT_HEIGHT / 2,
						get_enfont_ptr(),
						window->title,
						(window->width - 10 - CLOSE_BUTTON_WIDTH - HIDDEN_BUTTON_WIDTH) / ENFONT_WIDTH,
						0xff000000);
	
	/*rect_common_image(&window->workspace, 0, 0, window->width, window->height, window->bgcolor);*/
	
	if(window->event != NULL)
	{
		int32 x, y;
		get_mouse_position(&x, &y);
		x = x - window->x;
		y = y - window->y - TITLE_BAR_HEIGHT;
		struct WindowEventParams params;
		params.mouse_x = x;
		params.mouse_y = y;
		params.screen = &window->workspace;
		
		//Paint Event
		params.event_type = WINDOW_EVENT_PAINT;
		window->event(window, &params);
	}

	draw_common_image(image, &window->workspace, 0, TITLE_BAR_HEIGHT, window->width, window->height);
	return TRUE;	
}

/**
	@Function:		window_get_key
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
window_get_key(IN struct Window * window)
{
	if(window == NULL || !window_has_key(window))
		return 0;
	uint8 key = window->key_buffer[0];	
	uint32 ui;
	for(ui = 0; ui < window->key_count - 1; ui++)
		window->key_buffer[ui] = window->key_buffer[ui + 1];
	window->key_count--;
	return key;
}
