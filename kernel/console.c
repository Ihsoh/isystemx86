/**
	@File:			console.c
	@Author:		Ihsoh
	@Date:			2014-2-1
	@Description:
		控制台程序。
*/

#include "console.h"
#include "types.h"
#include "keyboard.h"
#include "screen.h"
#include "memory.h"
#include "kfuncs.h"
#include "cmos.h"
#include "ifs1disk.h"
#include "ifs1fs.h"
#include "ifs1blocks.h"
#include "tasks.h"
#include "system.h"
#include "vars.h"
#include "cpu.h"
#include "kstring.h"
#include "log.h"

#include <string.h>

#include "386.h"
#include "vesa.h"
#include "die.h"

#include "mqueue.h"

DEFINE_LOCK_IMPL(console)

#define	FORMAT(message)	"Foramt:\n\t"message

static int32 enable_clock = 1;
static int8 current_path[MAX_PATH_BUFFER_LEN] = "";
static int32 wait_app_tid = -1;
static struct Vars * global_vars_s = NULL;

#define	parse_cmd(cmd, word, n) (_parse_cmd(cmd, word, n, 1, local_vars_s))
#define	parse_cmd_nv(cmd, word, n) (_parse_cmd(cmd, word, n, 0, NULL))

/**
	@Function:		_parse_cmd
	@Access:		Private
	@Description:
		解析命令。依次获取命令中得每一部分。
	@Parameters:
		cmd, int8 *, IN
			如果该参数不为NULL，则设置将要进行解析的命令。如果该参数为NULL则获取刚才设置的命令的下一部分。
		word, int8 *, OUT
			当需要获取命令中的一部分时，该参数指向用于储存的缓冲区。
		n, uint32, IN
			指定储存缓冲区最多可以容纳的字节数。
		process_var, int32, IN
			是否处理命令内的变量。值为0时不处理，否则处理。
		local_vars_s, struct Vars *, IN
			当需要处理命令内变量时，需要提供一个指向本地变量列表的指针。
	@Return:
		int32
			获取的部分的长度。		
*/
static
int32
_parse_cmd(	IN int8 * cmd,
			OUT int8 * word,
			IN uint32 n,
			IN int32 process_var,
			IN struct Vars * local_vars_s)
{
	static uint32 len = 0;
	static uint8 buffer[1024];
	static uint32 pos = 0;
	if(cmd != NULL)
	{
		strcpy(buffer, cmd);
		len = strlen(buffer);
		pos = 0;
	}
	else
	{
		uint32 ui = 0;
		int32 state = 0;
		for(; pos < len && ui < n; pos++)
		{
			int8 chr = buffer[pos];
			//普通
			if(state == 0 && chr != ' ' && chr != '\t' && chr != '"' && chr != '\'')
			{
				word[ui++] = chr;
				state = 1;
			}
			else if(state == 1 && chr != ' ' && chr != '\t' && chr != '"')
				word[ui++] = chr;
			else if(state == 1 && (chr == ' ' || chr == '\t' || chr == '"'))
				break;
			//双引号字符串
			else if(state == 0 && chr == '"')
				state = 2;
			else if(state == 2 && chr == '\\' && pos + 1 < len && (buffer[pos + 1] == '"' || buffer[pos + 1] == '\\'))
				word[ui++] = buffer[++pos];
			else if(state == 2 && chr == '\\' && pos + 1 < len && buffer[pos + 1] == 'r')
			{
				word[ui++] = '\r';
				pos++;			
			}	
			else if(state == 2 && chr == '\\' && pos + 1 < len && buffer[pos + 1] == 'n')
			{
				word[ui++] = '\n';
				pos++;			
			}
			else if(state == 2 && chr == '\\' && pos + 1 < len && buffer[pos + 1] == 't')
			{
				word[ui++] = '\t';
				pos++;			
			}		
			else if(state == 2 && chr == '\\' && pos + 1 < len && buffer[pos + 1] != '"' && buffer[pos + 1] != '\\')
				word[ui++] = '\\';
			else if(state == 2 && chr != '\\' && chr != '"')
				word[ui++] = chr;
			else if(state == 2 && chr == '"')
			{
				pos++;
				break;
			}
			//单引号字符串
			else if(state == 0 && chr == '\'')
				state = 3;
			else if(state == 3 && chr == '\\' && pos + 1 < len && (buffer[pos + 1] == '\'' || buffer[pos + 1] == '\\'))
				word[ui++] = buffer[++pos];
			else if(state == 3 && chr == '\\' && pos + 1 < len && buffer[pos + 1] == 'r')
			{
				word[ui++] = '\r';
				pos++;			
			}	
			else if(state == 3 && chr == '\\' && pos + 1 < len && buffer[pos + 1] == 'n')
			{
				word[ui++] = '\n';
				pos++;			
			}
			else if(state == 3 && chr == '\\' && pos + 1 < len && buffer[pos + 1] == 't')
			{
				word[ui++] = '\t';
				pos++;			
			}
			else if(state == 3 && chr == '\\' && pos + 1 < len && buffer[pos + 1] != '\'' && buffer[pos + 1] != '\\')
				word[ui++] = '\\';
			else if(state == 3 && chr != '\\' && chr != '\'')
				word[ui++] = chr;
			else if(state == 3 && chr == '\'')
			{
				pos++;
				break;
			}
		}
		word[ui] = '\0';
		if(process_var && len >= 2)
			if(word[0] == '$')
				get_var_value(local_vars_s, word + 1, word);
			else if(word[0] == '@')
				get_var_value(global_vars_s, word + 1, word);
	}
	return len - pos;
}

/**
	@Function:		ver
	@Access:		Private
	@Description:
		打印版本信息。
	@Parameters:
	@Return:	
*/
static
void
ver(void)
{
	print_str("ISystem II(x86) Console\nIhsoh Software 2014\n\n");
}

/**
	@Function:		print
	@Access:		Private
	@Description:
		打印文本。打印完毕后不会进行换行。
	@Parameters:
		text, int8 *, IN
			需要打印的文本。
	@Return:	
*/
static
void
print(IN int8 * text)
{
	print_str(text);
}

/**
	@Function:		error
	@Access:		Private
	@Description:
		打印错误信息。打印完毕后会进行换行。
	@Parameters:
		text, int8 *, IN
			错误信息。
	@Return:	
*/
static
void
error(IN int8 * text)
{
	print_str_p(text, CC_RED);
	print_str("\n");
}

/**
	@Function:		disks
	@Access:		Private
	@Description:
		打印所有磁盘的信息。
	@Parameters:
	@Return:
		int32
			返回0则失败，否则成功。	
*/
static
int32
disks(void)
{
	uint32 disk_count = get_disk_count();
	uint32 ui;
	print_str_p("Name    System  Size                    \n", 
				CC_YELLOW | CBGC_GRAYWHITE);
	for(ui = 0; ui < disk_count; ui++)
	{
		int8 symbol[3];
		get_disk_symbol(ui, symbol);
		print_str(symbol);
		print_str("\t\t");
		if(is_system_disk(symbol))
			print_str("Yes\t\t");
		else
			print_str("No\t\t");
		uint32 disk_size = get_disk_size(symbol);
		printun(disk_size);
		print_str("KB, ");
		printun(disk_size / 1024);
		print_str("MB\n");
	}
	return 1;
}

