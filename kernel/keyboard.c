/**
	@File:			keyboard.c
	@Author:		Ihsoh
	@Date:			2014-1-30
	@Description:
		提供键盘的功能。
*/

#include "types.h"
#include "keyboard.h"
#include "386.h"
#include "screen.h"
#include "tasks.h"
#include "kernel.h"
#include "sfunc_window.h"
#include "vesa.h"
#include "console_window.h"
#include "window.h"

#define	IBUFFER_SIZE	16
#define	KEY_MAP_SIZE	256	

static volatile uint8 	buffer[IBUFFER_SIZE];
static volatile int32 	key_map[KEY_MAP_SIZE];
static volatile int32 	bpos = -1;
static volatile BOOL 	shift = FALSE, control = FALSE, alt = FALSE, capslock = FALSE, ext = FALSE;

#define	is_upper() ((shift && !capslock) || (!shift && capslock))

/**
	@Function:		keydown
	@Access:		Private
	@Description:
		设置键盘状态表中的指定按键的状态为按下。
	@Parameters:
		key, int32, IN
			按键。
	@Return:	
*/
static
void
keydown(IN int32 key)
{
	key_map[key] = 1;
}

/**
	@Function:		keyup
	@Access:		Private
	@Description:
		设置键盘状态表中的指定按键的状态为弹起。
	@Parameters:
		key, int32, IN
			按键。
	@Return:	
*/
static
void
keyup(IN int32 key)
{
	key_map[key] = 0;
}

/**
	@Function:		add_key
	@Access:		Private
	@Description:
		向键盘队列添加一个按下的键。
	@Parameters:
		key, uint8, IN
			按键。
	@Return:	
*/
static
void
add_key(IN uint8 key)
{
	if(vesa_is_valid())
	{
		struct Window * top_window = get_top_window();
		if(top_window == console_window)
		{
			if(bpos + 1 < IBUFFER_SIZE)
				buffer[++bpos] = key;
		}
		else if(top_window != NULL)
			if(top_window->key_count < WINDOW_KEY_BUFFER_SIZE)
				top_window->key_buffer[top_window->key_count++] = key;
	}		
	else if(bpos + 1 < IBUFFER_SIZE)
		buffer[++bpos] = key;
}

