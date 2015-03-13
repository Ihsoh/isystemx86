#include <ilib.h>

#define	MAX_COLUMN				1024
#define	MAX_ROW					2048

static uint column, row;
static ushort cursor_x, cursor_y;
static char content_buffer[MAX_ROW][MAX_COLUMN];
static char * screen_buffer;
static uint top_column, top_row;
static char filepath[1024];
static uint total_line;

void die(char * message);
void init(void);
void edit(void);
void flush(void);
void load(void);
void save(void);

int main(int argc, char * argv[])
{
	if(argc != 2)
	{
		printf("Format:\n\tedit {file}\n");
		return -1;
	}
	strcpy(filepath, argv[1]);
	fix_path(filepath, filepath);
	init();
	edit();
	return 0;
}

void die(char * message)
{
	clear_screen();
	print_str_p(message, CC_RED);
	print_str("\n");
	if(screen_buffer != NULL)
		freem(screen_buffer);
	app_exit();
}

void init(void)
{
	uint ui, ui1;
	for(ui = 0; ui < MAX_ROW; ui++)
		for(ui1 = 0; ui1 < MAX_COLUMN; ui1++)
			content_buffer[ui][ui1] = '\0';
	load();
	set_clock(0);
	get_text_screen_size(&column, &row);
	row--;
	screen_buffer = (char *)allocm(row * column + 1);
	if(screen_buffer == NULL)
		die("Cannot alloc memory!");
	cursor_x = 0;
	cursor_y = 0;
	top_row = 0;
	top_column = 0;
	clear_screen();
	set_cursor(cursor_x, cursor_y);
}

static int get_line_len(uint line)
{
	int len = 0;
	for(len = 0; len < MAX_COLUMN && content_buffer[line][len] != '\0'; len++);
	return len;
}

void edit(void)
{
	flush();
	while(1)
	{
		uchar chr = get_char();
		char chr1;
		if(chr == KEY_EXT)
			switch(chr1 = get_char())
			{
				case KEY_UP:
					if(cursor_y != 0)
						cursor_y--;
					else if(top_row != 0)
						top_row--;
					break;
				case KEY_DOWN:
					if(top_row + cursor_y + 1 < total_line && cursor_y + 1 < row)
						cursor_y++;
					else if(top_row + row < total_line && top_row + row < MAX_ROW)
						top_row++;
					break;
				case KEY_LEFT:
					if(cursor_x != 0)
						cursor_x--;
					else if(top_column != 0)
						top_column--;
					break;
				case KEY_RIGHT:
					if(cursor_x + 1 < column)
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
					uint prev_line_len, curr_line_len;
					if((top_column + cursor_x) != 0)
					{
						uint ui;
						for(ui = top_column + cursor_x; ui < MAX_COLUMN; ui++)
							content_buffer[top_row + cursor_y][ui - 1] = content_buffer[top_row + cursor_y][ui];
						content_buffer[top_row + cursor_y][MAX_COLUMN - 1] = '\0';
						if(cursor_x != 0)
							cursor_x--;
						else
							top_column--;
					}
					else if(top_row + cursor_y != 0 
							&& (prev_line_len = get_line_len(top_row + cursor_y - 1)) + (curr_line_len = get_line_len(top_row + cursor_y)) < MAX_COLUMN)
					{
						memcpy(content_buffer[top_row + cursor_y - 1] + prev_line_len, content_buffer[top_row + cursor_y], curr_line_len);
						int i;
						for(i = top_row + cursor_y; i < total_line - 1; i++)
							memcpy(content_buffer[i], content_buffer[i + 1], MAX_COLUMN);
						if(cursor_y != 0)
							cursor_y--;
						else
							top_row--;
						for(i = 0; i < MAX_COLUMN; i++)
							content_buffer[total_line - 1][i] = '\0';
						total_line--;
						cursor_x = prev_line_len;
					}
					break;
				}
				case '\n':
					if(total_line < MAX_ROW)
					{
						total_line++;
						int i, i1;
						if(total_line == 0)
							for(i = 0; i < MAX_COLUMN; i++)
								content_buffer[top_row + cursor_y + 1][i] = content_buffer[top_row + cursor_y][i];
						else
							for(i = total_line; i > top_row + cursor_y + 1; i--)
								for(i1 = 0; i1 < MAX_COLUMN; i1++)
									content_buffer[i][i1] = content_buffer[i - 1][i1];
						for(i = 0; i < MAX_COLUMN; i++)
							content_buffer[top_row + cursor_y + 1][i] = '\0';
						for(i = top_column + cursor_x, i1 = 0; i < MAX_COLUMN; i++, i1++)
						{
							content_buffer[top_row + cursor_y + 1][i1] = content_buffer[top_row + cursor_y][i]; 
							content_buffer[top_row + cursor_y][i] = '\0';
						}
						cursor_x = 0;
						if(cursor_y + 1 >= row)
							top_row++;
						else
							cursor_y++;
					}
					break;
				case '\t':
					break;
				default:
					if(get_control() && chr == 's')
						save();
					else if(chr == 27)
					{
						clear_screen();
						app_exit();
					}
					else if(top_column + cursor_x + 1 < MAX_COLUMN)
					{
						int i;
						for(i = MAX_COLUMN - 1; i > top_column + cursor_x; i--)
							content_buffer[top_row + cursor_y][i] = content_buffer[top_row + cursor_y][i - 1];
						content_buffer[top_row + cursor_y][top_column + cursor_x] = chr;
						for(i = 0; i < top_column + cursor_x; i++)
							if(content_buffer[top_row + cursor_y][i] == 0)
								content_buffer[top_row + cursor_y][i] = ' ';
						if(cursor_x + 1 < column)
							cursor_x++;
						else if(top_column + column < MAX_COLUMN)
							top_column++;
						if(top_column + cursor_y == total_line)
							total_line++;
					}
					break;
			}
		flush();
	}
}

