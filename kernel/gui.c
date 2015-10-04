/**
	@File:			gui.c
	@Author:		Ihsoh
	@Date:			2015-10-05
	@Description:
		用户模式GUI功能。
*/

#include "gui.h"
#include "types.h"
#include "tasks.h"
#include "memory.h"
#include "screen.h"
#include "vesa.h"
#include "lock.h"

#include <dslib/list.h>
#include <dslib/linked_list.h>
#include <dslib/value.h>

#include <ilib/string.h>

DEFINE_LOCK_IMPL(gui)

typedef struct
{
	int32				tid;
	WindowPtr			window;
	DSLLinkedListPtr	messages;
	DSLListPtr			controls;	
} WindowInstance, * WindowInstancePtr;

static DSLListPtr		_winstances = NULL;

BOOL
gui_init(void)
{
	_winstances = dsl_lst_new();
	if(_winstances == NULL)
		return FALSE;
	return TRUE;
}

BOOL
gui_is_valid(void)
{
	if(vesa_is_valid())
		return TRUE;
	else
		return FALSE;
}

static
void
_window_event(	struct Window * window,
				struct WindowEventParams * params)
{

}

int32
gui_create_window(	IN int32		tid,
					IN uint32		width,
					IN uint32		height,
					IN uint32		bgcolor,
					IN uint32		style,
					IN CASCTEXT		title)
{
	WindowPtr window = NULL;
	DSLLinkedListPtr messages = NULL;
	DSLListPtr controls = NULL;
	WindowInstancePtr winstance = NULL;
	window = create_window(	width,
							height,
							bgcolor,
							style,
							title,
							_window_event);
	if(window == NULL)
		goto err;
	messages = dsl_lnklst_new();
	if(messages == NULL)
		goto err;
	controls = dsl_lst_new();
	if(controls == NULL)
		goto err;
	winstance = (WindowInstancePtr)alloc_memory(sizeof(WindowInstance));
	if(winstance == NULL)
		goto err;
	winstance->tid = tid;
	winstance->window = window;
	winstance->messages = messages;
	winstance->controls = controls;
	DSLValuePtr v = dsl_val_object(winstance);
	lock();
	int32 wid = dsl_lst_find_value(_winstances, NULL);
	if(wid == -1)
	{
		if(!dsl_lst_add_value(_winstances, v))
			goto err;
		unlock();
		return dsl_lst_find_value(_winstances, v);
	}
	else
	{
		dsl_lst_set(_winstances, wid, v);
		unlock();
		return wid;
	}
err:
	if(window != NULL)
		free_memory(window);
	if(messages != NULL)
		dsl_lnklst_free(messages);
	if(controls != NULL)
		dsl_lst_free(controls);
	if(winstance != NULL)
		free_memory(winstance);
	unlock();
	return -1;
}

#define _WINSTANCE_FALSE	\
	DSLValuePtr v = dsl_lst_get(_winstances, wid);	\
	if(v == NULL)	\
		return FALSE;	\
	WindowInstancePtr winstance = (WindowInstancePtr)DSL_OBJECTVAL(v);	\
	if(winstance == NULL || winstance->tid != tid)	\
		return FALSE;

BOOL
gui_close_window(	IN int32 tid,
					IN int32 wid)
{
	_WINSTANCE_FALSE
	destroy_window(winstance->window);
	dsl_lnklst_delete_all_object_node(winstance->messages);
	dsl_lst_delete_all_object_value(winstance->controls);
	dsl_lst_set(_winstances, wid, NULL);
	free_memory(winstance);
	free_memory(v);
	return TRUE;
}

void
gui_close_windows(IN int32 tid)
{
	lock();
	uint32 wid = 0;
	uint32 count = _winstances->count;
	for(wid = 0; wid < count; wid++)
	{
		DSLValuePtr v = dsl_lst_get(_winstances, wid);
		if(v != NULL)
		{
			WindowInstancePtr winstance = (WindowInstancePtr)DSL_OBJECTVAL(v);
			if(winstance->tid == tid)
				gui_close_window(tid, wid);
		}
	}
	unlock();
}

BOOL
gui_set_window_state(	IN int32 tid,
						IN int32 wid,
						IN uint32 state)
{
	_WINSTANCE_FALSE
	winstance->window->state = state;
	return TRUE;
}

BOOL
gui_get_window_state(	IN int32 tid,
						IN int32 wid,
						IN uint32 * state)
{
	_WINSTANCE_FALSE
	if(state == NULL)
		return FALSE;
	*state = winstance->window->state;
	return TRUE;
}

BOOL
gui_set_window_position(IN int32 tid,
						IN int32 wid,
						IN int32 x,
						IN int32 y)
{
	_WINSTANCE_FALSE
	winstance->window->x = x;
	winstance->window->y = y;
	return TRUE;
}

BOOL
gui_get_window_position(IN int32 tid,
						IN int32 wid,
						IN int32 * x,
						IN int32 * y)
{
	_WINSTANCE_FALSE
	if(x == NULL || y == NULL)
		return FALSE;
	*x = winstance->window->x;
	*y = winstance->window->y;
	return TRUE;
}

BOOL
gui_set_window_size(IN int32 tid,
					IN int32 wid,
					IN uint32 width,
					IN uint32 height)
{
	_WINSTANCE_FALSE

	// 未实现！！！
	return FALSE;

	return TRUE;
}

BOOL
gui_get_window_size(IN int32 tid,
					IN int32 wid,
					IN uint32 * width,
					IN uint32 * height)
{
	_WINSTANCE_FALSE
	if(width == NULL || height == NULL)
		return FALSE;
	*width = winstance->window->width;
	*height = winstance->window->height;
	return TRUE;
}

BOOL
gui_set_window_title(	IN int32 tid,
						IN int32 wid,
						IN CASCTEXT title)
{
	_WINSTANCE_FALSE
	if(strlen(title) >= sizeof(winstance->window->title))
		return FALSE;
	if(title == NULL)
		return FALSE;
	strcpy(winstance->window->title, title);
	return TRUE;
}

BOOL
gui_get_window_title(	IN int32 tid,
						IN int32 wid,
						OUT ASCTEXT title)
{
	_WINSTANCE_FALSE
	if(title == NULL)
		return FALSE;
	strcpy(title, winstance->window->title);
	return TRUE;
}