/**
	@Function:		mm
	@Access:		Private
	@Description:
		打印内存相关信息。
	@Parameters:
	@Return:
		int32
			返回0则失败，否则成功。	
*/
static
int32
mm(void)
{
	uint64 max_memory_byte = get_memory_size();
	uint32 max_memory_mbyte = (uint32)max_memory_byte == 0 ? 4096 : (uint32)max_memory_byte / 1024 / 1024;
	print_str("Max Memory:  ");
	printun(max_memory_mbyte);
	print_str("MB\n");
	uint64 used_memory_byte = get_used_memory_length();
	uint32 used_memory_mbyte = (uint32)used_memory_byte == 0 ? 4096 : (uint32)used_memory_byte / 1024 / 1024;
	print_str("Used Memory: ");
	printun(used_memory_mbyte);
	print_str("MB\n");
	uint32 t, c, m;
	print_str("MBDT:        ");
	get_mbdt_info(&t, &c, &m);
	printn(c);
	print_str("/");
	printn(t);
	print_str(", ");
	printn(m / 1024 / 1024);
	print_str("MB\n");
	print_str("UMBDT:       ");
	get_umbdt_info(&t, &c, &m);
	printn(c);
	print_str("/");
	printn(t);
	print_str(", ");
	printn(m / 1024 / 1024);
	print_str("MB\n");
	return 1;
}

/**
	@Function:		date
	@Access:		Private
	@Description:
		打印日期。
	@Parameters:
	@Return:
		int32
			返回0则失败，否则成功。	
*/
static
int32
date(void)
{
	int8 weeks[][10] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
	struct CMOSDateTime dt;
	get_cmos_date_time(&dt);
	printn(dt.year);
	print_str("-");
	printn(dt.month);
	print_str("-");
	printn(dt.day);
	print_str(" ");
	print_str(weeks[dt.day_of_week]);
	return 1;
}

/**
	@Function:		time
	@Access:		Private
	@Description:
		打印时间。
	@Parameters:
		display_second, int32, IN
			值为0则不打印时间的秒部分，否则打印。
	@Return:
		int32
			返回0则失败，否则成功。
*/
static
int32
time(IN int32 display_second)
{
	struct CMOSDateTime dt;
	get_cmos_date_time(&dt);
	printn(dt.hour);
	print_str(":");
	printn(dt.minute);
	if(display_second)
	{
		print_str(":");
		printn(dt.second);
	}
	return 1;
}

/**
	@Function:		datetime
	@Access:		Private
	@Description:
		打印日期和时间。
	@Parameters:
	@Return:
		int32
			返回0则失败，否则成功。	
*/
static
int32
datetime(void)
{
	date();
	print_str("\n");
	time(1);
	return 1;
}

/**
	@Function:		clock
	@Access:		Private
	@Description:
		设置控制台的时间的显示或关闭。
	@Parameters:
		param, int8 *, IN
			可以传入的值为"on"和"off"。
	@Return:
		int32
			返回0则失败，否则成功。	
*/
static
int32
clock(IN int8 * param)
{
	if(strcmp(param, "on") == 0)
	{
		enable_clock = 1;
		return 1;	
	}	
	else if(strcmp(param, "off") == 0)
	{
		enable_clock = 0;
		return 1;	
	}	
	else
	{
		error(FORMAT("clock {on|off}"));
		return 0;
	}
}

/**
	@Function:		format
	@Access:		Private
	@Description:
		格式化磁盘。
	@Parameters:
		param, int8 *, IN
			磁盘标识符。
	@Return:
		int32
			返回0则失败，否则成功。	
*/
static
int32
format(IN int8 * param)
{
	if(strcmp(param, "") == 0)
	{
		error(FORMAT("format {DA|DB|VA|VB|...}"));
		return 0;
	}
	if(format_disk(param))
	{
		print_str("OK!");
		return 1;
	}
	else
	{
		error("Failed to format disk!");		
		return 0;
	}
}

/**
	@Function:		cdisk
	@Access:		Private
	@Description:
		检查磁盘是否为IFS1，并显示相关信息。
	@Parameters:
		param, int8 *, IN
			磁盘标识符。
	@Return:
		int32
			返回0则失败，否则成功。	
*/
static
int32
cdisk(IN int8 * param)
{
	if(strcmp(param, "") == 0)
	{
		error(FORMAT("cdisk {DA|DB|VA|VB|...}"));
		return 0;
	}
	print_str(param);
	if(check_disk(param))
		print_str(" is IFS1!");
	else
		error(" is not IFS1!");
	return 1;
}

/**
	@Function:		mkdir
	@Access:		Private
	@Description:
		创建目录。
	@Parameters:
		path, int8 *, IN
			目标路径。
		name, int8 *, IN
			新建的目录的名称。
	@Return:
		int32
			返回0则失败，否则成功。	
*/
static
int32
mkdir(	IN int8 * path, 
		IN int8 * name)
{
	if(strcmp(path, "") == 0 || strcmp(name, "") == 0)
	{
		error(FORMAT("mkdir {path} {dir name}"));
		return 0;
	}
	if(!is_valid_df_name(name))
	{
		error("Invalid directory name!");
		return 0;
	}
	int8 temp[MAX_PATH_BUFFER_LEN];
	if(!fix_path(path, current_path, temp))
	{
		error("Invalid path!");
		return 0;
	}
	if(create_dir(temp, name))
	{
		print_str("OK!");
		return 1;
	}
	else
	{
		error("Failed to create a directory!");
		return 0;
	}
}

/**
	@Function:		mkfile
	@Access:		Private
	@Description:
		创建文件。
	@Parameters:
		path, int8 *, IN
			目标路径。
		name, int8 *, IN
			新建的文件的名称。
	@Return:
		int32
			返回0则失败，否则成功。	
*/
static
int32
mkfile(	IN int8 * path,
		IN int8 * name)
{
	if(strcmp(path, "") == 0 || strcmp(name, "") == 0)
	{
		error(FORMAT("mkfile {path} {file name}"));
		return 0;
	}
	if(!is_valid_df_name(name))
	{
		error("Invalid file name!");
		return 0;
	}
	int8 temp[MAX_PATH_BUFFER_LEN];
	if(!fix_path(path, current_path, temp))
	{
		error("Invalid path!");
		return 0;
	}
	if(create_file(temp, name))
	{
		print_str("OK!");
		return 1;
	}
	else
	{
		error("Failed to create a file!");
		return 0;
	}
}

/**
	@Function:		deldir
	@Access:		Private
	@Description:
		删除目录。但是该目录必须为空。
	@Parameters:
		path, int8 *, IN
			目录的路径。
	@Return:
		int32
			返回0则失败，否则成功。
*/
static
int32
deldir(IN int8 * path)
{
	if(strcmp(path, "") == 0)
	{
		error(FORMAT("deldir {path}"));
		return 0;
	}
	int8 temp[MAX_PATH_BUFFER_LEN];
	if(!fix_path(path, current_path, temp))
	{
		error("Invalid path!");
		return 0;
	}
	if(!is_sub_dir(temp, current_path) && del_dir(temp))
	{
		print_str("OK!");
		return 1;
	}
	else
	{
		error("Failed to delete a directory!");
		return 0;
	}
}

/**
	@Function:		deldirs
	@Access:		Private
	@Description:
		删除目录。包括该目录下的所有内容。
	@Parameters:
		path, int8 *, IN
			目录的路径。
	@Return:
		int32
			返回0则失败，否则成功。
*/
static
int32
deldirs(IN int8 * path)
{
	if(strcmp(path, "") == 0)
	{
		error(FORMAT("deldirs {path}"));
		return 0;
	}
	int8 temp[MAX_PATH_BUFFER_LEN];
	if(!fix_path(path, current_path, temp))
	{
		error("Invalid path!");
		return 0;
	}
	if(!is_sub_dir(temp, current_path) && del_dirs(temp))
	{
		print_str("OK!");
		return 1;
	}
	else
	{
		error("Failed to delete directorys!");
		return 0;
	}
}

/**
	@Function:		delfile
	@Access:		Private
	@Description:
		删除文件。
	@Parameters:
		path, int8 *, IN
			文件的路径。
	@Return:
		int32
			返回0则失败，否则成功。
*/
static
int32
delfile(IN int8 * path)
{
	if(strcmp(path, "") == 0)
	{
		error(FORMAT("delfile {path}"));
		return 0;
	}
	int8 temp[MAX_PATH_BUFFER_LEN];
	if(!fix_path(path, current_path, temp))
	{
		error("Invalid path!");
		return 0;
	}
	if(del_file(temp))
	{
		print_str("OK!");
		return 1;
	}
	else
	{
		error("Failed to delete a file!");
		return 0;
	}
}

