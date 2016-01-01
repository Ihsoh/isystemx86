/**
	@File:			edit.c
	@Author:		Ihsoh
	@Date:			2015-12-11
	@Description:
		窗体控件 - Edit。
*/

#include "edit.h"

#include "../memory.h"
#include "../enfont.h"
#include "../keyboard.h"
#include "../screen.h"
#include "../window.h"

#define	MAX_COLUMN				MAX_EDIT_COLUMN			// 列数的最大值。
#define	MAX_ROW					MAX_EDIT_ROW			// 行数的最大值。

#define	EDITAREA_X(_edit) ((_edit)->cursor_x)			// 光标相对内容编辑区的X坐标。
#define	EDITAREA_Y(_edit) ((_edit)->cursor_y)			// 光标相对内容编辑区的Y坐标。

#define	SBUFFER_CHR(n)	(n * 2)					// 屏幕缓冲区中第n个字符的偏移值。
#define	SBUFFER_PRO(n)	(n * 2 + 1)				// 屏幕缓冲区中第n个字符的属性的偏移值。

#define	_WINDOW_WIDTH(_edit)	((_edit)->scr_column * ENFONT_WIDTH)
#define	_WINDOW_HEIGHT(_edit)	((_edit)->scr_row * (ENFONT_HEIGHT + CURSOR_HEIGHT))

#define	_SBROW(_edit, _r)	((_edit)->content_buffer + (_r) * MAX_COLUMN)

static
BOOL
_init(	OUT EditPtr edit,
		IN uint32 scr_column,
		IN uint32 scr_row);

static
void
_uninit(IN EditPtr edit);

static
uint32
_get_line_len(	IN EditPtr edit,
				IN uint32 line);

static
void
_edit(	IN OUT EditPtr edit,
		IN uint32 chr,
		IN uint32 chr1);

static
void
_strcpy_sbuffer(OUT ASCCHAR * dest,
				IN CASCCHAR * src);

static
void
_render(IN EditPtr edit,
		IN OUT ImagePtr image);

static
void
_flush(IN OUT EditPtr edit);

/**
	@Function:		_init
	@Access:		Private
	@Description:
		初始化EDIT控件。
	@Parameters:
		edit, EditPtr, OUT
			指向EDIT控件对象的指针。给对象的各个字段填充初始值、创建内容缓冲区和显示缓冲区。
		scr_row, uint32, IN
			编辑区行数。
		scr_column, uint32, IN
			编辑区列数。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
static
BOOL
_init(	OUT EditPtr edit,
		IN uint32 scr_row,
		IN uint32 scr_column)
{
	if(edit == NULL)
		return FALSE;
	// 填充初始值。
	edit->content_buffer = NULL;
	edit->screen_buffer = NULL;
	edit->scr_column = scr_column;
	edit->scr_row = scr_row;
	edit->row = edit->scr_row;
	edit->column = edit->scr_column;
	edit->cursor_x = 0;
	edit->cursor_y = 0;
	edit->top_row = 0;
	edit->top_column = 0;

	// 创建内容缓冲区。
	uint32 ui;
	edit->content_buffer = (ASCCHAR *)alloc_memory(MAX_ROW * MAX_COLUMN);
	if(edit->content_buffer == NULL)
		return FALSE;
	for(ui = 0; ui < MAX_ROW * MAX_COLUMN; ui++)
		edit->content_buffer[ui] = '\0';

	// 创建显示缓冲区。
	edit->screen_buffer = (uint8 *)alloc_memory(edit->scr_row * edit->scr_column * 2);
	if(edit->screen_buffer == NULL)
		return FALSE;
	for(ui = 0; ui < edit->scr_row * edit->scr_column * 2; ui++)
		edit->screen_buffer[ui] = 0;

	return TRUE;
}

/**
	@Function:		_uninit
	@Access:		Private
	@Description:
		释放EDIT控件的内容缓冲区和显示缓冲区。
	@Parameters:
		edit, EditPtr, IN
			指向EDIT控件对象的指针。
	@Return:
*/
static
void
_uninit(IN EditPtr edit)
{
	if(edit == NULL)
		return;
	if(edit->content_buffer != NULL)
		free_memory(edit->content_buffer);
	if(edit->screen_buffer != NULL)
		free_memory(edit->screen_buffer);
}

