#include <ilib/ilib.h>

#define	_TITLE		"Editor"

#define	MAX_COLUMN				1024			// 列数的最大值。
#define	MAX_ROW					2048			// 行数的最大值。

#define	MAX_LNUM_DIGIT			5				// 行号的最大字符数量。

#define	EDITAREA_X (cursor_x - MAX_LNUM_DIGIT)	// 光标相对内容编辑区的X坐标。
#define	EDITAREA_Y (cursor_y)					// 光标相对内容编辑区的Y坐标。

#define	SBUFFER_CHR(n)	(n * 2)					// 屏幕缓冲区中第n个字符的偏移值。
#define	SBUFFER_PRO(n)	(n * 2 + 1)				// 屏幕缓冲区中第n个字符的属性的偏移值。

#define	_WINDOW_WIDTH	(scr_column * ENFONT_WIDTH)
#define	_WINDOW_HEIGHT	(scr_row * (ENFONT_HEIGHT + CURSOR_HEIGHT))

static uint32 column, row;						// 文本编辑区的列和行。
static uint32 scr_column, scr_row;				// 屏幕的列和行。
static uint16 cursor_x, cursor_y;				// 光标的坐标。
static ASCCHAR * content_buffer[MAX_ROW];		// 内容缓冲区。
static uint8 * screen_buffer = NULL;			// 屏幕缓冲区。
static uint32 top_column, top_row;				// 屏幕缓冲区左上角相对于内容缓冲区的偏移量。
static ASCCHAR filepath[1024];					// 文件路径。
static uint32 total_line;						// 总行数。
static BOOL is_changed = FALSE;					// 是否已编辑。

static int32 _wid = -1;

static void release_resource(void);
static void die(ASCCHAR * message);
static void init(void);
static int get_line_len(uint32 line);
static void edit(void);
static void strcpy_sbuffer(ASCCHAR * dest, CASCCHAR * src);
static void flush(void);
static void load(void);
static void save(void);

int32 main(int32 argc, ASCCHAR * argv[])
{
	if(argc != 2)
	{
		printf("Format:\n\tedit {file}\n");
		return -1;
	}
	strcpy(filepath, argv[1]);
	ILFixPath(filepath, filepath);
	init();
	edit();
	return 0;
}

static void release_resource(void)
{
	if(screen_buffer != NULL)
		free(screen_buffer);
	uint32 ui;
	for(ui = 0; ui < MAX_ROW; ui++)
		if(content_buffer[ui] != NULL)
			free(content_buffer[ui]);
}

static void die(ASCCHAR * message)
{
	print_str_p(message, CC_RED);
	print_str("\n");
	release_resource();
	exit(-1);
}

static void init(void)
{
	uint32 ui, ui1;
	for(ui = 0; ui < MAX_ROW; ui++)
		content_buffer[ui] = NULL;
	for(ui = 0; ui < MAX_ROW; ui++)
	{
		content_buffer[ui] = malloc(MAX_COLUMN);
		if(content_buffer[ui] == NULL)
			die("Cannot allocate memory!");
		for(ui1 = 0; ui1 < MAX_COLUMN; ui1++)
			content_buffer[ui][ui1] = '\0';
	}
	load();
	get_text_screen_size(&scr_column, &scr_row);

	// 创建窗体。
	_wid = ILGCreateWindow(	_WINDOW_WIDTH,
							_WINDOW_HEIGHT,
							0xffffffff,
							WINDOW_STYLE_CLOSE | WINDOW_STYLE_MINIMIZE,
							_TITLE);
	if(_wid == -1)
		die(_TITLE" cannot create a new window!");
	ILGDrawRect(_wid, 0, 0, _WINDOW_WIDTH, _WINDOW_HEIGHT, 0xffffffff);
	ILGSetWindowState(_wid, WINDOW_STATE_SHOW);
	ILGUpdate(_wid);
	ILGFocus(_wid);
	ILRunInBackground();

	row = scr_row - 1;
	column = scr_column - MAX_LNUM_DIGIT;
	screen_buffer = (uint8 *)malloc(scr_row * scr_column * 2);
	if(screen_buffer == NULL)
		die("Cannot allocate memory!");
	cursor_x = MAX_LNUM_DIGIT;
	cursor_y = 0;
	top_row = 0;
	top_column = 0;
}

static int get_line_len(uint32 line)
{
	int32 len = 0;
	for(len = 0; len < MAX_COLUMN && content_buffer[line][len] != '\0'; len++);
	return len;
}