/**
	@Function:		rndir
	@Access:		Private
	@Description:
		重命名文件夹。
	@Parameters:
		path, int8 *, IN
			重命名文件夹的路径。
		new_name, int8 *, IN
			新名称。
	@Return:
		int32
			返回0则失败，否则成功。	
*/
static
int32
rndir(	IN int8 * path, 
		IN int8 * new_name)
{
	if(strcmp(path, "") == 0 || strcmp(new_name, "") == 0)
	{
		error(FORMAT("rndir {path} {new name}"));
		return 0;
	}
	int8 temp[MAX_PATH_BUFFER_LEN];
	if(!fix_path(path, current_path, temp))
	{
		error("Invalid path!");
		return 0;
	}
	if(!is_sub_dir(temp, current_path) && rename_dir(temp, new_name))
	{
		print_str("OK!");
		return 1;
	}
	else
	{
		error("Failed to rename directory!");
		return 0;
	}
}

/**
	@Function:		rnfile
	@Access:		Private
	@Description:
		重命名文件。
	@Parameters:
		path, int8 *, IN
			重命名文件的路径。
		new_name, int8 *, IN
			新名称。
	@Return:
		int32
			返回0则失败，否则成功。	
*/
static
int32
rnfile(	IN int8 * path, 
		IN int8 * new_name)
{
	if(strcmp(path, "") == 0 || strcmp(new_name, "") == 0)
	{
		error(FORMAT("rnfiler {path} {new name}"));
		return 0;
	}
	int8 temp[MAX_PATH_BUFFER_LEN];
	if(!fix_path(path, current_path, temp))
	{
		error("Invalid path!");
		return 0;
	}
	if(rename_file(temp, new_name))
	{
		print_str("OK!");
		return 1;
	}
	else
	{
		error("Failed to rename file!");
		return 0;
	}
}

/**
	@Function:		files
	@Access:		Private
	@Description:
		打印指定路径下的所有文件和目录信息。
	@Parameters:
		path, int8 *, IN
			指定路径。
		simple, int32, IN
			值为0则以简单的方式打印文件和目录信息，否则详细地打印信息。
		part, int32, IN
			值为0则直接打印所有文件和目录信息，否则每打印20项暂停等待输入继续。
	@Return:
		int32
			返回0则失败，否则成功。	
*/
static
int32
files(	IN int8 * path, 
		IN int32 simple, 
		IN int32 part)
{
	if(strcmp(path, "") == 0)
		path = current_path;
	int8 temp[MAX_PATH_BUFFER_LEN];
	if(!fix_path(path, current_path, temp))
	{
		error("Invalid path!");
		return 0;
	}
	path = temp;
	uint ui, ui1;
	DSLLinkedList * list = dsl_lnklst_new();
	int32 count = dir_list(path, list);
	if(count == -1)
	{
		error("Invalid path!");
		return 0;
	}
	for(ui = 0; ui < count; ui++)
	{
		if(part && ui != 0 && ui % 20 == 0)
		{
			print_str("Press any key to continue!\n");
			if(get_char() == KEY_EXT)
				get_char();
		}
		if(simple)
		{
			struct RawBlock * block = dsl_lnklst_get(list, ui)->value.value.object_value;
			if(block->type == BLOCK_TYPE_FILE)
			{
				struct FileBlock * file = (struct FileBlock *)block;
				print_str_p(file->filename, CC_YELLOW);
			}
			else if(block->type == BLOCK_TYPE_DIR)
			{
				struct DirBlock * dir = (struct DirBlock *)block;
				print_str_p(dir->dirname, CC_GREEN);
			}
			else if(block->type == BLOCK_TYPE_SLINK)
			{
				struct SLinkBlock * slink = (struct SLinkBlock *)block;
				print_str_p(slink->filename, CC_BLUE);
			}
			else
				print_str_p("????", CC_RED);
			print_str(" ");
		}
		else
		{
			int8 name[21];
			for(ui1 = 0; ui1 < sizeof(name) - 1; ui1++)
				name[ui1] = ' ';
			name[sizeof(name) - 1] = '\0';
			struct RawBlock * block = dsl_lnklst_get(list, ui)->value.value.object_value;
			if(block->type == BLOCK_TYPE_FILE)
			{
				struct FileBlock * file = (struct FileBlock *)block;
				if(strlen(file->filename) > sizeof(name) - 1)
				{
					memcpy(name, file->filename, sizeof(name) - 1);
					name[19] = '.';
					name[18] = '.';
					name[17] = '.';
				}
				else
					memcpy(name, file->filename, strlen(file->filename));
				print_str_p(name, CC_YELLOW);
				print_str("       ");
				print_datetime(&(file->change));
				print_str(" ");
				printn(file->length);
				print_str("Bytes");
			}
			else if(block->type == BLOCK_TYPE_DIR)
			{
				struct DirBlock * dir = (struct DirBlock *)block;
				if(strlen(dir->dirname) > sizeof(name) - 1)
				{
					memcpy(name, dir->dirname, sizeof(name) - 1);
					name[19] = '.';
					name[18] = '.';
					name[17] = '.';
				}
				else		
					memcpy(name, dir->dirname, strlen(dir->dirname));
				print_str_p(name, CC_GREEN);
				print_str(" <DIR> ");
				print_datetime(&(dir->change));
			}
			else if(block->type == BLOCK_TYPE_SLINK)
			{
				struct SLinkBlock * slink = (struct SLinkBlock *)block;
				if(strlen(slink->filename) > sizeof(name) - 1)
				{
					memcpy(name, slink->filename, sizeof(name) - 1);
					name[19] = '.';
					name[18] = '.';
					name[17] = '.';
				}
				else		
					memcpy(name, slink->filename, strlen(slink->filename));
				print_str_p(name, CC_BLUE);
				print_str(" <LNK> ");
			}
			else
				print_str_p("????", CC_RED);
			print_str("\n");
		}
	}
	dsl_lnklst_delete_all_object_node(list);
	free_memory(list);
	return 1;
}

/**
	@Function:		cd
	@Access:		Private
	@Description:
		切换当前目录。
	@Parameters:
		path, int8 *, IN
			要切换的目标目录路径。
	@Return:
		int32
			返回0则失败，否则成功。	
*/
static
int32
cd(IN int8 * path)
{
	int8 temp[MAX_PATH_BUFFER_LEN];
	if(!fix_path(path, current_path, temp))
	{
		error("Invalid path!");
		return 0;
	}
	int32 type;
	int8 symbol[3];
	uint id = parse_path(temp, symbol, &type);
	struct DirBlock dir;
	if(	id == 0xFFFFFFFF
		|| !get_block(symbol, id, (struct RawBlock *)&dir) 
		|| dir.used == 0 
		|| dir.type != BLOCK_TYPE_DIR)
	{
		error("Invalid path!");
		return 0;
	}
	strcpy(current_path, temp);
	return 1;
}

/**
	@Function:		cpfile
	@Access:		Private
	@Description:
		复制文件。
	@Parameters:
		src_path, int8 *, IN
			源文件路径。
		dst_path, int8 *, IN
			目标文件夹路径。
		name, int8 *, IN
			新的文件的文件名。
	@Return:
		int32
			返回0则失败，否则成功。		
*/
static
int32
cpfile(	IN int8 * src_path, 
		IN int8 * dst_path, 
		IN int8 * name)
{
	int8 temp[MAX_PATH_BUFFER_LEN];
	int8 temp1[MAX_PATH_BUFFER_LEN];
	if(!fix_path(src_path, current_path, temp))
	{
		error("Invalid path!");
		return 0;
	}
	if(!fix_path(dst_path, current_path, temp1))
	{
		error("Invalid path!");
		return 0;
	}
	if(copy_file(temp, temp1, name))
	{
		print_str("OK!");
		return 1;
	}
	else
	{
		error("Failed to copy a file");
		return 0;
	}
}