/**
	@Function:		_get_line_len
	@Access:		Private
	@Description:
		获取指定行的长度。
	@Parameters:
		edit, EditPtr, IN
			指向EDIT控件对象的指针。
		line, uint32, IN
			行数，从0开始。
	@Return:
		指定行的长度。
*/
static
uint32
_get_line_len(	IN EditPtr edit,
				IN uint32 line)
{
	if(edit == NULL)
		return 0;
	uint32 len = 0;
	for(len = 0; len < MAX_COLUMN && _SBROW(edit, line)[len] != '\0'; len++);
	return len;
}

/**
	@Function:		_edit
	@Access:		Private
	@Description:
		处理输入的按键。
	@Parameters:
		edit, EditPtr, IN OUT
			指向EDIT控件对象的指针。
		chr, uint32, IN
			输入的按键值。如果按键值为KEY_EXT，则具体的执行根据chr1而定。
		chr1, uint32, IN
			如果chr为KEY_EXT，则该参数的值为扩展按键值，否则这个参数值为0。
	@Return:
*/
static
void
_edit(	IN OUT EditPtr edit,
		IN uint32 chr,
		IN uint32 chr1)
{
	if(edit == NULL)
		return;
	ControlEvent event = edit->event;
	if(event != NULL)
	{
		uint32 data[2];
		data[0] = chr;
		data[1] = chr1;
		event(edit->id, EDIT_INPUT, data);
	}
	if(chr == KEY_EXT)
		switch(chr1)
		{
			case KEY_UP:
				if(EDITAREA_Y(edit) != 0)
					edit->cursor_y--;
				else if(edit->top_row != 0)
					edit->top_row--;
				break;
			case KEY_DOWN:
				if(edit->top_row + EDITAREA_Y(edit) + 1 < edit->total_line && EDITAREA_Y(edit) + 1 < edit->row)
					edit->cursor_y++;
				else if(edit->top_row + edit->row < edit->total_line && edit->top_row + edit->row < MAX_ROW)
					edit->top_row++;
				break;
			case KEY_LEFT:
				if(EDITAREA_X(edit) != 0)
					edit->cursor_x--;
				else if(edit->top_column != 0)
					edit->top_column--;
				break;
			case KEY_RIGHT:
				if(EDITAREA_X(edit) + 1 < edit->column)
					edit->cursor_x++;
				else if(edit->top_column + edit->column < MAX_COLUMN)
					edit->top_column++;
				break;
			default:
				break;
		}
	else
	{
		if(edit->style & EDIT_STYLE_READONLY)
			return;
		switch(chr)
		{
			case 8:
			{
				uint32 prev_line_len, curr_line_len;
				if((edit->top_column + EDITAREA_X(edit)) != 0)
				{
					uint32 ui;
					for(ui = edit->top_column + EDITAREA_X(edit); ui < MAX_COLUMN; ui++)
						_SBROW(edit, edit->top_row + EDITAREA_Y(edit))[ui - 1]
							= _SBROW(edit, edit->top_row + EDITAREA_Y(edit))[ui];
					_SBROW(edit, edit->top_row + EDITAREA_Y(edit))[MAX_COLUMN - 1] = '\0';
					if(EDITAREA_X(edit) != 0)
						edit->cursor_x--;
					else
						edit->top_column--;
				}
				else if(edit->top_row + EDITAREA_Y(edit) != 0 
						&& (prev_line_len = _get_line_len(edit, edit->top_row + EDITAREA_Y(edit) - 1)) 
												+ (curr_line_len = _get_line_len(edit, edit->top_row + EDITAREA_Y(edit))) 
								< MAX_COLUMN)
				{
					memcpy(	_SBROW(edit, edit->top_row + EDITAREA_Y(edit) - 1) + prev_line_len,
							_SBROW(edit, edit->top_row + EDITAREA_Y(edit)),
							curr_line_len);
					int32 i;
					for(i = edit->top_row + EDITAREA_Y(edit); i < edit->total_line - 1; i++)
						memcpy(_SBROW(edit, i), _SBROW(edit, i + 1), MAX_COLUMN);
					if(EDITAREA_Y(edit) != 0)
						edit->cursor_y--;
					else
						edit->top_row--;
					for(i = 0; i < MAX_COLUMN; i++)
						_SBROW(edit, edit->total_line - 1)[i] = '\0';
					edit->total_line--;
					edit->cursor_x = prev_line_len;
				}
				break;
			}
			case '\n':
				if(!(edit->style & EDIT_STYLE_SINGLE_LINE))
				{
					if(edit->total_line < MAX_ROW)
					{
						edit->total_line++;
						int32 i, i1;
						if(edit->total_line == 0)
							for(i = 0; i < MAX_COLUMN; i++)
								_SBROW(edit, edit->top_row + EDITAREA_Y(edit) + 1)[i]
									= _SBROW(edit, edit->top_row + EDITAREA_Y(edit))[i];
						else
							for(i = edit->total_line; i > edit->top_row + EDITAREA_Y(edit) + 1; i--)
								for(i1 = 0; i1 < MAX_COLUMN; i1++)
									_SBROW(edit, i)[i1] = _SBROW(edit, i - 1)[i1];
						for(i = 0; i < MAX_COLUMN; i++)
							_SBROW(edit, edit->top_row + EDITAREA_Y(edit) + 1)[i] = '\0';
						for(i = edit->top_column + EDITAREA_X(edit), i1 = 0; i < MAX_COLUMN; i++, i1++)
						{
							_SBROW(edit, edit->top_row + EDITAREA_Y(edit) + 1)[i1]
								= _SBROW(edit, edit->top_row + EDITAREA_Y(edit))[i]; 
							_SBROW(edit, edit->top_row + EDITAREA_Y(edit))[i] = '\0';
						}
						edit->cursor_x = 0;
						if(EDITAREA_Y(edit) + 1 >= edit->row)
							edit->top_row++;
						else
							edit->cursor_y++;
					}
				}
				break;
			case '\t':
				break;
			default:
				if(edit->top_column + EDITAREA_X(edit) + 1 < MAX_COLUMN)
				{
					int32 i;
					for(i = MAX_COLUMN - 1; i > edit->top_column + EDITAREA_X(edit); i--)
						_SBROW(edit, edit->top_row + EDITAREA_Y(edit))[i]
							= _SBROW(edit, edit->top_row + EDITAREA_Y(edit))[i - 1];
					_SBROW(edit, edit->top_row + EDITAREA_Y(edit))[edit->top_column + EDITAREA_X(edit)] = chr;
					for(i = 0; i < edit->top_column + EDITAREA_X(edit); i++)
						if(_SBROW(edit, edit->top_row + EDITAREA_Y(edit))[i] == 0)
							_SBROW(edit, edit->top_row + EDITAREA_Y(edit))[i] = ' ';
					if(EDITAREA_X(edit) + 1 < edit->column)
						edit->cursor_x++;
					else if(edit->top_column + edit->column < MAX_COLUMN)
						edit->top_column++;
					if(edit->top_column + EDITAREA_Y(edit) == edit->total_line)
						edit->total_line++;
				}
				break;
		}
	}
}