static void edit(void)
{
	flush();
	while(1)
	{
		uint8 chr = ILGGetKeyWait(_wid);
		uint8 chr1;
		if(chr == KEY_EXT)
			switch(chr1 = ILGGetKeyWait(_wid))
			{
				case KEY_UP:
					if(EDITAREA_Y != 0)
						cursor_y--;
					else if(top_row != 0)
						top_row--;
					break;
				case KEY_DOWN:
					if(top_row + EDITAREA_Y + 1 < total_line && EDITAREA_Y + 1 < row)
						cursor_y++;
					else if(top_row + row < total_line && top_row + row < MAX_ROW)
						top_row++;
					break;
				case KEY_LEFT:
					if(EDITAREA_X != 0)
						cursor_x--;
					else if(top_column != 0)
						top_column--;
					break;
				case KEY_RIGHT:
					if(EDITAREA_X + 1 < column)
						cursor_x++;
					else if(top_column + column < MAX_COLUMN)
						top_column++;
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
					if((top_column + EDITAREA_X) != 0)
					{
						uint32 ui;
						for(ui = top_column + EDITAREA_X; ui < MAX_COLUMN; ui++)
							content_buffer[top_row + EDITAREA_Y][ui - 1] = content_buffer[top_row + EDITAREA_Y][ui];
						content_buffer[top_row + EDITAREA_Y][MAX_COLUMN - 1] = '\0';
						if(EDITAREA_X != 0)
							cursor_x--;
						else
							top_column--;
						is_changed = TRUE;
					}
					else if(top_row + EDITAREA_Y != 0 
							&& (prev_line_len = get_line_len(top_row + EDITAREA_Y - 1)) 
													+ (curr_line_len = get_line_len(top_row + EDITAREA_Y)) 
									< MAX_COLUMN)
					{
						memcpy(content_buffer[top_row + EDITAREA_Y - 1] + prev_line_len, content_buffer[top_row + EDITAREA_Y], curr_line_len);
						int32 i;
						for(i = top_row + EDITAREA_Y; i < total_line - 1; i++)
							memcpy(content_buffer[i], content_buffer[i + 1], MAX_COLUMN);
						if(EDITAREA_Y != 0)
							cursor_y--;
						else
							top_row--;
						for(i = 0; i < MAX_COLUMN; i++)
							content_buffer[total_line - 1][i] = '\0';
						total_line--;
						cursor_x = MAX_LNUM_DIGIT + prev_line_len;
						is_changed = TRUE;
					}
					break;
				}
				case '\n':
					if(total_line < MAX_ROW)
					{
						total_line++;
						int32 i, i1;
						if(total_line == 0)
							for(i = 0; i < MAX_COLUMN; i++)
								content_buffer[top_row + EDITAREA_Y + 1][i] = content_buffer[top_row + EDITAREA_Y][i];
						else
							for(i = total_line; i > top_row + EDITAREA_Y + 1; i--)
								for(i1 = 0; i1 < MAX_COLUMN; i1++)
									content_buffer[i][i1] = content_buffer[i - 1][i1];
						for(i = 0; i < MAX_COLUMN; i++)
							content_buffer[top_row + EDITAREA_Y + 1][i] = '\0';
						for(i = top_column + EDITAREA_X, i1 = 0; i < MAX_COLUMN; i++, i1++)
						{
							content_buffer[top_row + EDITAREA_Y + 1][i1] = content_buffer[top_row + EDITAREA_Y][i]; 
							content_buffer[top_row + EDITAREA_Y][i] = '\0';
						}
						cursor_x = MAX_LNUM_DIGIT;
						if(EDITAREA_Y + 1 >= row)
							top_row++;
						else
							cursor_y++;
						is_changed = TRUE;
					}
					break;
				case '\t':
					break;
				default:
					if(get_control() && chr == 's')
					{
						is_changed = FALSE;
						save();
					}
					else if(chr == 27)
					{
						release_resource();
						exit(0);
						break;
					}
					else if(top_column + EDITAREA_X + 1 < MAX_COLUMN)
					{
						int32 i;
						for(i = MAX_COLUMN - 1; i > top_column + EDITAREA_X; i--)
							content_buffer[top_row + EDITAREA_Y][i] = content_buffer[top_row + EDITAREA_Y][i - 1];
						content_buffer[top_row + EDITAREA_Y][top_column + EDITAREA_X] = chr;
						for(i = 0; i < top_column + EDITAREA_X; i++)
							if(content_buffer[top_row + EDITAREA_Y][i] == 0)
								content_buffer[top_row + EDITAREA_Y][i] = ' ';
						if(EDITAREA_X + 1 < column)
							cursor_x++;
						else if(top_column + column < MAX_COLUMN)
							top_column++;
						if(top_column + EDITAREA_Y == total_line)
							total_line++;
						is_changed = TRUE;
					}
					break;
			}
		flush();
	}
}

static void strcpy_sbuffer(ASCCHAR * dest, CASCCHAR * src)
{
	while(*src != '\0')
	{
		*dest = *src;
		dest += 2;
		src++;
	}
}

static void render(void)
{
	ILGRenderTextBuffer(_wid, screen_buffer, scr_row, scr_column, cursor_x, cursor_y);
}