/**
	@Function:		kill
	@Access:		Private
	@Description:
		杀死任务。
	@Parameters:
		tid_s, int8 *, IN
			任务ID。
	@Return:
		int32
			返回0则失败，否则成功。		
*/
static
int32
kill(IN int8 * tid_s)
{
	if(strcmp(tid_s, "") == 0)
	{
		error(FORMAT("kill {task id}"));
		return 0;
	}
	if(!is_valid_uint(tid_s))
	{
		error("Task id must unsigned integer!");
		return 0;
	}
	int32 tid = (int32)stol(tid_s);
	if(kill_task(tid))
	{
		print_str("OK!");
		return 1;
	}
	else
	{
		error("Failed to kill a task!");
		return 0;
	}
}

/**
	@Function:		tasks
	@Access:		Private
	@Description:
		打印所有正在运行的任务信息。
	@Parameters:
		part, int32, IN
			值为0则直接打印正在运行的任务信息，否则每打印20项暂停等待输入继续。
	@Return:
		int32
			返回0则失败，否则成功。	
*/
static
int32
tasks(IN int32 part)
{
	int32 i;
	uint32 ui;
	for(i = 0; i < MAX_TASK_COUNT; i++)
	{
		if(part && i != 0 && i % 20 == 0)
		{
			print_str("Press any key to continue!\n");
			if(get_char() == KEY_EXT)
				get_char();
		}
		struct Task task;
		if(get_task_info(i, &task))
		{
			int8 tid_s[30];
			ltos(tid_s, i);
			print_str_p(tid_s, CC_YELLOW);
			for(ui = 0; ui < 6 - strlen(tid_s); ui++)
				print_str(" ");
			printn(task.used_memory_size / 1024 / 1024);
			print_str("MB, ");
			print_str(task.name);
			print_str("\n");
		}
	}
	return 1;
}

/**
	@Function:		run
	@Access:		Private
	@Description:
		运行一个程序。
	@Parameters:
		path, int8 *, IN
			程序的路径。
		cmd, int8 *, IN
			运行该程序时所使用的命令。
	@Return:
		int32
			返回-1则失败，否则成功。	
*/
static
int32
run(IN int8 * path, 
	IN int8 * cmd)
{
	if(strcmp(path, "") == 0)
	{
		error(FORMAT("run {path}"));
		return -1;
	}
	int32 r;
	int8 temp[MAX_PATH_BUFFER_LEN];
	if(!fix_path(path, current_path, temp))
	{
		error("Invalid path!");
		return -1;
	}
	if((r = create_task_by_file(temp, cmd, current_path)) == -1)
		error("Failed to run application!");
	else
		task_ready(r);
	return r;
}

/**
	@Function:		run_wait
	@Access:		Private
	@Description:
		运行一个程序。并且控制台会等待程序运行完毕才能进行进一步操作。
	@Parameters:
		path, int8 *, IN
			程序的路径。
		cmd, int8 *, IN
			运行该程序时所使用的命令。
	@Return:
		int32
			返回-1则失败，否则成功。		
*/
static
int32
run_wait(	IN int8 * path, 
			IN int8 * cmd)
{
	if(strcmp(path, "") == 0)
	{
		error(FORMAT("{application path}"));
		return -1;
	}
	int32 r = -1;
	int8 temp[MAX_PATH_BUFFER_LEN];
	if(!fix_path(path, current_path, temp))
	{
		error("Invalid path!");
		return -1;
	}

	//在这里调用 lock 是为了确保 wait_app_tid = r; 的执行。
	//如果在 wait_app_tid = r; 被执行之前内核任务被切换到新的任务，
	//并且新的任务在被切换之前调用了 app_exit，新的任务将不能正常的
	//退出。因为 wait_app_tid 此时等于-1。
	lock();
	if((r = create_task_by_file(temp, cmd, current_path)) == -1)
	{
		unlock();
		error("Failed to run application!");
		return -1;
	}
	wait_app_tid = r;
	unlock();
	task_ready(r);
	return r;
}

/**
	@Function:		help
	@Access:		Private
	@Description:
		显示帮助信息。
	@Parameters:
	@Return:
		int32
			返回0则失败，否则成功。
*/
static
int32
help(void)
{
	FILE * fptr = fopen(SYSTEM_HELP_FILE, FILE_MODE_READ);
	if(fptr == NULL)
	{
		error("Cannot open file '"SYSTEM_HELP_FILE"'!");
		return 0;
	}
	int8 * text = (char *)alloc_memory(flen(fptr) + 1);
	if(text == NULL)
	{
		error("No enough memory!");
		fclose(fptr);
		return 0;
	}
	text[flen(fptr)] = '\0';
	if(!fread(fptr, text, flen(fptr)))
	{
		error("Cannot read file '"SYSTEM_HELP_FILE"'!");
		fclose(fptr);
		return 0;
	}
	int8 chr;
	int32 line = 0;
	int32 end = 0;
	while((chr = *(text++)) != '\0' && !end)
	{
		if(line != 0 && line % 20 == 0)
		{
			while(1)
			{
				print_str("Press Enter to continue, Q to quit!\n");
				int8 input_char = get_char();
				if(input_char == '\n')
					break;
				else if(input_char == 'Q' || input_char == 'q')
				{
					end = 1;
					break;
				}
				else if(input_char == KEY_EXT)
					get_char();
			}
			line = 0;
		}
		print_char(chr);
		if(chr == '\n')
			line++;
	}
	fclose(fptr);
	return 1;
}

/**
	@Function:		reboot
	@Access:		Private
	@Description:
		重启计算机。
	@Parameters:
	@Return:
*/
static
void
reboot(void)
{
	print_str("Killing all tasks...\n");
	kill_all_tasks();
	print_str("OK!");
	reboot_system();
}

/**
	@Function:		shutdown
	@Access:		Private
	@Description:
		关闭计算机。
	@Parameters:
	@Return:
*/
static
void
shutdown(void)
{
	print_str("Killing all tasks...\n");
	kill_all_tasks();
	print_str("OK!");
	shutdown_system();
}

/**
	@Function:		vmode
	@Access:		Private
	@Description:
		修改显示模式。
	@Parameters:
		mode, int8 *, IN
			模式。可以为: "text", "vesa640_480", "vesa800_600", "vesa1024_768", "vesa1280_1024"。
	@Return:
		int32
			返回0则失败，否则成功。	
*/
static
int32
vmode(IN int8 * mode)
{
	FILE * fptr = NULL;
	if(strcmp(mode, "text") == 0)
		fptr = fopen(KERNELLDR_TEXT, FILE_MODE_READ);
	else if(strcmp(mode, "vesa640_480") == 0)
		fptr = fopen(KERNELLDR_VESA_640_480, FILE_MODE_READ);
	else if(strcmp(mode, "vesa800_600") == 0)
		fptr = fopen(KERNELLDR_VESA_800_600, FILE_MODE_READ);
	else if(strcmp(mode, "vesa1024_768") == 0)
		fptr = fopen(KERNELLDR_VESA_1024_768, FILE_MODE_READ);
	else if(strcmp(mode, "vesa1280_1024") == 0)
		fptr = fopen(KERNELLDR_VESA_1280_1024, FILE_MODE_READ);
	else
	{
		error(FORMAT("vmode {text|vesa640_480|vesa800_600|vesa1024_768|vesa1280_1024}"));
		return 0;
	}
	if(fptr == NULL)
	{
		error("Failed to change video mode!");
		return 0;
	}
	uint8 * buffer = alloc_memory((flen(fptr) / 512 + 1) * 512);
	uint r;
	if(buffer == NULL || !(r = fread(fptr, buffer, flen(fptr))) )
	{
		error("Failed to change video mode!");
		fclose(fptr);
		return 0;
	}
	fclose(fptr);
	if(!write_sectors(SYSTEM_DISK, 1, flen(fptr) / 512 + 1, buffer))
	{
		error("Failed to change video mode!");
		free_memory(buffer);
		return 0;
	}
	else
	{
		print_str("OK!");
		free_memory(buffer);
		return 1;
	}
}