/**
	@Function:		_strcpy_sbuffer
	@Access:		Private
	@Description:
		复制文本到显示缓冲区。
	@Parameters:
		dest, ASCCHAR *, OUT
			指向显示缓冲区的指针。
		src, CASCCHAR *, IN
			指向文本缓冲区的指针。
	@Return:
*/
static
void
_strcpy_sbuffer(OUT ASCCHAR * dest,
				IN CASCCHAR * src)
{
	while(*src != '\0')
	{
		*dest = *src;
		dest += 2;
		src++;
	}
}

/**
	@Function:		_flush
	@Access:		Private
	@Description:
		把当前的内容缓冲区的部分更新到显示缓冲区。
	@Parameters:
		edit, EditPtr, IN OUT
			指向EDIT控件对象的指针。
	@Return:
*/
static
void
_flush(IN OUT EditPtr edit)
{
	uint32 ui, ui1;
	for(ui = edit->top_row; ui < edit->top_row + edit->row; ui++)
	{
		// 输出内容到内容编辑区。
		ASCCHAR * line = _SBROW(edit, ui);
		for(ui1 = 0; ui1 < edit->column; ui1++)
		{
			uint32 offset = ((ui - edit->top_row) * edit->scr_column + ui1) * 2;
			edit->screen_buffer[offset + 0] 
				= ((line[edit->top_column + ui1] == '\0' || line[edit->top_column + ui1] == '\t') 
					? ' ' 
					: line[edit->top_column + ui1]);
			edit->screen_buffer[offset + 1] = CC_GRAYWHITE;
		}
	}
}

