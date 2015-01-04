//Filename:		window.h
//Author:		Ihsoh
//Date:			2014-11-20
//Descriptor:	Window

#ifndef	_WINDOW_H_
#define	_WINDOW_H_

#include "types.h"
#include "image.h"

#define	TITLE_BAR_HEIGHT		30
#define	TITLE_BAR_BGCOLOR		0xff2474d8
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

struct WindowEventParams
{
	int32					event_type;
	int32					mouse_x;
	int32					mouse_y;
	int32					mouse_button;
	uint8					key;
	struct CommonImage *	screen;
};

typedef void (* WindowEvent)(struct Window * window, struct WindowEventParams * params);

struct Window
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
};

extern BOOL init_window_resources(void);
extern void destroy_window_resources(void);
extern BOOL render_window(struct Window * window, struct CommonImage * image, int32 top);
extern uint8 window_get_key(struct Window * window);

#define	window_has_key(window)	((window) == NULL ? 0 : (window)->key_count)

#endif