/**
	@Function:		cpuid
	@Access:		Private
	@Description:
		打印CPU相关信息。
	@Parameters:
	@Return:
		int32
			返回0则失败，否则成功。	
*/
static
int32
cpuid(void)
{
	int8 vendor_id_string[13];
	int8 brand_string[49];
	get_vendor_id_string(vendor_id_string);
	print_str_p("Vendor ID String: ", CC_YELLOW);
	print_str(vendor_id_string);
	print_str("\n");
	get_brand_string(brand_string);	
	trim(brand_string);
	print_str_p("Brand String: ", CC_YELLOW);
	print_str(brand_string);
	print_str("\n");
	struct CacheInfo L1;
	if(get_cpu_L1(&L1))
	{
		print_str_p("L1: ", CC_YELLOW);
		printn(L1.size);
		print_str(", ");
		printn(L1.way);
		print_str(", ");
		printn(L1.linesize);
		print_str("\n");
	}
	struct CacheInfo L2;
	if(get_cpu_L2(&L2))
	{
		print_str_p("L2: ", CC_YELLOW);
		printn(L2.size);
		print_str(", ");
		printn(L2.way);
		print_str(", ");
		printn(L2.linesize);
		print_str("\n");
	}
	struct CacheInfo L3;
	if(get_cpu_L3(&L3))
	{
		print_str_p("L3: ", CC_YELLOW);
		printn(L3.size);
		print_str(", ");
		printn(L3.way);
		print_str(", ");
		printn(L3.linesize);
		print_str("\n");
	}
	print_str_p("Features: ", CC_YELLOW);
	print_str_p("FPU ", cpu_feature_fpu() ? CC_GREEN : CC_RED);
	print_str_p("VME ", cpu_feature_vme() ? CC_GREEN : CC_RED);
	print_str_p("DE ", cpu_feature_de() ? CC_GREEN : CC_RED);
	print_str_p("PSE ", cpu_feature_pse() ? CC_GREEN : CC_RED);
	print_str_p("TSC ", cpu_feature_tsc() ? CC_GREEN : CC_RED);
	print_str_p("MSR ", cpu_feature_msr() ? CC_GREEN : CC_RED);
	print_str_p("PAE ", cpu_feature_pae() ? CC_GREEN : CC_RED);
	print_str_p("MCE ", cpu_feature_mce() ? CC_GREEN : CC_RED);
	print_str_p("CXB ", cpu_feature_cxb() ? CC_GREEN : CC_RED);
	print_str_p("APIC ", cpu_feature_apic() ? CC_GREEN : CC_RED);
	print_str_p("SEP ", cpu_feature_sep() ? CC_GREEN : CC_RED);
	print_str_p("MTRR ", cpu_feature_mtrr() ? CC_GREEN : CC_RED);
	print_str_p("PGE ", cpu_feature_pge() ? CC_GREEN : CC_RED);
	print_str_p("MCA ", cpu_feature_mca() ? CC_GREEN : CC_RED);
	print_str_p("CMOV ", cpu_feature_cmov() ? CC_GREEN : CC_RED);
	print_str_p("PAT ", cpu_feature_pat() ? CC_GREEN : CC_RED);
	print_str_p("PSE-36 ", cpu_feature_pse36() ? CC_GREEN : CC_RED);
	print_str_p("PN ", cpu_feature_pn() ? CC_GREEN : CC_RED);
	print_str_p("MMX ", cpu_feature_mmx() ? CC_GREEN : CC_RED);
	print_str_p("FXSR ", cpu_feature_fxsr() ? CC_GREEN : CC_RED);
	print_str_p("XMM ", cpu_feature_xmm() ? CC_GREEN : CC_RED);
	print_str_p("Hyper-Threading ", cpu_feature_hyper_threading() ? CC_GREEN : CC_RED);
	print_str_p("Speed-Sted ", cpu_feature_speed_sted() ? CC_GREEN : CC_RED);
	return 1;
}

/**
	@Function:		set
	@Access:		Private
	@Description:
		设置系统属性。
	@Parameters:
		property, int8 *, IN
			属性名。
		value, int8 *, IN
			属性值。
	@Return:
		int32
			返回0则失败，否则成功。		
*/
static
int32
set(IN int8 * property, 
	IN int8 * value)
{
	if(strcmp(property, "cursor_color") == 0)
		set_cursor_color((uchar)stoi(value));
	else if(strcmp(property, "char_color") == 0)
		set_char_color((uchar)stoi(value));
	else
	{
		error("Failed to set property!");
		return 0;
	}
	print_str("OK!");
	return 1;
}

/**
	@Function:		mkslink
	@Access:		Private
	@Description:
		创建软链接。
	@Parameters:
		path, int8 *, IN
			指定创建软链接的目录。
		name, int8 *, IN
			软链接名称。
		link, int8 *, IN
			链接的目标。
	@Return:
		BOOL
			返回 FALSE 则失败，否则成功。		
*/
static
BOOL
mkslink(IN int8 * path,
		IN int8 * name,
		IN int8 * link)
{
	if(strcmp(path, "") == 0 || strcmp(name, "") == 0 || strcmp(link, "") == 0)
	{
		error(FORMAT("mkslink {path} {name} {link}"));
		return FALSE;
	}
	int8 temp[MAX_PATH_BUFFER_LEN];
	if(!fix_path(path, current_path, temp))
	{
		error("Invalid path!");
		return FALSE;
	}
	if(create_slink(temp, name, link))
	{
		print_str("OK!");
		return TRUE;
	}
	else
	{
		error("Failed to create a soft link!");
		return FALSE;
	}
}

/**
	@Function:		delslink
	@Access:		Private
	@Description:
		删除软链接。
	@Parameters:
		path, int8 *, IN
			软链接的路径。
	@Return:
		BOOL
			返回 FALSE 则失败，否则成功。		
*/
static
BOOL
delslink(IN int8 * path)
{
	if(strcmp(path, "") == 0)
	{
		error(FORMAT("delslink {path}"));
		return FALSE;
	}
	int8 temp[MAX_PATH_BUFFER_LEN];
	if(!fix_path(path, current_path, temp))
	{
		error("Invalid path!");
		return FALSE;
	}
	if(del_slink(temp))
	{
		print_str("OK!");
		return TRUE;
	}
	else
	{
		error("Failed to delete a soft link!");
		return FALSE;
	}
}

/**
	@Function:		slink
	@Access:		Private
	@Description:
		显示软链接的链接目标。
	@Parameters:
		path, int8 *, IN
			软链接的路径。
	@Return:
		BOOL
			返回 FALSE 则失败，否则成功。		
*/
static
BOOL
slink(IN int8 * path)
{
	if(strcmp(path, "") == 0)
	{
		error(FORMAT("slink {path}"));
		return FALSE;
	}
	int8 temp[MAX_PATH_BUFFER_LEN];
	if(!fix_path(path, current_path, temp))
	{
		error("Invalid path!");
		return FALSE;
	}
	int8 link[1024];
	link[1023] = '\0';
	if(get_slink_link(temp, 1023, link))
	{
		print_str(link);
		return TRUE;
	}
	else
	{
		error("Failed to show link target of soft link!");
		return FALSE;
	}
}

static int exec(char * cmd, char * lines, uint * pointer, uint line, struct Vars * local_vars_s);

#define	BATCH_MAX_CMD_LEN		1024	//脚本的命令缓冲区的最大长度
#define	BATCH_MAX_CMD_LINES		1024	//脚本的命令的最大行数
#define	BATCH_MAX_VARS_COUNT	1024	//脚本的变量的最大数量