/**
	@Function:		tran_key
	@Access:		Public
	@Description:
		把键盘扫描码转换为 ASCII 码。
	@Parameters:
		scan_code, uint8, IN
			键盘扫描码。
	@Return:
*/
void
tran_key(IN uint8 scan_code)
{
	if(scan_code == KEY_EXT)
	{
		ext = TRUE;
		return;
	}
	if(!ext)
		switch(scan_code)
		{
			case KEY_CAPSLOCK | KEY_KEYDOWN:
				capslock = capslock ? FALSE : TRUE;
				keydown(KEY_CAPSLOCK);
				break;
			case KEY_CAPSLOCK | KEY_KEYUP:
				keyup(KEY_CAPSLOCK);
				break;
			case KEY_LEFTCTR | KEY_KEYDOWN:
				control = TRUE;
				keydown(KEY_LEFTCTR);
				break;
			case KEY_LEFTCTR | KEY_KEYUP:
				control = FALSE;
				keyup(KEY_LEFTCTR);
				break;
			case KEY_LEFTSHIFT | KEY_KEYDOWN:
				shift = TRUE;
				keydown(KEY_LEFTSHIFT);
				break;
			case KEY_LEFTSHIFT | KEY_KEYUP:
				shift = FALSE;
				keyup(KEY_LEFTSHIFT);
				break;
			case KEY_LEFTALT | KEY_KEYDOWN:
				alt = TRUE;
				keydown(KEY_LEFTALT);
				break;
			case KEY_LEFTALT | KEY_KEYUP:
				alt = FALSE;
				keyup(KEY_LEFTALT);
				break;
			case KEY_ESC | KEY_KEYDOWN:
				add_key(27);
				keydown(KEY_ESC);
				break;
			case KEY_ESC | KEY_KEYUP:
				keyup(KEY_ESC);
				break;
			case KEY_A | KEY_KEYDOWN:
				add_key(is_upper() ? 'A' : 'a');
				keydown(KEY_A);
				break;
			case KEY_A | KEY_KEYUP:
				keyup(KEY_A);
				break;
			case KEY_B | KEY_KEYDOWN:
				add_key(is_upper() ? 'B' : 'b');
				keydown(KEY_B);
				break;
			case KEY_B | KEY_KEYUP:
				keyup(KEY_B);
				break;
			case KEY_C | KEY_KEYDOWN:
				add_key(is_upper() ? 'C' : 'c');
				keydown(KEY_C);
				break;
			case KEY_C | KEY_KEYUP:
				keyup(KEY_C);
				break;
			case KEY_D | KEY_KEYDOWN:
				add_key(is_upper() ? 'D' : 'd');
				keydown(KEY_D);
				break;
			case KEY_D | KEY_KEYUP:
				keyup(KEY_D);
				break;
			case KEY_E | KEY_KEYDOWN:
				add_key(is_upper() ? 'E' : 'e');
				keydown(KEY_E);
				break;
			case KEY_E | KEY_KEYUP:
				keyup(KEY_E);
				break;
			case KEY_F | KEY_KEYDOWN:
				add_key(is_upper() ? 'F' : 'f');
				keydown(KEY_F);
				break;
			case KEY_F | KEY_KEYUP:
				keyup(KEY_F);
				break;
			case KEY_G | KEY_KEYDOWN:
				add_key(is_upper() ? 'G' : 'g');
				keydown(KEY_G);
				break;
			case KEY_G | KEY_KEYUP:
				keyup(KEY_G);
				break;
			case KEY_H | KEY_KEYDOWN:
				add_key(is_upper() ? 'H' : 'h');
				keydown(KEY_H);
				break;
			case KEY_H | KEY_KEYUP:
				keyup(KEY_H);
				break;
			case KEY_I | KEY_KEYDOWN:
				add_key(is_upper() ? 'I' : 'i');
				keydown(KEY_I);
				break;
			case KEY_I | KEY_KEYUP:
				keyup(KEY_I);
				break;
			case KEY_J | KEY_KEYDOWN:
				add_key(is_upper() ? 'J' : 'j');
				keydown(KEY_J);
				break;
			case KEY_J | KEY_KEYUP:
				keyup(KEY_J);
				break;
			case KEY_K | KEY_KEYDOWN:
				add_key(is_upper() ? 'K' : 'k');
				keydown(KEY_K);
				break;
			case KEY_K | KEY_KEYUP:
				keyup(KEY_K);
				break;
			case KEY_L | KEY_KEYDOWN:
				add_key(is_upper() ? 'L' : 'l');
				keydown(KEY_L);
				break;
			case KEY_L | KEY_KEYUP:
				keyup(KEY_L);
				break;
			case KEY_M | KEY_KEYDOWN:
				add_key(is_upper() ? 'M' : 'm');
				keydown(KEY_M);
				break;
			case KEY_M | KEY_KEYUP:
				keyup(KEY_M);
				break;
			case KEY_N | KEY_KEYDOWN:
				add_key(is_upper() ? 'N' : 'n');
				keydown(KEY_N);
				break;
			case KEY_N | KEY_KEYUP:
				keyup(KEY_N);
				break;
			case KEY_O | KEY_KEYDOWN:
				add_key(is_upper() ? 'O' : 'o');
				keydown(KEY_O);
				break;
			case KEY_O | KEY_KEYUP:
				keyup(KEY_O);
				break;
			case KEY_P | KEY_KEYDOWN:
				add_key(is_upper() ? 'P' : 'p');
				keydown(KEY_P);
				break;
			case KEY_P | KEY_KEYUP:
				keyup(KEY_P);
				break;
			case KEY_Q | KEY_KEYDOWN:
				add_key(is_upper() ? 'Q' : 'q');
				keydown(KEY_Q);
				break;
			case KEY_Q | KEY_KEYUP:
				keyup(KEY_Q);
				break;
			case KEY_R | KEY_KEYDOWN:
				add_key(is_upper() ? 'R' : 'r');
				keydown(KEY_R);
				break;
			case KEY_R | KEY_KEYUP:
				keyup(KEY_R);
				break;
			case KEY_S | KEY_KEYDOWN:
				add_key(is_upper() ? 'S' : 's');
				keydown(KEY_S);
				break;
			case KEY_S | KEY_KEYUP:
				keyup(KEY_S);
				break;
			case KEY_T | KEY_KEYDOWN:
				add_key(is_upper() ? 'T' : 't');
				keydown(KEY_T);
				break;
			case KEY_T | KEY_KEYUP:
				keyup(KEY_T);
				break;
			case KEY_U | KEY_KEYDOWN:
				add_key(is_upper() ? 'U' : 'u');
				keydown(KEY_U);
				break;
			case KEY_U | KEY_KEYUP:
				keyup(KEY_U);
				break;
			case KEY_V | KEY_KEYDOWN:
				add_key(is_upper() ? 'V' : 'v');
				keydown(KEY_V);
				break;
			case KEY_V | KEY_KEYUP:
				keyup(KEY_V);
				break;
			case KEY_W | KEY_KEYDOWN:
				add_key(is_upper() ? 'W' : 'w');
				keydown(KEY_W);
				break;
			case KEY_W | KEY_KEYUP:
				keyup(KEY_W);
				break;
			case KEY_X | KEY_KEYDOWN:
				add_key(is_upper() ? 'X' : 'x');
				keydown(KEY_X);
				break;
			case KEY_X | KEY_KEYUP:
				keyup(KEY_X);
				break;
			case KEY_Y | KEY_KEYDOWN:
				add_key(is_upper() ? 'Y' : 'y');
				keydown(KEY_Y);
				break;
			case KEY_Y | KEY_KEYUP:
				keyup(KEY_Y);
				break;
			case KEY_Z | KEY_KEYDOWN:
				add_key(is_upper() ? 'Z' : 'z');
				keydown(KEY_Z);
				break;
			case KEY_Z | KEY_KEYUP:
				keyup(KEY_Z);
				break;
			case KEY_LQUOTE | KEY_KEYDOWN:
				add_key(shift ? '~' : '`');
				keydown(KEY_LQUOTE);
				break;
			case KEY_LQUOTE | KEY_KEYUP:
				keyup(KEY_LQUOTE);
				break;
			case KEY_1 | KEY_KEYDOWN:
				add_key(shift ? '!' : '1');
				keydown(KEY_1);
				break;
			case KEY_1 | KEY_KEYUP:
				keyup(KEY_1);
				break;
			case KEY_2 | KEY_KEYDOWN:
				add_key(shift ? '@' : '2');
				keydown(KEY_2);
				break;
			case KEY_2 | KEY_KEYUP:
				keyup(KEY_2);
				break;
			case KEY_3 | KEY_KEYDOWN:
				add_key(shift ? '#' : '3');
				keydown(KEY_3);
				break;
			case KEY_3 | KEY_KEYUP:
				keyup(KEY_3);
				break;
			case KEY_4 | KEY_KEYDOWN:
				add_key(shift ? '$' : '4');
				keydown(KEY_4);
				break;
			case KEY_4 | KEY_KEYUP:
				keyup(KEY_4);
				break;
			case KEY_5 | KEY_KEYDOWN:
				add_key(shift ? '%' : '5');
				keydown(KEY_5);
				break;
			case KEY_5 | KEY_KEYUP:
				keyup(KEY_5);
				break;
			case KEY_6 | KEY_KEYDOWN:
				add_key(shift ? '^' : '6');
				keydown(KEY_6);
				break;
			case KEY_6 | KEY_KEYUP:
				keyup(KEY_6);
				break;
			case KEY_7 | KEY_KEYDOWN:
				add_key(shift ? '&' : '7');
				keydown(KEY_7);
				break;
			case KEY_7 | KEY_KEYUP:
				keyup(KEY_7);
				break;
			case KEY_8 | KEY_KEYDOWN:
				add_key(shift ? '*' : '8');
				keydown(KEY_8);
				break;
			case KEY_8 | KEY_KEYUP:
				keyup(KEY_8);
				break;
			case KEY_9 | KEY_KEYDOWN:
				add_key(shift ? '(' : '9');
				keydown(KEY_9);
				break;
			case KEY_9 | KEY_KEYUP:
				keyup(KEY_9);
				break;
			case KEY_0 | KEY_KEYDOWN:
				add_key(shift ? ')' : '0');
				keydown(KEY_0);
				break;
			case KEY_0 | KEY_KEYUP:
				keyup(KEY_0);
				break;
			case KEY_DASH | KEY_KEYDOWN:
				add_key(shift ? '_' : '-');
				keydown(KEY_DASH);
				break;
			case KEY_DASH | KEY_KEYUP:
				keyup(KEY_DASH);
				break;
			case KEY_EQUAL | KEY_KEYDOWN:
				add_key(shift ? '+' : '=');
				keydown(KEY_EQUAL);
				break;
			case KEY_EQUAL | KEY_KEYUP:
				keyup(KEY_EQUAL);
				break;
			case KEY_TAB | KEY_KEYDOWN:
				if(!vesa_is_valid())
					add_key('\t');
				else
					// ALT + TAB是切换窗体的热键。
					// 如果ALT键被按下，TAB将不会被添加到按键列表中。
					if(!alt)
						add_key('\t');

				keydown(KEY_TAB);
				break;
			case KEY_TAB | KEY_KEYUP:
				// ALT + TAB切换当前窗体。
				if(vesa_is_valid() && alt)
					switch_window();
				
				keyup(KEY_TAB);
				break;
			case KEY_BACKSPACE | KEY_KEYDOWN:
				add_key(8);
				keydown(KEY_BACKSPACE);
				break;
			case KEY_BACKSPACE | KEY_KEYUP:
				keyup(KEY_BACKSPACE);
				break;
			case KEY_LBRACKET | KEY_KEYDOWN:
				add_key(shift ? '{' : '[');
				keydown(KEY_LBRACKET);
				break;
			case KEY_LBRACKET | KEY_KEYUP:
				keyup(KEY_LBRACKET);
				break;
			case KEY_RBRACKET | KEY_KEYDOWN:
				add_key(shift ? '}' : ']');
				keydown(KEY_RBRACKET);
				break;
			case KEY_RBRACKET | KEY_KEYUP:
				keyup(KEY_RBRACKET);
				break;
			case KEY_SEMICOLON | KEY_KEYDOWN:
				add_key(shift ? ':' : ';');
				keydown(KEY_SEMICOLON);
				break;
			case KEY_SEMICOLON | KEY_KEYUP:
				keyup(KEY_SEMICOLON);
				break;
			case KEY_RQUOTE | KEY_KEYDOWN:
				add_key(shift ? '"' : '\'');
				keydown(KEY_RQUOTE);
				break;
			case KEY_RQUOTE | KEY_KEYUP:
				keyup(KEY_RQUOTE);
				break;
			case KEY_BACKSLASH | KEY_KEYDOWN:
				add_key(shift ? '|' : '\\');
				keydown(KEY_BACKSLASH);
				break;
			case KEY_BACKSLASH | KEY_KEYUP:
				keyup(KEY_BACKSLASH);
				break;
			case KEY_COMMA | KEY_KEYDOWN:
				add_key(shift ? '<' : ',');
				keydown(KEY_COMMA);
				break;
			case KEY_COMMA | KEY_KEYUP:
				keyup(KEY_COMMA);
				break;
			case KEY_PERIOD | KEY_KEYDOWN:
				add_key(shift ? '>' : '.');
				keydown(KEY_PERIOD);
				break;
			case KEY_PERIOD | KEY_KEYUP:
				keyup(KEY_PERIOD);
				break;
			case KEY_SLASH | KEY_KEYDOWN:
				add_key(shift ? '?' : '/');
				keydown(KEY_SLASH);
				break;
			case KEY_SLASH | KEY_KEYUP:
				keyup(KEY_SLASH);
				break;
			case KEY_ENTER | KEY_KEYDOWN:
				add_key('\n');
				keydown(KEY_ENTER);
				break;
			case KEY_ENTER | KEY_KEYUP:
				keyup(KEY_ENTER);
				break;
			case KEY_SPACE | KEY_KEYDOWN:
				add_key(' ');
				keydown(KEY_SPACE);
				break;
			case KEY_SPACE | KEY_KEYUP:
				keyup(KEY_SPACE);
				break;
			case KEY_F2:
			{
				if(alt && vesa_is_valid())
					show_sfunc_window();
				break;
			}
			case KEY_F3:
			{
				if(alt && control)
				{
					int32 tid = get_wait_app_tid();
					if(tid != -1)
					{
						set_wait_app_tid(-1);
						kill_task(tid);
						clear_screen();
						enable_flush_screen();
					}
				}
				else if(alt)
					set_wait_app_tid(-1);
				break;
			}
		}
	else
	{
		add_key(KEY_EXT);
		add_key(scan_code);
		ext = FALSE;	
	}
}

