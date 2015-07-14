/**
	@File:			window.h
	@Author:		Ihsoh
	@Date:			2014-11-20
	@Description:
*/

#ifndef	_WINDOW_H_
#define	_WINDOW_H_

#include "types.h"
#include "image.h"

#define	TITLE_BAR_HEIGHT		30
#define	TITLE_BAR_BGCOLOR		0xff8e70c9
#define	TITLE_BAR_BGCOLOR_NT	0xffcccccc

#define	WINDOW_STATE_SHOW		1
#define	WINDOW_STATE_HIDDEN		2
#define	WINDOW_STATE_CLOSED		3

#define	CLOSE_BUTTON_WIDTH		55
#define	CLOSE_BUTTON_HEIGHT		30

#define	HIDDEN_BUTTON_WIDTH		55
#define	HIDDEN_BUTTON_HEIGHT	30


#define	WINDOW_EVENT_PAINT		1
#define	WINDOW_EVENT_KEYDOWN	2

#define	WINDOW_KEY_BUFFER_SIZE	16

typedef struct WindowEventParams
{
	int32					event_type;
	int32					mouse_x;
	int32					mouse_y;
	int32					mouse_button;
	uint8					key;
	struct CommonImage *	screen;
} * WindowEventParamsPtr;

typedef void (* WindowEvent)(struct Window * window, struct WindowEventParams * params);

typedef struct Window
{
	uint32				id;
	int32				x;
	int32				y;
	uint32				width;
	uint32				height;
	int8				title[1024];
	uint32				bgcolor;
	int32				state;
	int32				has_close_button;
	int32				has_hidden_button;
	int32				over_close_button;
	int32				over_hidden_button;
	WindowEvent			event;
	struct CommonImage	workspace;
	struct CommonImage	image;
	uint8				key_buffer[WINDOW_KEY_BUFFER_SIZE];
	uint32				key_count;
} * WindowPtr;

extern
BOOL
init_window_resources(void);

extern
void
destroy_window_resources(void);

extern
BOOL
render_window(	IN struct Window * window,
				OUT struct CommonImage * image,
				IN BOOL top);

extern
uint8
window_get_key(IN struct Window * window);

#define	window_has_key(window)	((window) == NULL ? 0 : (window)->key_count)

#endif