/**
	@Function:		batch
	@Access:		Private
	@Description:
		运行脚本。
	@Parameters:
		path, int8 *, IN
			脚本文件的路径。
	@Return:
		int32
			返回0则失败，否则成功。
*/
static
int32
batch(IN int8 * path)
{
	struct Vars * local_vars_s;
	local_vars_s = alloc_vars(BATCH_MAX_VARS_COUNT);
	if(local_vars_s == NULL)
		return 0;
	FILE * fptr = fopen(path, FILE_MODE_READ);
	if(fptr == NULL)
	{
		free_vars(local_vars_s);
		return 0;
	}
	uint32 file_len = flen(fptr);
	int8 * cmds = (int8 *)alloc_memory(file_len);
	if(cmds == NULL)
	{
		fclose(fptr);
		free_vars(local_vars_s);
		return 0;
	}
	fread(fptr, cmds, file_len);
	fclose(fptr);
	uint32 ui;
	int8 cmd[BATCH_MAX_CMD_LEN];
	int8 * lines = alloc_memory(BATCH_MAX_CMD_LINES * BATCH_MAX_CMD_LEN);
	if(lines == NULL)
	{
		free_memory(cmds);
		free_vars(local_vars_s);
		return 0;
	}
	uint32 line = 0;
	uint32 index = 0;
	cmd[0] = '\0';
	for(ui = 0; ui < file_len && line < BATCH_MAX_CMD_LINES; ui++)
	{
		int8 chr = cmds[ui];
		if(chr == '\n' || index + 1 == BATCH_MAX_CMD_LEN)
		{
			trim(cmd);
			strcpy(lines + line++ * BATCH_MAX_CMD_LEN, cmd);
			cmd[0] = '\0';
			index = 0;
		}
		else
		{
			cmd[index++] = chr;
			cmd[index] = '\0';
		}
	}
	uint32 pointer = 0;
	for(; pointer < line; pointer++)
	{
		int8 * current_cmd = lines + pointer * BATCH_MAX_CMD_LEN;
		if(!exec(current_cmd, lines, &pointer, line, local_vars_s))
		{
			print_str_p("\nBatch have error. Command:\n\t", CC_RED);
			print_str_p(current_cmd, CC_RED);
			print_str("\n");
			free_memory(cmds);
			free_memory(lines);
			free_vars(local_vars_s);
			return 0;
		}
	}
	free_memory(cmds);
	free_memory(lines);
	free_vars(local_vars_s);
	return 1;
}

