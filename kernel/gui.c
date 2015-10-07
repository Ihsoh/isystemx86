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
#include "kernel.h"
#include "mouse.h"
#include "keyboard.h"
#include "window.h"
#include "image.h"
#include "enfont.h"

#include "window/button.h"
#include "window/label.h"

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
	BOOL top = get_top_window() == window;
	switch(params->event_type)
	{
		case WINDOW_EVENT_PAINT:
		{
			int32 wid = window->uwid;
			DSLValuePtr v = dsl_lst_get(_winstances, wid);
			if(v != NULL)
			{
				WindowInstancePtr winstance = (WindowInstancePtr)DSL_OBJECTVAL(v);
				uint32 ui;
				for(ui = 0; ui < winstance->controls->count; ui++)
				{
					v = dsl_lst_get(winstance->controls, ui);
					ControlPtr control = (ControlPtr)DSL_OBJECTVAL(v);
					switch(control->type)
					{
						case CONTROL_BUTTON:
						{
							BUTTON(control, &window->workspace, params, top);
							break;
						}
						case CONTROL_LABEL:
						{
							LABEL(control, &window->workspace, params, top);
							break;
						}
					}
				}
			}
			break;
		}
		case WINDOW_EVENT_WILL_CLOSE:
		{
			DSLValuePtr v = dsl_lst_get(_winstances, window->uwid);
			if(v == NULL)
				return;
			WindowInstancePtr winstance = (WindowInstancePtr)DSL_OBJECTVAL(v);
			if(winstance == NULL)
				return;
			gui_clear_messages(winstance->tid, window->uwid);
			gui_push_message(	winstance->tid,
								window->uwid,
								-1,
								WINDOW_WILL_CLOSE,
								NULL,
								NULL);
			break;
		}
	}
}

