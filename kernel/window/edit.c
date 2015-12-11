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

static
void
_edit(	IN OUT EditPtr edit,
		IN uint32 chr,
		IN uint32 chr1)
{
	if(edit == NULL)
		return;
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

BOOL
edit_set_text(	IN EditPtr edit,
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
		if(chr == '\n')
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

uint32
edit_get_width(IN EditPtr edit)
{
	if(edit == NULL)
		return 0;
	return edit->scr_column * ENFONT_WIDTH;
}

uint32
edit_get_height(IN EditPtr edit)
{
	if(edit == NULL)
		return 0;
	return edit->scr_row * (ENFONT_HEIGHT + CURSOR_HEIGHT);
}