/**
	@Function:		exec
	@Access:		Private
	@Description:
		执行一条命令。
	@Parameters:
		cmd, int8 *, IN
			需要执行的命令。
		lines, int8 *, IN
			该命令所在的命令集合。
		pointer, uint32 *, IN OUT
			下一个将要被执行的脚本命令的指针。
		line, uint32, IN
			当前行。
		local_vars_s, struct Vars *, IN
			本地变量列表。
	@Return:
		int32
			返回0则失败，否则成功。	
*/
static
int32
exec(	IN int8 * cmd, 
		IN int8 * lines, 
		IN OUT uint32 * pointer, 
		IN uint32 line, 
		IN struct Vars * local_vars_s)
{
	int32 r = 1;
	int8 word[1024];
	parse_cmd(cmd, word, 1023);
	if(parse_cmd(NULL, word, 1023) != 0 || strlen(word) != 0)
	{
		int8 * name = word;
		if(strcmp(name, "ver") == 0)
			ver();
		else if(strcmp(name, "print") == 0)
			if(parse_cmd(NULL, word, 1023) != 0 || strlen(word) != 0)
			{
				print(word);
				r = 1;
				print_str("\n");
			}
			else
			{
				error(FORMAT("print {message}"));
				r = 0;			
			}
		else if(strcmp(name, "error") == 0)
		{
			parse_cmd(NULL, word, 1023);
			error(word);
			r = 1;
		}
		else if(strcmp(name, "disks") == 0)
		{
			r = disks();
			print_str("\n");
		}
		else if(strcmp(name, "mm") == 0)
		{
			r = mm();
			print_str("\n");
		}
		else if(strcmp(name, "date") == 0)
		{
			r = date();
			print_str("\n");
		}
		else if(strcmp(name, "time") == 0)
		{
			r = time(1);
			print_str("\n");
		}
		else if(strcmp(name, "dt") == 0)
		{
			r = datetime();
			print_str("\n");
		}
		else if(strcmp(name, "clock") == 0)
		{
			parse_cmd(NULL, word, 1023);
			r = clock(word);
			print_str("\n");
		}
		else if(strcmp(name, "format") == 0)
		{
			parse_cmd(NULL, word, 1023);
			trim(word);
			r = format(word);
			print_str("\n");
		}
		else if(strcmp(name, "cdisk") == 0)
		{
			parse_cmd(NULL, word, 1023);
			trim(word);
			r = cdisk(word);
			print_str("\n");
		}
		else if(strcmp(name, "mkdir") == 0)
		{
			int8 path[MAX_PATH_BUFFER_LEN];
			int8 name[MAX_DIRNAME_BUFFER_LEN];
			parse_cmd(NULL, path, MAX_PATH_LEN);
			parse_cmd(NULL, name, MAX_DIRNAME_LEN);
			trim(path);
			trim(name);
			r = mkdir(path, name);
			print_str("\n");
		}
		else if(strcmp(name, "mkfile") == 0)
		{
			int8 path[MAX_PATH_BUFFER_LEN];
			int8 name[MAX_FILENAME_BUFFER_LEN];
			parse_cmd(NULL, path, MAX_PATH_LEN);
			parse_cmd(NULL, name, MAX_FILENAME_LEN);
			trim(path);
			trim(name);
			r = mkfile(path, name);
			print_str("\n");
		}
		else if(strcmp(name, "deldir") == 0)
		{
			int8 path[MAX_PATH_BUFFER_LEN];
			parse_cmd(NULL, path, MAX_PATH_LEN);
			trim(path);
			r = deldir(path);
			print_str("\n");
		}
		else if(strcmp(name, "deldirs") == 0)
		{
			int8 path[MAX_PATH_BUFFER_LEN];
			parse_cmd(NULL, path, MAX_PATH_LEN);
			trim(path);
			r = deldirs(path);
			print_str("\n");
		}
		else if(strcmp(name, "delfile") == 0)
		{
			int8 path[MAX_PATH_BUFFER_LEN];
			parse_cmd(NULL, path, MAX_PATH_LEN);
			trim(path);
			r = delfile(path);
			print_str("\n");
		}
		else if(strcmp(name, "rndir") == 0)
		{
			int8 path[MAX_PATH_BUFFER_LEN];
			int8 new_name[MAX_DIRNAME_BUFFER_LEN];
			parse_cmd(NULL, path, MAX_PATH_LEN);
			parse_cmd(NULL, new_name, MAX_DIRNAME_LEN);
			trim(path);
			trim(new_name);
			r = rndir(path, new_name);
			print_str("\n");
		}
		else if(strcmp(name, "rnfile") == 0)
		{
			int8 path[MAX_PATH_BUFFER_LEN];
			int8 new_name[MAX_FILENAME_BUFFER_LEN];
			parse_cmd(NULL, path, MAX_PATH_LEN);
			parse_cmd(NULL, new_name, MAX_FILENAME_LEN);
			trim(path);
			trim(new_name);
			r = rnfile(path, new_name);
			print_str("\n");
		}
		else if(strcmp(name, "files") == 0)
		{
			int8 path[MAX_PATH_BUFFER_LEN];
			int32 simple = 0;
			int32 part = 0;
			int32 err = 0;
			path[0] = '\0';
			while(1)
			{
				parse_cmd(NULL, word, 1023);
				if(strcmp(word, "-d") == 0)
				{
					parse_cmd(NULL, path, MAX_PATH_LEN);
					trim(path);
				}
				else if(strcmp(word, "-s") == 0)
					simple = 1;
				else if(strcmp(word, "-p") == 0)
					part = 1;
				else if(strcmp(word, "") == 0)
					break;
				else
				{
					error("Invalid option!");
					err = 1;
					r = 0;
					break;
				}
			}
			if(!err)
				r = files(path, simple, part);
			print_str("\n");
		}
		else if(strcmp(name, "cd") == 0)
		{
			int8 path[MAX_PATH_BUFFER_LEN];
			parse_cmd(NULL, path, MAX_PATH_LEN);
			trim(path);
			r = cd(path);
			print_str("\n");
		}
		else if(strcmp(name, "cpfile") == 0)
		{
			int8 src_path[MAX_PATH_BUFFER_LEN];
			int8 dst_path[MAX_PATH_BUFFER_LEN];
			int8 dst_name[MAX_FILENAME_BUFFER_LEN];
			parse_cmd(NULL, src_path, MAX_PATH_LEN);
			parse_cmd(NULL, dst_path, MAX_PATH_LEN);
			parse_cmd(NULL, dst_name, MAX_FILENAME_LEN);
			r = cpfile(src_path, dst_path, dst_name);
			print_str("\n");
		}
		else if(strcmp(name, "kill") == 0)
		{
			int8 tid_s[30];
			parse_cmd(NULL, tid_s, 29);
			r = kill(tid_s);
			print_str("\n");
		}
		else if(strcmp(name, "tasks") == 0)
		{
			int32 part = 0;
			int32 err = 0;
			while(1)
			{
				parse_cmd(NULL, word, 1023);
				if(strcmp(word, "-p") == 0)
					part = 1;
				else if(strcmp(word, "") == 0)
					break;
				else
				{
					error("Invalid option!");
					err = 1;
					r = 0;
				}
			}
			if(!err)
				r = tasks(part);
			print_str("\n");
		}
		else if(strcmp(name, "run") == 0)
		{
			int8 src_path[MAX_PATH_BUFFER_LEN];
			parse_cmd(NULL, src_path, MAX_PATH_LEN);
			BOOL ran = FALSE;
			int8 buffer[10 * 1024];
			int8 path[1024];
			get_var_value_with_size(global_vars_s,
									"__path__",
									buffer,
									10 * 1024);
			split_string(NULL, NULL, 0, 0);
			while(split_string(path, buffer, ';', 1024) != NULL)
				if(exists_file(path, src_path))
				{
					int8 temp[1024];
					strcpy(temp, path);
					strcat(temp, src_path);
					r = run(temp, cmd) == -1 ? 0 : 1;
					ran = TRUE;
				}
			if(!ran)
				r = run(src_path, cmd) == -1 ? 0 : 1;
		}
		else if(strcmp(name, "clear") == 0)
		{
			clear_screen();
			r = 1;
		}
		else if(strcmp(name, "help") == 0 || strcmp(name, "?") == 0)
		{
			help();
			r = 1;
			print_str("\n");
		}
		else if(strcmp(name, "reboot") == 0)
			reboot();
		else if(strcmp(name, "shutdown") == 0)
			shutdown();
		else if(strcmp(name, "vmode") == 0)
		{
			parse_cmd(NULL, word, 1023);
			r = vmode(word);
			print_str("\n");
		}
		else if(strcmp(name, "cpuid") == 0)
		{
			r = cpuid();
			print_str("\n");
		}
		else if(strcmp(name, "set") == 0)
		{
			int8 property[1024];
			int8 value[1024];
			parse_cmd(NULL, property, 1023);
			parse_cmd(NULL, value, 1023);
			r = set(property, value);
			print_str("\n");
		}
		else if(strcmp(name, "flushlog") == 0)
		{
			write_log_to_disk();
			clear_log();
			r = 1;
			print_str("\n");
		}
		else if(strcmp(name, "mkslink") == 0)
		{
			int8 path[1024];
			int8 name[1024];
			int8 link[1024];
			parse_cmd(NULL, path, 1023);
			parse_cmd(NULL, name, 1023);
			parse_cmd(NULL, link, 1023);
			r = mkslink(path, name, link);
			print_str("\n");
		}
		else if(strcmp(name, "delslink") == 0)
		{
			int8 path[1024];
			parse_cmd(NULL, path, 1023);
			r = delslink(path);
			print_str("\n");
		}
		else if(strcmp(name, "slink") == 0)
		{
			int8 path[1024];
			parse_cmd(NULL, path, 1023);
			r = slink(path);
			print_str("\n");
		}
		//Batch
		else if(strcmp(name, "goto") == 0)
		{
			uint32 ui;
			parse_cmd(NULL, word, 1023);
			r = 0;
			for(ui = 0; ui < line; ui++)
			{
				int8 * l = lines + ui * BATCH_MAX_CMD_LEN;
				if(strcmp(l, word) == 0)
				{
					*pointer = ui;
					r = 1;
					break;
				}
			}
		}
		else if(strcmp(name, "rem") == 0)
			;
		else if(name[0] == ':')
			;
		else if(strcmp(name, "def") == 0)
		{
			int8 var_name[MAX_VAR_NAME_BUFFER_LEN];
			int8 var_value[MAX_VAR_VALUE_BUFFER_LEN];
			parse_cmd_nv(NULL, var_name, MAX_VAR_NAME_LEN);
			if(strlen(var_name) < 2)
				r = 0;
			else
			{
				struct Vars * vars_s = NULL;
				if(var_name[0] == '$')
					vars_s = local_vars_s;
				else if(var_name[0] == '@')
					vars_s = global_vars_s;
				if(new_var(vars_s, var_name + 1))
				{
					parse_cmd_nv(NULL, var_value, MAX_VAR_VALUE_LEN);
					if(set_var_value(vars_s, var_name + 1, var_value))
						r = 1;
					else
						r = 0;
				}
				else
					r = 0;
			}
		}
		else if(strcmp(name, "rpn") == 0)
		{
			int8 stack[64][1024];
			int32 stack_top = -1;
			int8 result[1024];
			double n_r = 0.0;
			parse_cmd_nv(NULL, result, 1023);
			while(1)
			{
				parse_cmd(NULL, word, 1023);
				if(strcmp(word, "") == 0)
					break;
				else if(strcmp(word, "seq") == 0)
				{
					if(stack_top == -1)
						return 0;
					int8 * str2 = stack[stack_top--];
					if(stack_top == -1)
						return 0;
					int8 * str1 = stack[stack_top--];
					if(strcmp(str1, str2) == 0)
						n_r = 1.0;
					else
						n_r = 0.0;
					int8 buffer[100];
					strcpy(stack[++stack_top], dtos(buffer, n_r));
				}
				else if(strcmp(word, "add") == 0
						|| strcmp(word, "sub") == 0
						|| strcmp(word, "mul") == 0
						|| strcmp(word, "div") == 0
						|| strcmp(word, "mod") == 0
						|| strcmp(word, "gra") == 0
						|| strcmp(word, "les") == 0
						|| strcmp(word, "ge") == 0
						|| strcmp(word, "le") == 0
						|| strcmp(word, "eq") == 0
						|| strcmp(word, "and") == 0
						|| strcmp(word, "or") == 0)
				{
					if(stack_top == -1)
						return 0;
					double n1 = stod(stack[stack_top--]);
					if(stack_top == -1)
						return 0;
					double n0 = stod(stack[stack_top--]);
					if(strcmp(word, "add") == 0)
						n_r = n0 + n1;
					else if(strcmp(word, "sub") == 0)
						n_r = n0 - n1;
					else if(strcmp(word, "mul") == 0)
						n_r = n0 * n1;
					else if(strcmp(word, "div") == 0)
						n_r = n0 / n1;
					else if(strcmp(word, "mod") == 0)
						n_r = (int32)n0 % (int32)n1;
					else if(strcmp(word, "gra") == 0)
						n_r = n0 > n1 ? 1 : 0;
					else if(strcmp(word, "les") == 0)
						n_r = n0 < n1 ? 1 : 0;
					else if(strcmp(word, "ge") == 0)
						n_r = n0 >= n1 ? 1 : 0;
					else if(strcmp(word, "le") == 0)
						n_r = n0 <= n1 ? 1 : 0;
					else if(strcmp(word, "eq") == 0)
						n_r = n0 == n1 ? 1 : 0;
					else if(strcmp(word, "and") == 0)
						n_r = n0 && n1;
					else if(strcmp(word, "or") == 0)
						n_r = n0 || n1;
					int8 buffer[100];
					strcpy(stack[++stack_top], dtos(buffer, n_r));
				}
				else if(strcmp(word, "int") == 0
						|| strcmp(word, "uint") == 0
						|| strcmp(word, "not") == 0)
				{
					if(stack_top == -1)
						return 0;
					double n = stod(stack[stack_top--]);
					if(strcmp(word, "int") == 0)
						n_r = (int32)n;
					else if(strcmp(word, "uint") == 0)
						n_r = (uint32)n;
					else if(strcmp(word, "not") == 0)
						n_r = n == 0.0 ? 1.0 : 0.0;
					int8 buffer[100];
					strcpy(stack[++stack_top], dtos(buffer, n_r));
				}
				else if(strcmp(word, "strcat") == 0)
				{
					if(stack_top == -1)
						return 0;
					int8 * str = stack[stack_top--];
					if(stack_top == -1)
						return 0;
					strcat(stack[stack_top], str);
				}
				else
				{
					if(stack_top == 63)
						return 0;
					strcpy(stack[++stack_top], word);
				}
			}
			if(stack_top != 0)
				return 0;
			struct Vars * vars_s = NULL;
			if(result[0] == '$')
				set_var_value(local_vars_s, result + 1, stack[stack_top--]);
			else if(result[0] == '@')
				set_var_value(global_vars_s, result + 1, stack[stack_top--]);
			else if(result[0] == ':' && n_r != 0.0)
			{
				uint32 ui;
				for(ui = 0; ui < line; ui++)
				{
					int8 * l = lines + ui * BATCH_MAX_CMD_LEN;
					if(strcmp(l, result) == 0)
					{
						*pointer = ui;
						r = 1;
						break;
					}
				}
			}
			r = 1;
		}
		else if(strlen(name) >= 4 && strcmp(name + strlen(name) - 4, ".bat") == 0)
		{
			BOOL ran = FALSE;
			int8 buffer[10 * 1024];
			int8 path[1024];
			get_var_value_with_size(global_vars_s,
									"__path__",
									buffer,
									10 * 1024);
			split_string(NULL, NULL, 0, 0);
			while(split_string(path, buffer, ';', 1024) != NULL)
				if(exists_file(path, name))
				{
					int8 temp[1024];
					strcpy(temp, path);
					strcat(temp, name);
					r = batch(temp);
					ran = TRUE;
					break;
				}
			if(!ran)
				r = batch(name);
		}
		else
		{
			BOOL ran = FALSE;
			int8 buffer[10 * 1024];
			int8 path[1024];
			get_var_value_with_size(global_vars_s,
									"__path__",
									buffer,
									10 * 1024);
			split_string(NULL, NULL, 0, 0);
			while(split_string(path, buffer, ';', 1024) != NULL)
				if(exists_file(path, name))
				{
					int8 temp[1024];
					strcpy(temp, path);
					strcat(temp, name);
					r = run_wait(temp, cmd) == -1 ? 0 : 1;
					ran = TRUE;
				}
			if(!ran)
				r = run_wait(name, cmd) == -1 ? 0 : 1;
		}
	}
	while(wait_app_tid != -1)
		if(get_task_info_ptr(wait_app_tid) == NULL)
			wait_app_tid = -1;
	return r;
}