int32
gui_create_window(	IN int32		tid,
					IN uint32		width,
					IN uint32		height,
					IN uint32		bgcolor,
					IN uint32		style,
					IN CASCTEXT		title)
{
	lock();
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
	if(v == NULL)
		goto err;
	int32 wid = dsl_lst_find_value(_winstances, NULL);
	if(wid == -1)
	{
		if(!dsl_lst_add_value(_winstances, v))
			goto err;
		wid = dsl_lst_find_value(_winstances, v);
	}
	else
		dsl_lst_set(_winstances, wid, v);
	window->uwid = wid;
	unlock();
	return wid;
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
	DSLValuePtr __v = dsl_lst_get(_winstances, wid);	\
	if(__v == NULL)	\
		return FALSE;	\
	WindowInstancePtr winstance = (WindowInstancePtr)DSL_OBJECTVAL(__v);	\
	if(winstance == NULL || winstance->tid != tid)	\
		return FALSE;

BOOL
gui_clear_messages(	IN int32 tid,
					IN int32 wid)
{
	_WINSTANCE_FALSE
	lock();
	void * phyaddr_data = NULL;
	while(gui_pop_message(tid, wid, NULL, NULL, NULL, &phyaddr_data))
		if(phyaddr_data != NULL)
			free_memory(phyaddr_data);
	unlock();
	return TRUE;
}

BOOL
gui_close_window(	IN int32 tid,
					IN int32 wid)
{
	_WINSTANCE_FALSE
	lock();
	destroy_window(winstance->window);
	gui_clear_messages(tid, wid);
	dsl_lnklst_free(winstance->messages);
	dsl_lst_delete_all_object_value(winstance->controls);
	dsl_lst_free(winstance->controls);
	dsl_lst_set(_winstances, wid, NULL);
	free_memory(winstance);
	free_memory(__v);
	unlock();
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

BOOL
gui_has_key(IN int32 tid,
			IN int32 wid,
			OUT uint32 * key)
{
	_WINSTANCE_FALSE
	int32 count = window_has_key(winstance->window);
	if(count > 0)
	{
		if(key != NULL)
			*key = window_peek_key(winstance->window);
		return TRUE;
	}
	else
		return FALSE;
}

BOOL
gui_get_key(IN int32 tid,
			IN int32 wid,
			OUT uint32 * key)
{
	_WINSTANCE_FALSE
	if(key == NULL)
		return FALSE;
	int32 count = window_has_key(winstance->window);
	if(count > 0)
	{
		*key = window_get_key(winstance->window);
		return TRUE;
	}
	else
		return FALSE;
}

BOOL
gui_get_mouse(	IN int32 tid,
				IN int32 wid,
				OUT int32 * x,
				OUT int32 * y,
				OUT uint32 * button)
{
	_WINSTANCE_FALSE
	if(x == NULL || y == NULL || button == NULL)
		return FALSE;
	WindowPtr window = winstance->window;
	uint32 wstyle = window->style;
	BOOL has_title_bar = !(wstyle & WINDOW_STYLE_NO_TITLE);
	get_mouse_position(x, y);
	*x = *x - window->x;
	if(has_title_bar)
		*y = *y - window->y - TITLE_BAR_HEIGHT;
	else
		*y = *y - window->y;
	*button = MOUSE_BUTTON_NONE;
	if(is_mouse_left_button_down())
		*button |= MOUSE_BUTTON_LEFT;
	if(is_mouse_right_button_down())
		*button |= MOUSE_BUTTON_RIGHT;
	return TRUE;
}

BOOL
gui_focused(IN int32 tid,
			IN int32 wid)
{
	_WINSTANCE_FALSE
	if(get_top_window() == winstance->window)
		return TRUE;
	else
		return FALSE;
}

BOOL
gui_focus(	IN int32 tid,
			IN int32 wid)
{
	_WINSTANCE_FALSE
	return set_top_window(winstance->window);
}

BOOL
gui_set_pixel(	IN int32 tid,
				IN int32 wid,
				IN int32 x,
				IN int32 y,
				IN uint32 pixel)
{
	_WINSTANCE_FALSE
	return set_pixel_common_image(	&winstance->window->workspace,
									x,
									y,
									pixel);
}

BOOL
gui_get_pixel(	IN int32 tid,
				IN int32 wid,
				IN int32 x,
				IN int32 y,
				OUT uint32 * pixel)
{
	_WINSTANCE_FALSE
	if(pixel == NULL)
		return FALSE;
	*pixel = get_pixel_common_image(&winstance->window->workspace,
									x,
									y);
	return TRUE;
}

BOOL
gui_draw_rect(	IN int32 tid,
				IN int32 wid,
				IN int32 x,
				IN int32 y,
				IN int32 width,
				IN int32 height,
				IN uint32 pixel)
{
	_WINSTANCE_FALSE
	return rect_common_image(	&winstance->window->workspace,
								x,
								y,
								width,
								height,
								pixel);
}

BOOL
gui_draw_image(	IN int32 tid,
				IN int32 wid,
				IN int32 x,
				IN int32 y,
				IN int32 width,
				IN int32 height,
				IN ImagePtr image)
{
	_WINSTANCE_FALSE
	if(image == NULL)
		return FALSE;
	return draw_common_image(	&winstance->window->workspace,
								image,
								x,
								y,
								width,
								height);
}

BOOL
gui_draw_text(	IN int32 tid,
				IN int32 wid,
				IN int32 x,
				IN int32 y,
				IN CASCTEXT text,
				IN uint32 color)
{
	_WINSTANCE_FALSE
	if(text == NULL)
		return FALSE;
	return text_common_image_ml(&winstance->window->workspace,
								x,
								y,
								get_enfont_ptr(),
								text,
								strlen(text),
								color);
}

BOOL
gui_draw_line(	IN int32 tid,
				IN int32 wid,
				IN int32 startx,
				IN int32 starty,
				IN int32 endx,
				IN int32 endy,
				IN uint32 color)
{
	_WINSTANCE_FALSE
	return line_common_image(	&winstance->window->workspace,
								startx,
								starty,
								endx,
								endy,
								color);
}

BOOL
gui_push_message(	IN int32 tid,
					IN int32 wid,
					IN int32 cid,
					IN uint32 type,
					IN void * data,
					IN void * phyaddr_data)
{
	_WINSTANCE_FALSE
	if(winstance->messages->count >= GUI_MAX_MESSAGE_COUNT)
		return FALSE;
	lock();
	WindowMessagePtr msg = NULL;
	DSLLinkedListNodePtr node = NULL;
	msg = NEW(WindowMessage);
	if(msg == NULL)
		goto err;
	msg->wid = wid;
	msg->cid = cid;
	msg->type = type;
	msg->data = data;
	msg->phyaddr_data = phyaddr_data;
	node = dsl_lnklst_new_object_node(msg);
	if(node == NULL)
		goto err;
	dsl_lnklst_add_node(winstance->messages, node);
	unlock();
	return TRUE;
err:
	if(msg != NULL)
		DELETE(msg);
	if(node != NULL)
		DELETE(node);
	unlock();
	return FALSE;
}

BOOL
gui_pop_message(IN int32 tid,
				IN int32 wid,
				OUT int32 * cid,
				OUT uint32 * type,
				OUT void ** data,
				OUT void ** phyaddr_data)
{
	_WINSTANCE_FALSE
	if(winstance->messages->count == 0)
		return FALSE;
	DSLLinkedListNodePtr node = dsl_lnklst_shift_node(winstance->messages);
	if(node == NULL)
		return FALSE;
	WindowMessagePtr msg = node->value.value.object_value;
	if(cid != NULL)
		*cid = msg->cid;
	if(type != NULL)
		*type = msg->type;
	if(data != NULL)
		*data = msg->data;
	if(phyaddr_data != NULL)
		*phyaddr_data = msg->phyaddr_data;
	DELETE(msg);
	DELETE(node);
	return TRUE;
}

BOOL
gui_set_text(	IN int32 tid,
				IN int32 wid,
				IN int32 cid,
				IN CASCTEXT text)
{
	_WINSTANCE_FALSE
	if(text == NULL)
		return FALSE;
	DSLValuePtr v = dsl_lst_get(winstance->controls, cid);
	ControlPtr control = (ControlPtr)DSL_OBJECTVAL(v);
	switch(control->type)
	{
		case CONTROL_BUTTON:
		{
			if(strlen(text) > MAX_BUTTON_TEXT_LEN)
				return FALSE;
			ButtonPtr button = (ButtonPtr)control;
			SET_BUTTON_TEXT(button, text);
			break;
		}
		case CONTROL_LABEL:
		{
			if(strlen(text) > MAX_LABEL_TEXT_LEN)
				return FALSE;
			LabelPtr label = (LabelPtr)control;
			SET_LABEL_TEXT(label, text);
			break;
		}
		default:
			return FALSE;
	}
	return TRUE;
}

BOOL
gui_get_text(	IN int32 tid,
				IN int32 wid,
				IN int32 cid,
				OUT ASCTEXT text,
				IN uint32 bufsz)
{
	_WINSTANCE_FALSE
	if(text == NULL)
		return FALSE;
	DSLValuePtr v = dsl_lst_get(winstance->controls, cid);
	ControlPtr control = (ControlPtr)DSL_OBJECTVAL(v);
	CASCTEXT ctl_text = NULL;
	switch(control->type)
	{
		case CONTROL_BUTTON:
		{
			ButtonPtr button = (ButtonPtr)control;
			ctl_text = button->text;
			break;
		}
		case CONTROL_LABEL:
		{
			LabelPtr label = (LabelPtr)control;
			ctl_text = label->text;
			break;
		}
		default:
			return FALSE;
	}
	if(ctl_text != NULL)
		if(strlen(ctl_text) < bufsz)
			strcpy(text, ctl_text);
		else
		{
			strncpy(text, ctl_text, bufsz - 1);
			text[bufsz - 1] = '\0';
		}
	else
		return FALSE;
	return TRUE;
}

BOOL
gui_update(	IN int32 tid,
			IN int32 wid)
{
	_WINSTANCE_FALSE
	window_dispatch_event(winstance->window, WINDOW_EVENT_PAINT, NULL);
	return TRUE;
}

static
void
_no_data_event(	IN uint32 id,
				IN uint32 type,
				IN void * param)
{
	ControlPtr ctrl = (ControlPtr)id;
	int32 wid = ctrl->uwid;
	int32 cid = ctrl->uwcid;
	DSLValuePtr v = dsl_lst_get(_winstances, wid);
	WindowInstancePtr winstance = (WindowInstancePtr)DSL_OBJECTVAL(v);
	int32 tid = winstance->tid;
	gui_push_message(tid, wid, cid, type, NULL, NULL);
}

BOOL
gui_new_button(	IN int32 tid,
				IN int32 wid,
				IN int32 x,
				IN int32 y,
				IN CASCTEXT text,
				OUT int32 * cid)
{
	_WINSTANCE_FALSE
	lock();
	ButtonPtr btn = NULL;
	DSLValuePtr val = NULL;
	if(cid == NULL)
		goto err;
	btn = NEW(Button);
	if(btn == NULL)
		goto err;
	INIT_BUTTON(btn, x, y, text, _no_data_event);
	btn->uwid = wid;
	val = dsl_val_object(btn);
	if(val == NULL)
		goto err;
	int32 _cid = dsl_lst_find_value(winstance->controls, NULL);
	if(_cid == -1)
	{
		if(!dsl_lst_add_value(winstance->controls, val))
			goto err;
		_cid = dsl_lst_find_value(winstance->controls, val);
	}
	else
		dsl_lst_set(winstance->controls, _cid, val);
	btn->uwcid = _cid;
	*cid = _cid;
	unlock();
	return TRUE;
err:
	if(btn != NULL)
		DELETE(btn);
	if(val != NULL)
		DELETE(val);
	unlock();
	return FALSE;
}

BOOL
gui_new_label(	IN int32 tid,
				IN int32 wid,
				IN int32 x,
				IN int32 y,
				IN CASCTEXT text,
				OUT int32 * cid)
{
	_WINSTANCE_FALSE
	lock();
	LabelPtr lbl = NULL;
	DSLValuePtr val = NULL;
	if(cid == NULL)
		goto err;
	lbl = NEW(Label);
	if(lbl == NULL)
		goto err;
	INIT_LABEL(lbl, x, y, text, _no_data_event);
	lbl->uwid = wid;
	val = dsl_val_object(lbl);
	if(val == NULL)
		goto err;
	int32 _cid = dsl_lst_find_value(winstance->controls, NULL);
	if(_cid == -1)
	{
		if(!dsl_lst_add_value(winstance->controls, val))
			goto err;
		_cid = dsl_lst_find_value(winstance->controls, val);
	}
	else
		dsl_lst_set(winstance->controls, _cid, val);
	lbl->uwcid = _cid;
	*cid = _cid;
	unlock();
	return TRUE;
err:
	if(lbl != NULL)
		DELETE(lbl);
	if(val != NULL)
		DELETE(val);
	unlock();
	return FALSE;
}