/**
	@Function:		get_char
	@Access:		Public
	@Description:
		获取一个字符。
	@Parameters:
	@Return:
		uint8
			字符。		
*/
uint8
get_char(void)
{
	for(;;)
	{
		LOCK_TASK();
		if(bpos != -1)
			break;
		UNLOCK_TASK();
		PAUSE();
	}
	LOCK_TASK();
	uint8 chr = buffer[0];
	int32 i;
	for(i = 0; i <= bpos - 1; i++)
		buffer[i] = buffer[i + 1];
	bpos--;
	UNLOCK_TASK();
	return chr;
}

/**
	@Function:		get_char_utask
	@Access:		Public
	@Description:
		用户任务获取一个字符。
	@Parameters:
	@Return:
		uint8
			字符。		
*/
uint8
get_char_utask(void)
{
	if(kernel_is_knltask())
		return get_char();
	else
	{
		int32 tid = kernel_get_current_tid();
		struct Task * task = get_task_info_ptr(tid);
		if(task != NULL)
			if(task->stdin == NULL)
				return get_char();
			else
			{
				uint8 chr = 0;
				while(fread(task->stdin, &chr, 1) == 0)
					asm volatile ("pause");
				task->read_count++;
				if(flen(task->stdin) == task->read_count)
				{
					fclose(task->stdin);
					task->stdin = NULL;
				}
				return chr;
			}
	}
}