/**
	@Function:		call_init_bat
	@Access:		Private
	@Description:
		调用初始化脚本。
	@Parameters:
	@Return:
*/
static
void
call_init_bat(void)
{
	int8 buffer[100];
	//__video_mode__
	new_var(global_vars_s, "__video_mode__");
	if(!vesa_is_valid())
		set_var_value(global_vars_s, "__video_mode__", "text");
	else	
	{
		uint32 width = vesa_get_width();
		uint32 height = vesa_get_height();
		if(width == 640 && height == 480)
			set_var_value(global_vars_s, "__video_mode__", "vesa640_480");
		else if(width == 800 && height == 600)
			set_var_value(global_vars_s, "__video_mode__", "vesa800_600");
		else if(width == 1024 && height == 768)
			set_var_value(global_vars_s, "__video_mode__", "vesa1024_768");
		else if(width == 1280 && height == 1024)
			set_var_value(global_vars_s, "__video_mode__", "vesa1280_1024");
	}

	//__master_ver__
	new_var(global_vars_s, "__master_ver__");
	set_var_value(global_vars_s, "__master_ver__", MASTER_VER);

	//__slave_ver__
	new_var(global_vars_s, "__slave_ver__");
	set_var_value(global_vars_s, "__slave_ver__", SLAVE_VER);

	batch(SYSTEM_INIT_BAT);
}

/**
	@Function:		console
	@Access:		Public
	@Description:
		控制台入口函数。
	@Parameters:
	@Return:		
*/
void
console(void)
{
	int8 command[1024];
	global_vars_s = alloc_vars(BATCH_MAX_VARS_COUNT);
	struct Vars * local_vars_s;
	local_vars_s = alloc_vars(BATCH_MAX_VARS_COUNT);
	call_init_bat();
	ver();
	while(1)
	{
		if(strcmp(current_path, "") == 0)
			print_str("#");
		else
			print_str(current_path);
		print_str("> ");
		get_strn(command, 1023);
		exec(command, NULL, 0, 0, local_vars_s);
	}
}

/**
	@Function:		console_clock
	@Access:		Public
	@Description:
		该函数可以在控制台右上角显示当前日期时间。
	@Parameters:
	@Return:		
*/
void
console_clock(void)
{
	if(enable_clock)
	{
		//YYYY-MM-DD WWWWWWWWW HH:NN
		lock_cursor();
		uint32 ui;
		uint16 max_chars = 26;
		uint16 startx = COLUMN - max_chars;	
		uint16 x, y;
		get_cursor(&x, &y);
		set_cursor_pos(startx, 0);
		for(ui = 0; ui < max_chars; ui++)
			print_str(" ");
		set_cursor_pos(startx, 0);
		date();
		print_str(" ");
		time(0);
		set_cursor_pos(x, y);
		unlock_cursor();
	}
}

/**
	@Function:		get_wait_app_tid
	@Access:		Public
	@Description:
		获取控制台正在等待的任务的任务ID。
	@Parameters:
	@Return:
		int32
			控制台正在等待的任务的任务ID。		
*/
int32
get_wait_app_tid(void)
{
	return wait_app_tid;
}

/**
	@Function:		set_wait_app_tid
	@Access:		Public
	@Description:
		设置控制台正在等待的任务的任务ID。
	@Parameters:
		tid, int32, IN
			控制台正在等待的任务的任务ID。
	@Return:	
*/
void
set_wait_app_tid(IN int32 tid)
{
	wait_app_tid = tid;
}

/**
	@Function:		set_clock
	@Access:		Public
	@Description:
		设置控制台的时钟是否可用。
	@Parameters:
		enable, int32, IN
			值为0时不可用，否则可用。
	@Return:	
*/
void
set_clock(IN int32 enable)
{
	enable_clock = enable;
}

/**
	@Function:		get_current_path
	@Access:		Public
	@Description:
		获取当前目录路径。
	@Parameters:
		path, int8 *, IN
			用于储存当前目录路径的缓冲区。
	@Return:	
*/
void
get_current_path(OUT int8 * path)
{
	strcpy(path, current_path);
}