static void flush(void)
{
	uint32 ui, ui1;
	for(ui = top_row; ui < top_row + row; ui++)
	{
		// 输出行号。
		uint32 ln = 1 + ui;
		uint32 lnoff = (ui - top_row) * scr_column * 2;
		screen_buffer[lnoff + 0] = ' ';
		screen_buffer[lnoff + 1] = CC_GRAYWHITE | CBGC_BLUE;
		screen_buffer[lnoff + 2] = ' ';
		screen_buffer[lnoff + 3] = CC_GRAYWHITE | CBGC_BLUE;
		screen_buffer[lnoff + 4] = ' ';
		screen_buffer[lnoff + 5] = CC_GRAYWHITE | CBGC_BLUE;
		screen_buffer[lnoff + 6] = ' ';
		screen_buffer[lnoff + 7] = CC_GRAYWHITE | CBGC_BLUE;
		screen_buffer[lnoff + 8] = ' ';
		screen_buffer[lnoff + 9] = CC_GRAYWHITE | CBGC_BLUE;
		int8 ln_s[20];
		itos(ln_s, ln);
		if(strlen(ln_s) > MAX_LNUM_DIGIT)
		{
			screen_buffer[lnoff + 0] = 'X';
			screen_buffer[lnoff + 2] = 'X';
			screen_buffer[lnoff + 4] = 'X';
			screen_buffer[lnoff + 6] = 'X';
			screen_buffer[lnoff + 8] = 'X';
		}
		else
			for(ui1 = 0; ui1 < MAX_LNUM_DIGIT && ln_s[ui1] != '\0'; ui1++)
				screen_buffer[lnoff + ui1 * 2] = ln_s[ui1];

		// 输出内容到内容编辑区。
		ASCCHAR * line = content_buffer[ui];
		for(ui1 = 0; ui1 < column; ui1++)
		{
			uint32 offset = ((ui - top_row) * scr_column + (MAX_LNUM_DIGIT + ui1)) * 2;
			screen_buffer[offset + 0] 
				= ((line[top_column + ui1] == '\0' || line[top_column + ui1] == '\t') 
					? ' ' 
					: line[top_column + ui1]);
			screen_buffer[offset + 1] = CC_GRAYWHITE;
		}
	}

	// 输出状态栏。
	uint32 sbaroff = row * scr_column * 2;
	for(ui = 0; ui < scr_column; ui++)
	{
		screen_buffer[sbaroff + SBUFFER_CHR(ui)] = ' ';
		screen_buffer[sbaroff + SBUFFER_PRO(ui)] = CBGC_GRAYWHITE | CC_YELLOW;
	}
	if(is_changed)
		screen_buffer[sbaroff + SBUFFER_CHR(0)] = '*';
	screen_buffer[sbaroff + SBUFFER_CHR(1)] = '|';
	uint32 pos = 2;
	ASCCHAR temp[100];
	uitos(temp, 1 + top_column + EDITAREA_X);
	strcpy_sbuffer(&screen_buffer[sbaroff + SBUFFER_CHR(pos)], temp);
	pos += strlen(temp);
	screen_buffer[sbaroff + SBUFFER_CHR(pos)] = ',';
	pos++;
	uitos(temp, 1 + top_row + EDITAREA_Y);
	strcpy_sbuffer(&screen_buffer[sbaroff + SBUFFER_CHR(pos)], temp);
	pos += strlen(temp);
	screen_buffer[sbaroff + SBUFFER_CHR(pos)] = '|';
	pos++;
	uint32 len = strlen(filepath);
	if(len <= 40)
		strcpy_sbuffer(&screen_buffer[sbaroff + SBUFFER_CHR(pos)], filepath);
	else
	{
		memcpy(temp, filepath, 40);
		temp[40] = '\0';
		strcpy_sbuffer(&screen_buffer[sbaroff + SBUFFER_CHR(pos)], temp);
	}
	pos += len;
	render();
}

static void load(void)
{
	uint file_len;
	char * buffer = malloc(MAX_ROW * MAX_COLUMN);
	if(buffer == NULL)
		die("Cannot alloc memory!");
	ILFILE * fptr = ILOpenFile(filepath, FILE_MODE_READ);
	if(fptr == NULL)
	{
		free(buffer);
		die("Cannot open file!");
	}
	file_len = ILGetFileLength(fptr);
	ILReadFile(fptr, buffer, file_len);
	ILCloseFile(fptr);
	uint ui;
	uint line = 0;
	uint index = 0;
	for(ui = 0; ui < file_len; ui++)
	{
		char chr = buffer[ui];
		if(chr == '\n')
		{
			line++;
			index = 0;
		}
		else
			content_buffer[line][index++] = chr;
	}
	total_line = line;
	if(index != 0)
		total_line++;
	free(buffer);
}

static void save(void)
{
	ILFILE * fptr = ILOpenFile(filepath, FILE_MODE_WRITE | FILE_MODE_APPEND);
	if(fptr == NULL)
		die("Cannot open file!");
	ILWriteFile(fptr, "", 0);
	uint line;
	for(line = 0; line < total_line; line++)
	{
		uint len;
		for(len = 0; len < MAX_COLUMN && content_buffer[line][len] != '\0'; len++);
		ILAppendFile(fptr, content_buffer[line], len);
		ILAppendFile(fptr, "\n", 1);
	}
	ILCloseFile(fptr);
}