/**
	@Function:		move_cursor_left
	@Access:		Private
	@Description:
		向左移动光标。
	@Parameters:
	@Return:
		BOOL
			返回TRUE则成功，否则失败。	
*/
static
BOOL
move_cursor_left(void)
{
	LOCK_TASK();
	uint16 x, y;
	get_cursor(&x, &y);
	if(x == 0 && y == 0)
		return FALSE;
	if(x > 0)
		set_cursor(x - 1, y);
	else
		set_cursor(COLUMN - 1, y - 1);
	UNLOCK_TASK();
	return TRUE;
}

/**
	@Function:		move_cursor_right
	@Access:		Private
	@Description:
		向右移动光标。
	@Parameters:
	@Return:
		BOOL
			返回TRUE则成功，否则失败。	
*/
static
BOOL
move_cursor_right(void)
{
	LOCK_TASK();
	uint16 x, y;
	get_cursor(&x, &y);
	if(x == COLUMN - 1 && y == ROW - 1)
		return FALSE;
	if(x + 1 == COLUMN)
		set_cursor(0, y + 1);
	else
		set_cursor(x + 1, y);
	UNLOCK_TASK();
	return TRUE;
}

/**
	@Function:		get_strn
	@Access:		Public
	@Description:
		获取一个字符串。
	@Parameters:
		input_buffer, int8 *, OUT
			指向用于储存输入的缓冲区的指针。
		n, uint32, OUT
			最大获取字符个数。
	@Return:
		uint32
			字符个数。不包括结尾的'\0'。	
*/
uint32
get_strn(	OUT int8 * input_buffer,
			IN uint32 n)
{
	LOCK_TASK();
	uint32 count = 0;
	uint8 chr;
	int32 pos = -1;
	uint16 startx, starty;
	get_cursor(&startx, &starty);
	input_buffer[0] = '\0';
	UNLOCK_TASK();
	for(;;)
	{
		chr = get_char();
		LOCK_TASK();
		if(chr == '\n' || count >= n)
		{
			int32 i;
			set_cursor(startx, starty);
			for(i = 0; i < count; i++)
				print_char(input_buffer[i]);
			print_char('\n');
			break;
		}
		switch(chr)
		{
			//Backspace
			case 8:
				if(count > 0 && pos != -1)
				{
					uint16 x, y;
					get_cursor(&x, &y);
					if(x != 0 || y != 0)
					{
						int32 i;
						lock_cursor();					
						set_cursor(startx, starty);
						for(i = 0; i < count; i++)
							print_char(' ');
						for(i = pos; i < count - 1; i++)
							input_buffer[i] = input_buffer[i + 1];
						set_cursor(startx, starty);
						count--;
						pos--;
						for(i = 0; i < count; i++)
							print_char(input_buffer[i]);
						unlock_cursor();
						if(x == 0)
							set_cursor(COLUMN - 1, y - 1);
						else
							set_cursor(x - 1, y);
					}				
				}
				break;
			case KEY_EXT:
			{
				switch(get_char())
				{
					//Left
					case 75:
						if(pos != -1 && move_cursor_left())
							pos--;
						break;
					//Right
					case 77:
						if(pos + 1 < count && move_cursor_right())
							pos++;
						break;
					//Up
					case 72:
						break;
					//Down
					case 80:
						break;
				}
				break;
			}
			default:
			{
				//向输入缓冲区插入输入的字符
				int32 i;
				for(i = count - 1; i >= pos && i != -1; i--)
					input_buffer[i + 1] = input_buffer[i];
				input_buffer[++pos] = chr;
				count++;

				//刷新屏幕输入的内容
				BOOL is_screen_up = FALSE;
				BOOL is_screen_up1 = FALSE;
				uint16 x, y;
				get_cursor(&x, &y);
				if(pos + 1 != count && (startx + count) % COLUMN == 0 && starty * COLUMN + startx + count == ROW * COLUMN)
				{
					starty--;
					screen_up();
					is_screen_up1 = TRUE;
				}
				else if(pos + 1 == count && x == COLUMN - 1 && y == ROW - 1)
				{
					starty--;
					screen_up();
					is_screen_up = TRUE;
				}
				lock_cursor();
				set_cursor(startx, starty);
				for(i = 0; i < count; i++)
					print_char(input_buffer[i]);
				unlock_cursor();
				if(x + 1 == COLUMN)
					if(is_screen_up)
						set_cursor(0, y);
					else
						set_cursor(0, y + 1);
				else
					if(is_screen_up1)
						set_cursor(x + 1, y - 1);
					else
						set_cursor(x + 1, y);
				break;
			}		
		}
		UNLOCK_TASK();
	}
	LOCK_TASK();
	input_buffer[count] = '\0';
	UNLOCK_TASK();
	return count;
}