/**
	@Function:		_render
	@Access:		Private
	@Description:
		渲染显示缓冲区。
	@Parameters:
		edit, EditPtr, IN
			指向EDIT控件对象的指针。
		image, ImagePtr, IN OUT
			指向图片对象的指针。
	@Return:
*/
static
void
_render(IN EditPtr edit,
		IN OUT ImagePtr image)
{
	_flush(edit);
	render_text_buffer_ex(	image,
							edit->x,
							edit->y,
							edit->screen_buffer,
							edit->scr_row,
							edit->scr_column,
							edit->cursor_x,
							edit->cursor_y);
}

/**
	@Function:		edit_init
	@Access:		Public
	@Description:
		渲染显示缓冲区。
	@Parameters:
		edit, EditPtr, OUT
			指向EDIT控件对象的指针。
		id, uint32, IN
			控件ID。如果为0，则把控件对象的地址作为ID。
		x, uint32, IN
			X坐标。
		y, uint32, IN
			Y坐标。
		row, uint32, IN
			编辑区行数。
		column, uint32, IN
			编辑区列数。
		style, uint32, IN
			样式。
		event, ControlEvent, IN
			事件。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
edit_init(	OUT EditPtr edit,
			IN uint32 id,
			IN uint32 x,
			IN uint32 y,
			IN uint32 row,
			IN uint32 column,
			IN uint32 style,
			IN ControlEvent event)
{
	if(edit == NULL)
		return FALSE;
	if(id == 0)
		edit->id = (uint32)edit;
	else
		edit->id = id;
	edit->type = CONTROL_EDIT;
	edit->uwid = -1;
	edit->uwcid = -1;
	edit->x = x;
	edit->y = y;
	edit->enabled = TRUE;
	edit->style = style;
	edit->event = event;

	edit->_forced_flush = TRUE;

	_init(edit, row, column);
	return TRUE;
}

/**
	@Function:		edit_uninit
	@Access:		Public
	@Description:
		销毁EDIT的资源。
	@Parameters:
		edit, EditPtr, IN
			指向EDIT控件对象的指针。
	@Return:
*/
void
edit_uninit(IN EditPtr edit)
{
	_uninit(edit);
}