void flush(void)
{
	lock_cursor();
	uint ui, ui1;
	for(ui = top_row; ui < top_row + row; ui++)
	{
		char * line = content_buffer[ui];
		for(ui1 = 0; ui1 < column; ui1++)
			screen_buffer[(ui - top_row) * column + ui1] 
				= ((line[top_column + ui1] == '\0' || line[top_column + ui1] == '\t') 
					? ' ' 
					: line[top_column + ui1]);
	}
	screen_buffer[row * column] = '\0';
	if(vesa_is_valid())
		clear_screen();
	else
		set_cursor(0, 0);
	
	for(ui = 0; ui < row; ui++)
	{
		char temp[8192];
		if(column >= sizeof(temp))
		{
			memcpy(temp, screen_buffer + ui * column, sizeof(temp) - 1);
			temp[sizeof(temp) - 1] = '\0';
		}
		else
		{
			memcpy(temp, screen_buffer + ui * column, column);
			temp[column] = '\0';
		}
		if(cursor_y == ui)
			print_str_p(temp, CC_GRAYWHITE | CBGC_BROWN);
		else
			print_str(temp);
	}

	char buffer[100];
	print_str("                                                                      ");
	set_cursor(0, row);
	print_str_p(uitos(buffer, top_column + cursor_x), CC_YELLOW | CBGC_GRAYWHITE);
	print_str_p(",", CC_YELLOW | CBGC_GRAYWHITE);
	print_str_p(uitos(buffer, top_row + cursor_y), CC_YELLOW | CBGC_GRAYWHITE);
	print_str_p("|L:", CC_YELLOW | CBGC_GRAYWHITE);
	print_str_p(uitos(buffer, total_line), CC_YELLOW | CBGC_GRAYWHITE);
	unlock_cursor();
	set_cursor(cursor_x, cursor_y);
}

void load(void)
{
	uint file_len;
	char * buffer = allocm(MAX_ROW * MAX_COLUMN);
	if(buffer == NULL)
		die("Cannot alloc memory!");
	FILE * fptr = fopen(filepath, FILE_MODE_READ);
	if(fptr == NULL)
	{
		freem(buffer);
		die("Cannot open file!");
	}
	file_len = flen(fptr);
	fread(fptr, buffer, file_len);
	fclose(fptr);
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
	freem(buffer);
}

void save(void)
{
	clear_screen();
	print_str("Saving file");

	FILE * fptr = fopen(filepath, FILE_MODE_WRITE | FILE_MODE_APPEND);
	if(fptr == NULL)
		die("Cannot open file!");
	fwrite(fptr, "", 0);
	uint line;
	for(line = 0; line < total_line; line++)
	{
		uint len;
		for(len = 0; len < MAX_COLUMN && content_buffer[line][len] != '\0'; len++);
		fappend(fptr, content_buffer[line], len);
		fappend(fptr, "\n", 1);
		if(total_line / 10 > 0 && line % (total_line / 10) == 0)
			print_str(".");
	}
	fclose(fptr);
}