/**
	@Function:		get_strn_utask
	@Access:		Public
	@Description:
		用户任务获取一个字符串。
	@Parameters:
		input_buffer, int8 *, OUT
			指向用于储存输入的缓冲区的指针。
		n, uint32, OUT
			最大获取字符个数。
	@Return:
		uint32
			字符个数。不包括结尾的'\0'。	
*/
uint32
get_strn_utask(	OUT int8 * input_buffer,
				IN uint32 n)
{
	if(kernel_is_knltask())
		return get_strn(input_buffer, n);
	else
	{
		int32 tid = kernel_get_current_tid();
		struct Task * task = get_task_info_ptr(tid);
		if(task != NULL)
			if(task->stdin == NULL)
				return get_strn(input_buffer, n);
			else
			{
				uint32 read_count = 0;
				while(read_count < n && !feof(task->stdin))
					read_count += fread(task->stdin,
										input_buffer,
										n - read_count);
				fclose(task->stdin);
				task->stdin = NULL;
				input_buffer[read_count] = '\0';
				return read_count;
			}
	}
}

/**
	@Function:		get_shift
	@Access:		Public
	@Description:
		获取 Shift 键的状态。
	@Parameters:
	@Return:
		BOOL
			返回TRUE则被按下，否则没被按下。	
*/
BOOL
get_shift(void)
{
	return shift;
}