/**
	@Function:		nmledit
	@Access:		Public
	@Description:
		EDIT的渲染、事件处理函数。
	@Parameters:
		edit, EditPtr, IN OUT
			指向EDIT控件对象的指针。
		image, ImagePtr, OUT
			指向目标图片对象的指针。
		params, WindowEventParamsPtr, IN
			指向窗口事件参数的指针。
		top, BOOL, IN
			指示EDIT所在的窗体是否在最顶层。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
nmledit(IN OUT EditPtr edit,
		OUT ImagePtr image,
		IN WindowEventParamsPtr params,
		BOOL top)
{
	if(edit == NULL || image == NULL || params == NULL)
		return FALSE;
	if(!top && !(edit->style & EDIT_STYLE_REFRESH))
		return TRUE;
	uint32 x = edit->x;
	uint32 y = edit->y;
	uint32 width = _WINDOW_WIDTH(edit);
	uint32 height = _WINDOW_HEIGHT(edit);
	WindowPtr window = params->window;
	if(	point_in_rect(	params->mouse_x, params->mouse_y,
						x, y,
						width, height)
		&& edit->enabled)
	{
		if(is_mouse_left_button_down())
		{
			window_clear_key(window);
			window_focus_ctrl(window, edit->id);
		}
	}
	BOOL changed = FALSE;
	if(window->focused_ctrl == edit->id)
		while(window_has_key(window))
		{
			uint8 chr = window_get_key(window);
			if(chr == KEY_EXT)
			{
				while(!window_has_key(window));
				uint8 chr1 = window_get_key(window);
				_edit(edit, chr, chr1);
			}
			else
				_edit(edit, chr, 0);
			changed = TRUE;
		}
	if(changed || edit->_forced_flush)
	{
		_render(edit, image);
		edit->_forced_flush = FALSE;
	}
	return TRUE;
}

/**
	@Function:		edit_get_text
	@Access:		Public
	@Description:
		获取EDIT控件的文本。
	@Parameters:
		edit, EditPtr, IN
			指向EDIT控件对象的指针。
		text, ASCTEXT, OUT
			指向储存文本的缓冲区的指针。
		size, uint32, IN
			缓冲区大小。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
edit_get_text(	IN EditPtr edit,
				OUT ASCTEXT text,
				IN uint32 size)
{
	if(	edit == NULL
		|| text == NULL
		|| size == 0)
		return FALSE;
	size--;
	uint32 ui;
	for(ui = 0; ui < edit->total_line; ui++)
	{
		uint32 len;
		CASCTEXT row = _SBROW(edit, ui);
		for(len = 0; len < MAX_COLUMN && row[len] != '\0'; len++);
		uint32 ui1;
		for(ui1 = 0; ui1 < len; ui1++)
		{
			if(size != 0)
			{
				*(text++) = row[ui1];
				size--;
			}
			else
				goto end;
		}
		if(!(edit->style & EDIT_STYLE_SINGLE_LINE))
			if(size != 0)
			{
				*(text++) = '\n';
				size--;
			}
			else
				goto end;
	}
end:
	*text = '\0';
	return TRUE;
}

/**
	@Function:		edit_set_text
	@Access:		Public
	@Description:
		设置EDIT控件的文本。
	@Parameters:
		edit, EditPtr, OUT
			指向EDIT控件对象的指针。
		text, CASCTEXT, IN
			指向储存文本的缓冲区的指针。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
edit_set_text(	OUT EditPtr edit,
				IN CASCTEXT text)
{
	if(	edit == NULL
		|| text == NULL)
		return FALSE;
	uint32 ui;
	uint32 row = 0;
	uint32 index = 0;
	uint32 len = strlen(text);
	for(ui = 0; ui < len; ui++)
	{
		CASCCHAR chr = text[ui];
		if(	!(edit->style & EDIT_STYLE_SINGLE_LINE)
			&& chr == '\n')
		{
			row++;
			index = 0;
		}
		else
			_SBROW(edit, row)[index++] = chr;
	}
	edit->total_line = row;
	if(index != 0)
		edit->total_line++;
	return TRUE;
}

/**
	@Function:		edit_get_width
	@Access:		Public
	@Description:
		获取EDIT控件的宽度。
	@Parameters:
		edit, EditPtr, IN
			指向EDIT控件对象的指针。
	@Return:
		uint32
			EDIT控件的宽度。
*/
uint32
edit_get_width(IN EditPtr edit)
{
	if(edit == NULL)
		return 0;
	return edit->scr_column * ENFONT_WIDTH;
}

/**
	@Function:		edit_get_height
	@Access:		Public
	@Description:
		获取EDIT控件的高度。
	@Parameters:
		edit, EditPtr, IN
			指向EDIT控件对象的指针。
	@Return:
		uint32
			EDIT控件的高度。
*/
uint32
edit_get_height(IN EditPtr edit)
{
	if(edit == NULL)
		return 0;
	return edit->scr_row * (ENFONT_HEIGHT + CURSOR_HEIGHT);
}