/**
	@Function:		get_control
	@Access:		Public
	@Description:
		获取 Control 键的状态。
	@Parameters:
	@Return:
		BOOL
			返回TRUE则被按下，否则没被按下。	
*/
BOOL
get_control(void)
{
	return control;
}

/**
	@Function:		get_alt
	@Access:		Public
	@Description:
		获取 Alt 键的状态。
	@Parameters:
	@Return:
		BOOL
			返回TRUE则被按下，否则没被按下。	
*/
BOOL
get_alt(void)
{
	return alt;
}

/**
	@Function:		init_keyboard_driver
	@Access:		Public
	@Description:
		初始化键盘驱动。
	@Parameters:
	@Return:	
*/
void
init_keyboard_driver(void)
{
	uint32 ui;

	// 重置PS/2
	uint8 tmp = inb(0x61);
	outb(0x61, tmp | 0x80);
	outb(0x61, tmp & 0x7f);
	inb(0x60);

	for(ui = 0; ui < KEY_MAP_SIZE; ui++)
		key_map[ui] = 0;
}

/**
	@Function:		get_key_status
	@Access:		Public
	@Description:
		获取按键状态。
	@Parameters:
		key, int32, IN
			按键。
	@Return:
		int32
			返回0则没被按下，返回1则被按下。
*/
int32
get_key_status(IN int32 key)
{
	if(key < 0 || key >= KEY_MAP_SIZE)
		return 0;
	return key_map[key];	
}

/**
	@Function:		has_key
	@Access:		Public/Private
	@Description:
		检查键盘输入队列是否包含按键。
	@Parameters:
	@Return:
		BOOL
			返回TRUE则有，否则没有。	
*/
BOOL
has_key(void)
{
	if(bpos != -1)
		return TRUE;
	else
		return FALSE;
}
