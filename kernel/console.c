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
#include "disk.h"
#include "tasks.h"
#include "system.h"
#include "vars.h"
#include "cpu.h"
#include "kstring.h"
#include "log.h"
#include "pci.h"
#include "config.h"

#include "fs/ifs1/fs.h"
#include "fs/ifs1/blocks.h"

#include "windows/message.h"
#include "windows/detail.h"

#include "sb/sb16.h"

#include <ilib/string.h>

#include "386.h"
#include "vesa.h"
#include "die.h"

#include "mqueue.h"

#include "net/net.h"
#include "net/pcnet2.h"
#include "net/endian.h"

DEFINE_LOCK_IMPL(console)

#define	FORMAT(message)	"Foramt:\n\t"message

static int32 enable_clock 						= 1;
static int8 current_path[MAX_PATH_BUFFER_LEN] 	= "";
static int32 wait_app_tid 						= -1;
static struct Vars * global_vars_s 				= NULL;
static int8 promptns[32]						= "#";
static int8 prompts[32]							= "%s";
static uint8 promptns_color						= 7;
static uint8 prompts_color 						= 7;
static BOOL _exit_batch							= FALSE;
static BOOL _run_in_batch						= FALSE;

#define	_PARSE_COMMAND(cmd, word, n)	\
	(_ConParseCommand(cmd, word, n, 1, local_vars_s))

#define	_PARSE_COMMAND_WITHOUT_VAR(cmd, word, n)	\
	(_ConParseCommand(cmd, word, n, 0, NULL))

/**
	@Function:		_ConParseCommand
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
_ConParseCommand(	IN int8 * cmd,
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
		UtlCopyString(buffer, sizeof(buffer), cmd);
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
				KnlGetVarValue(local_vars_s, word + 1, word);
			else if(word[0] == '@')
				KnlGetVarValue(global_vars_s, word + 1, word);
	}
	return len - pos;
}

/**
	@Function:		_ConCmdVer
	@Access:		Private
	@Description:
		打印版本信息。
	@Parameters:
	@Return:	
*/
static
void
_ConCmdVer(void)
{
	int8 name[KB(1)]		= "ISystem";
	int8 platform[KB(1)]	= "Unknow platform";
	int8 version[KB(1)]		= "Unknow version";
	int8 group[KB(1)]		= "Unknow group";
	config_system_get_string("Name", name, sizeof(name));
	config_system_get_string("Platform", platform, sizeof(platform));
	config_system_get_string("Version", version, sizeof(version));
	config_system_get_string("Group", group, sizeof(group));
	int8 info[KB(5)];
	sprintf_s(	info,
				sizeof(info),
				"%s Platform %s Version %s\n%s\n\n",
				name,
				platform,
				version,
				group);
	ScrPrintString(info);
}

/**
	@Function:		_ConCmdPrint
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
_ConCmdPrint(IN int8 * text)
{
	ScrPrintString(text);
}

/**
	@Function:		_ConCmdError
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
_ConCmdError(IN int8 * text)
{
	ScrPrintStringToStderrWithProperty(text, CC_RED);
	ScrPrintStringToStderr("\n");
}

/**
	@Function:		_ConCmdShowDiskInfo
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
_ConCmdShowDiskInfo(void)
{
	uint32 disk_count = DskGetCount();
	uint32 ui;
	ScrPrintStringWithProperty(
		"Name    System  Read                Write               Size                \n", 
		CC_YELLOW | CBGC_GRAYWHITE);
	for(ui = 0; ui < disk_count; ui++)
	{
		int8 symbol[3];
		DskGetBySymbol(ui, symbol);
		ScrPrintString(symbol);
		ScrPrintString("\t\t");
		if(DskHasSystem(symbol))
			ScrPrintString("Yes\t\t");
		else
			ScrPrintString("No\t\t");

		// 输出磁盘读出字节数。
		uint64 rbytes = DskGetRBytes(symbol);
		ScrPrintString("0x");
		printuihex((uint32)(rbytes >> 32));
		printuihex((uint32)rbytes);
		ScrPrintString("\t");

		// 输出磁盘写入字节数。
		uint64 wbytes = DskGetWBytes(symbol);
		ScrPrintString("0x");
		printuihex((uint32)(wbytes >> 32));
		printuihex((uint32)wbytes);
		ScrPrintString("\t");

		uint32 disk_size = DskGetSize(symbol);
		printun(disk_size);
		ScrPrintString("KB, ");
		printun(disk_size / 1024);
		ScrPrintString("MB\n");
	}
	return 1;
}

/**
	@Function:		_ConCmdShowMemoryInfo
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
_ConCmdShowMemoryInfo(void)
{
	uint64 max_memory_byte = get_memory_size();
	uint32 max_memory_mbyte = (uint32)max_memory_byte == 0 ? 4096 : (uint32)max_memory_byte / 1024 / 1024;
	ScrPrintString("Max Memory:  ");
	printun(max_memory_mbyte);
	ScrPrintString("MB\n");
	uint64 used_memory_byte = MemGetUsedLength();
	uint32 used_memory_mbyte = (uint32)used_memory_byte == 0 ? 4096 : (uint32)used_memory_byte / 1024 / 1024;
	ScrPrintString("Used Memory: ");
	printun(used_memory_mbyte);
	ScrPrintString("MB\n");
	uint32 t, c, m;
	ScrPrintString("MBDT:        ");
	MemGetMBDTableInfo(&t, &c, &m);
	printn(c);
	ScrPrintString("/");
	printn(t);
	ScrPrintString(", ");
	printn(m / 1024 / 1024);
	ScrPrintString("MB\n");
	ScrPrintString("UMBDT:       ");
	MemGetUsedMBDTableInfo(&t, &c, &m);
	printn(c);
	ScrPrintString("/");
	printn(t);
	ScrPrintString(", ");
	printn(m / 1024 / 1024);
	ScrPrintString("MB\n");
	return 1;
}

/**
	@Function:		_ConCmdShowDate
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
_ConCmdShowDate(void)
{
	int8 weeks[][10] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
	struct CMOSDateTime dt;
	CmosGetDateTime(&dt);
	printn(dt.year);
	ScrPrintString("-");
	printn(dt.month);
	ScrPrintString("-");
	printn(dt.day);
	ScrPrintString(" ");
	ScrPrintString(weeks[dt.day_of_week]);
	return 1;
}

/**
	@Function:		_ConCmdShowTime
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
_ConCmdShowTime(IN int32 display_second)
{
	struct CMOSDateTime dt;
	CmosGetDateTime(&dt);
	printn(dt.hour);
	ScrPrintString(":");
	printn(dt.minute);
	if(display_second)
	{
		ScrPrintString(":");
		printn(dt.second);
	}
	return 1;
}

/**
	@Function:		_ConCmdShowDateTime
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
_ConCmdShowDateTime(void)
{
	_ConCmdShowDate();
	ScrPrintString("\n");
	_ConCmdShowTime(1);
	return 1;
}

/**
	@Function:		_ConSetDate
	@Access:		Private
	@Description:
		设置日期。这个版本的函数不会输出错误信息。
	@Parameters:
	@Return:
		BOOL
			返回TRUE则成功，否则失败。	
*/
static
BOOL
_ConSetDate(IN const int8 * str)
{
	if(str == NULL)
		return FALSE;
	uint8 y0, y1, y2, y3;
	uint8 m0, m1;
	uint8 d0, d1;
	y0 = *(str++);
	if(y0 < '0' || y0 > '9')
		return FALSE;
	y1 = *(str++);
	if(y1 < '0' || y1 > '9')
		return FALSE;
	y2 = *(str++);
	if(y2 < '0' || y2 > '9')
		return FALSE;
	y3 = *(str++);
	if(y3 < '0' || y3 > '9')
		return FALSE;
	if(*(str++) != '-')
		return FALSE;
	m0 = *(str++);
	if(m0 < '0' || m0 > '9')
		return FALSE;
	m1 = *(str++);
	if(m1 < '0' || m1 > '9')
		return FALSE;
	if(*(str++) != '-')
		return FALSE;
	d0 = *(str++);
	if(d0 < '0' || d0 > '9')
		return FALSE;
	d1 = *(str++);
	if(d1 < '0' || d1 > '9')
		return FALSE;
	uint16 year;
	uint8 month;
	uint8 day;
	year = 	(y0 - '0') * 1000
			+ (y1 - '0') * 100
			+ (y2 - '0') * 10
			+ (y3 - '0');
	month =	(m0 - '0') * 10 + (m1 - '0');
	day = (d0 - '0') * 10 + (d1 - '0');
	BOOL r = CmosSetDate(year, month, day);
	return r;
}

/**
	@Function:		_ConCmdSetDate
	@Access:		Private
	@Description:
		设置日期。这个版本的函数会输出错误信息。
	@Parameters:
	@Return:
		BOOL
			返回TRUE则成功，否则失败。	
*/
static
BOOL
_ConCmdSetDate(IN const int8 * str)
{
	BOOL r = _ConSetDate(str);
	if(r)
		_ConCmdPrint("OK!");
	else
		_ConCmdError("A error was occured when setting date. The date format is YYYY-MM-DD.");
	return r;
}

/**
	@Function:		_ConSetTime
	@Access:		Private
	@Description:
		设置时间。这个版本的函数不会输出错误信息。
	@Parameters:
	@Return:
		BOOL
			返回TRUE则成功，否则失败。	
*/
static
BOOL
_ConSetTime(IN const int8 * str)
{
	if(str == NULL)
		return FALSE;
	uint8 h0, h1;
	uint8 m0, m1;
	uint8 s0, s1;
	h0 = *(str++);
	if(h0 < '0' || h0 > '9')
		return FALSE;
	h1 = *(str++);
	if(h1 < '0' || h1 > '9')
		return FALSE;
	if(*(str++) != ':')
		return FALSE;
	m0 = *(str++);
	if(m0 < '0' || m0 > '9')
		return FALSE;
	m1 = *(str++);
	if(m1 < '0' || m1 > '9')
		return FALSE;
	if(*(str++) != ':')
		return FALSE;
	s0 = *(str++);
	if(s0 < '0' || s0 > '9')
		return FALSE;
	s1 = *(str++);
	if(s1 < '0' || s1 > '9')
		return FALSE;
	uint8 hour;
	uint8 minute;
	uint8 second;
	hour = (h0 - '0') * 10 + (h1 - '0');
	minute = (m0 - '0') * 10 + (m1 - '0');
	second = (s0 - '0') * 10 + (s1 - '0');
	BOOL r = CmosSetTime(hour, minute, second);
	return r;
}

/**
	@Function:		_ConCmdSetTime
	@Access:		Private
	@Description:
		设置时间。这个版本的函数会输出错误信息。
	@Parameters:
	@Return:
		BOOL
			返回TRUE则成功，否则失败。	
*/
static
BOOL
_ConCmdSetTime(IN const int8 * str)
{
	BOOL r = _ConSetTime(str);
	if(r)
		_ConCmdPrint("OK!");
	else
		_ConCmdError("A error was occured when setting time. The date format is HH-MM-SS.");
	return r;
}

/**
	@Function:		_ConCmdSetClock
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
_ConCmdSetClock(IN int8 * param)
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
		_ConCmdError(FORMAT("clock {on|off}"));
		return 0;
	}
}

/**
	@Function:		_ConCmdFormatDisk
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
_ConCmdFormatDisk(IN int8 * param)
{
	if(strcmp(param, "") == 0)
	{
		_ConCmdError(FORMAT("format {DA|DB|VA|VB|...}"));
		return 0;
	}
	if(Ifs1Format(param))
	{
		ScrPrintString("OK!");
		return 1;
	}
	else
	{
		_ConCmdError("Failed to format disk!");		
		return 0;
	}
}

/**
	@Function:		_ConCmdCheckDisk
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
_ConCmdCheckDisk(IN int8 * param)
{
	if(strcmp(param, "") == 0)
	{
		_ConCmdError(FORMAT("cdisk {DA|DB|VA|VB|...}"));
		return 0;
	}
	ScrPrintString(param);
	if(Ifs1Check(param))
		ScrPrintString(" is IFS1!");
	else
		_ConCmdError(" is not IFS1!");
	return 1;
}

/**
	@Function:		_ConCmdCreateDir
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
_ConCmdCreateDir(	IN int8 * path, 
		IN int8 * name)
{
	if(strcmp(path, "") == 0 || strcmp(name, "") == 0)
	{
		_ConCmdError(FORMAT("mkdir {path} {dir name}"));
		return 0;
	}
	if(!Ifs1IsValidName(name))
	{
		_ConCmdError("Invalid directory name!");
		return 0;
	}
	int8 temp[MAX_PATH_BUFFER_LEN];
	if(!Ifs1GetAbsolutePath(path, current_path, temp))
	{
		_ConCmdError("Invalid path!");
		return 0;
	}
	if(Ifs1CreateDir(temp, name))
	{
		ScrPrintString("OK!");
		return 1;
	}
	else
	{
		_ConCmdError("Failed to create a directory!");
		return 0;
	}
}

/**
	@Function:		_ConCmdCreateFile
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
_ConCmdCreateFile(	IN int8 * path,
					IN int8 * name)
{
	if(strcmp(path, "") == 0 || strcmp(name, "") == 0)
	{
		_ConCmdError(FORMAT("mkfile {path} {file name}"));
		return 0;
	}
	if(!Ifs1IsValidName(name))
	{
		_ConCmdError("Invalid file name!");
		return 0;
	}
	int8 temp[MAX_PATH_BUFFER_LEN];
	if(!Ifs1GetAbsolutePath(path, current_path, temp))
	{
		_ConCmdError("Invalid path!");
		return 0;
	}
	if(Ifs1CreateFile(temp, name))
	{
		ScrPrintString("OK!");
		return 1;
	}
	else
	{
		_ConCmdError("Failed to create a file!");
		return 0;
	}
}

/**
	@Function:		_ConCmdDeleteDir
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
_ConCmdDeleteDir(IN int8 * path)
{
	if(strcmp(path, "") == 0)
	{
		_ConCmdError(FORMAT("deldir {path}"));
		return 0;
	}
	int8 temp[MAX_PATH_BUFFER_LEN];
	if(!Ifs1GetAbsolutePath(path, current_path, temp))
	{
		_ConCmdError("Invalid path!");
		return 0;
	}
	if(!Ifs1IsChildPath(temp, current_path) && Ifs1DeleteDir(temp))
	{
		ScrPrintString("OK!");
		return 1;
	}
	else
	{
		_ConCmdError("Failed to delete a directory!");
		return 0;
	}
}

/**
	@Function:		_ConCmdDeleteDirRecursively
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
_ConCmdDeleteDirRecursively(IN int8 * path)
{
	if(strcmp(path, "") == 0)
	{
		_ConCmdError(FORMAT("deldirs {path}"));
		return 0;
	}
	int8 temp[MAX_PATH_BUFFER_LEN];
	if(!Ifs1GetAbsolutePath(path, current_path, temp))
	{
		_ConCmdError("Invalid path!");
		return 0;
	}
	if(!Ifs1IsChildPath(temp, current_path) && Ifs1DeleteDirRecursively(temp))
	{
		ScrPrintString("OK!");
		return 1;
	}
	else
	{
		_ConCmdError("Failed to delete directorys!");
		return 0;
	}
}

/**
	@Function:		_ConCmdDeleteFile
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
_ConCmdDeleteFile(IN int8 * path)
{
	if(strcmp(path, "") == 0)
	{
		_ConCmdError(FORMAT("delfile {path}"));
		return 0;
	}
	int8 temp[MAX_PATH_BUFFER_LEN];
	if(!Ifs1GetAbsolutePath(path, current_path, temp))
	{
		_ConCmdError("Invalid path!");
		return 0;
	}
	if(Ifs1DeleteFile(temp))
	{
		ScrPrintString("OK!");
		return 1;
	}
	else
	{
		_ConCmdError("Failed to delete a file!");
		return 0;
	}
}

/**
	@Function:		_ConCmdRenameDir
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
_ConCmdRenameDir(	IN int8 * path, 
					IN int8 * new_name)
{
	if(strcmp(path, "") == 0 || strcmp(new_name, "") == 0)
	{
		_ConCmdError(FORMAT("rndir {path} {new name}"));
		return 0;
	}
	int8 temp[MAX_PATH_BUFFER_LEN];
	if(!Ifs1GetAbsolutePath(path, current_path, temp))
	{
		_ConCmdError("Invalid path!");
		return 0;
	}
	if(!Ifs1IsChildPath(temp, current_path) && Ifs1RenameDir(temp, new_name))
	{
		ScrPrintString("OK!");
		return 1;
	}
	else
	{
		_ConCmdError("Failed to rename directory!");
		return 0;
	}
}

/**
	@Function:		_ConCmdRenameFile
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
_ConCmdRenameFile(	IN int8 * path, 
					IN int8 * new_name)
{
	if(strcmp(path, "") == 0 || strcmp(new_name, "") == 0)
	{
		_ConCmdError(FORMAT("rnfiler {path} {new name}"));
		return 0;
	}
	int8 temp[MAX_PATH_BUFFER_LEN];
	if(!Ifs1GetAbsolutePath(path, current_path, temp))
	{
		_ConCmdError("Invalid path!");
		return 0;
	}
	if(Ifs1RenameFile(temp, new_name))
	{
		ScrPrintString("OK!");
		return 1;
	}
	else
	{
		_ConCmdError("Failed to rename file!");
		return 0;
	}
}

/**
	@Function:		_ConCmdShowFileInfo
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
_ConCmdShowFileInfo(IN int8 * path, 
					IN int32 simple, 
					IN int32 part)
{
	if(strcmp(path, "") == 0)
		path = current_path;
	int8 temp[MAX_PATH_BUFFER_LEN];
	if(!Ifs1GetAbsolutePath(path, current_path, temp))
	{
		_ConCmdError("Invalid path!");
		return 0;
	}
	path = temp;
	uint ui, ui1;
	DSLLinkedList * list = dsl_lnklst_new();
	int32 count = Ifs1GetItemList(path, list);
	if(count == -1)
	{
		_ConCmdError("Invalid path!");
		return 0;
	}
	for(ui = 0; ui < count; ui++)
	{
		if(part && ui != 0 && ui % 20 == 0)
		{
			ScrPrintString("Press any key to continue!\n");
			if(get_char() == KEY_EXT)
				get_char();
		}
		if(simple)
		{
			struct RawBlock * block = dsl_lnklst_get(list, ui)->value.value.object_value;
			if(block->type == BLOCK_TYPE_FILE)
			{
				struct FileBlock * file = (struct FileBlock *)block;
				ScrPrintStringWithProperty(file->filename, CC_YELLOW);
			}
			else if(block->type == BLOCK_TYPE_DIR)
			{
				struct DirBlock * dir = (struct DirBlock *)block;
				ScrPrintStringWithProperty(dir->dirname, CC_GREEN);
			}
			else if(block->type == BLOCK_TYPE_SLINK)
			{
				struct SLinkBlock * slink = (struct SLinkBlock *)block;
				ScrPrintStringWithProperty(slink->filename, CC_BLUE);
			}
			else
				ScrPrintStringWithProperty("????", CC_RED);
			ScrPrintString(" ");
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
				ScrPrintStringWithProperty(name, CC_YELLOW);
				ScrPrintString("       ");
				ScrPrintDateTime(&(file->change));
				ScrPrintString(" ");
				printn(file->length);
				ScrPrintString("Bytes");
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
				ScrPrintStringWithProperty(name, CC_GREEN);
				ScrPrintString(" <DIR> ");
				ScrPrintDateTime(&(dir->change));
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
				ScrPrintStringWithProperty(name, CC_BLUE);
				ScrPrintString(" <LNK> ");
			}
			else
				ScrPrintStringWithProperty("????", CC_RED);
			ScrPrintString("\n");
		}
	}
	dsl_lnklst_delete_all_object_node(list);
	MemFree(list);
	return 1;
}

/**
	@Function:		_ConCmdChangeDir
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
_ConCmdChangeDir(IN int8 * path)
{
	int8 temp[MAX_PATH_BUFFER_LEN];
	if(!Ifs1GetAbsolutePath(path, current_path, temp))
	{
		_ConCmdError("Invalid path!");
		return 0;
	}
	int32 type;
	int8 symbol[3];
	uint id = Ifs1ParsePath(temp, symbol, &type);
	struct DirBlock dir;
	if(	id == 0xFFFFFFFF
		|| !Ifs1GetBlock(symbol, id, (struct RawBlock *)&dir) 
		|| dir.used == 0 
		|| dir.type != BLOCK_TYPE_DIR)
	{
		_ConCmdError("Invalid path!");
		return 0;
	}
	UtlCopyString(current_path, sizeof(current_path), temp);
	return 1;
}

/**
	@Function:		_ConCmdCopyFile
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
_ConCmdCopyFile(IN int8 * src_path, 
				IN int8 * dst_path, 
				IN int8 * name)
{
	int8 temp[MAX_PATH_BUFFER_LEN];
	int8 temp1[MAX_PATH_BUFFER_LEN];
	if(!Ifs1GetAbsolutePath(src_path, current_path, temp))
	{
		_ConCmdError("Invalid path!");
		return 0;
	}
	if(!Ifs1GetAbsolutePath(dst_path, current_path, temp1))
	{
		_ConCmdError("Invalid path!");
		return 0;
	}
	if(Ifs1CopyFile(temp, temp1, name))
	{
		ScrPrintString("OK!");
		return 1;
	}
	else
	{
		_ConCmdError("Failed to copy a file");
		return 0;
	}
}

/**
	@Function:		_ConCmdKillTask
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
_ConCmdKillTask(IN int8 * tid_s)
{
	if(strcmp(tid_s, "") == 0)
	{
		_ConCmdError(FORMAT("kill {task id}"));
		return 0;
	}
	if(!is_valid_uint(tid_s))
	{
		_ConCmdError("Task id must unsigned integer!");
		return 0;
	}
	int32 tid = (int32)stol(tid_s);
	if(TskmgrKillTask(tid))
	{
		ScrPrintString("OK!");
		return 1;
	}
	else
	{
		_ConCmdError("Failed to kill a task!");
		return 0;
	}
}

/**
	@Function:		_ConCmdShowTaskInfo
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
_ConCmdShowTaskInfo(IN int32 part)
{
	int32 i;
	uint32 ui;
	for(i = 0; i < MAX_TASK_COUNT; i++)
	{
		if(part && i != 0 && i % 20 == 0)
		{
			ScrPrintString("Press any key to continue!\n");
			if(get_char() == KEY_EXT)
				get_char();
		}
		struct Task task;
		if(TskmgrGetTaskInfo(i, &task))
		{
			int8 tid_s[30];
			ltos(tid_s, i);
			ScrPrintStringWithProperty(tid_s, CC_YELLOW);
			for(ui = 0; ui < 6 - strlen(tid_s); ui++)
				ScrPrintString(" ");
			printn(task.used_memory_size / 1024 / 1024);
			ScrPrintString("MB, ");
			ScrPrintString(task.name);
			ScrPrintString("\n");
		}
	}
	return 1;
}

/**
	@Function:		_ConCmdExecuteApplication
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
_ConCmdExecuteApplication(	IN int8 * path, 
							IN int8 * cmd)
{
	if(strcmp(path, "") == 0)
	{
		_ConCmdError(FORMAT("run {path}"));
		return -1;
	}
	int32 r;
	int8 temp[MAX_PATH_BUFFER_LEN];
	if(!Ifs1GetAbsolutePath(path, current_path, temp))
	{
		_ConCmdError("Invalid path!");
		return -1;
	}
	if((r = TskmgrCreateTaskByFile(temp, cmd, current_path)) == -1)
		_ConCmdError("Failed to run application!");
	else
		TskmgrSetTaskToReady(r);
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
			IN int8 * cmd,
			IN int8 * stdin,
			IN int8 * stdout,
			IN int8 * stderr)
{
	if(strcmp(path, "") == 0)
	{
		_ConCmdError(FORMAT("{application path}"));
		return -1;
	}
	int32 r = -1;
	int8 temp[MAX_PATH_BUFFER_LEN];
	if(!Ifs1GetAbsolutePath(path, current_path, temp))
	{
		_ConCmdError("Invalid path!");
		return -1;
	}

	//在这里调用 lock 是为了确保 wait_app_tid = r; 的执行。
	//如果在 wait_app_tid = r; 被执行之前内核任务被切换到新的任务，
	//并且新的任务在被切换之前调用了 app_exit，新的任务将不能正常的
	//退出。因为 wait_app_tid 此时等于-1。
	lock();
	if((r = TskmgrCreateTaskByFile(temp, cmd, current_path)) == -1)
	{
		unlock();
		_ConCmdError("Failed to run application!");
		return -1;
	}
	if(strcmp(stdin, "%") != 0)
		TaskmgrRedirectStdin(r, stdin);
	if(strcmp(stdout, "%") != 0)
		TaskmgrRedirectStdout(r, stdout);
	if(strcmp(stderr, "%") != 0)
		TaskmgrRedirectStderr(r, stderr);
	wait_app_tid = r;
	unlock();
	TskmgrSetTaskToReady(r);
	return r;
}

/**
	@Function:		_ConCmdShowHelpInfo
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
_ConCmdShowHelpInfo(void)
{
	FileObject * fptr = Ifs1OpenFile(SYSTEM_HELP_FILE, FILE_MODE_READ);
	if(fptr == NULL)
	{
		_ConCmdError("Cannot open file '"SYSTEM_HELP_FILE"'!");
		return 0;
	}
	int8 * text = (char *)MemAlloc(flen(fptr) + 1);
	if(text == NULL)
	{
		_ConCmdError("No enough memory!");
		Ifs1CloseFile(fptr);
		return 0;
	}
	text[flen(fptr)] = '\0';
	if(!Ifs1ReadFile(fptr, text, flen(fptr)))
	{
		_ConCmdError("Cannot read file '"SYSTEM_HELP_FILE"'!");
		Ifs1CloseFile(fptr);
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
				ScrPrintString("Press Enter to continue, Q to quit!\n");
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
		ScrPrintChar(chr);
		if(chr == '\n')
			line++;
	}
	Ifs1CloseFile(fptr);
	return 1;
}

/**
	@Function:		_ConCmdReboot
	@Access:		Private
	@Description:
		重启计算机。
	@Parameters:
	@Return:
*/
static
void
_ConCmdReboot(void)
{
	ScrPrintString("Killing all tasks...\n");
	//
	// !!!如果该函数在除了内核任务的其他任务的情况下被调用将会导致调用该函数的任务被杀死，这会引发问题!!!
	//
	// TskmgrKillAllTasks();
	ScrPrintString("OK!");
	KnlRebootSystem();
}

/**
	@Function:		_ConCmdShutdown
	@Access:		Private
	@Description:
		关闭计算机。
	@Parameters:
	@Return:
*/
static
void
_ConCmdShutdown(void)
{
	ScrPrintString("Killing all tasks...\n");
	//
	// !!!如果该函数在除了内核任务的其他任务的情况下被调用将会导致调用该函数的任务被杀死，这会引发问题!!!
	//
	// TskmgrKillAllTasks();
	ScrPrintString("OK!");
	KnlShutdownSystem();
}

/**
	@Function:		_ConCmdChangeVideoMode
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
_ConCmdChangeVideoMode(IN int8 * mode)
{
	FileObject * fptr = NULL;
	if(strcmp(mode, "text") == 0)
		fptr = Ifs1OpenFile(SYSTEM_PATH"kernelldrs/kernelldrtm.bin", FILE_MODE_READ);
	else if(strcmp(mode, "vesa320_200") == 0)
		fptr = Ifs1OpenFile(SYSTEM_PATH"kernelldrs/kernelldrgm_320_200.bin", FILE_MODE_READ);
	else if(strcmp(mode, "vesa640_400") == 0)
		fptr = Ifs1OpenFile(SYSTEM_PATH"kernelldrs/kernelldrgm_640_400.bin", FILE_MODE_READ);
	else if(strcmp(mode, "vesa640_480") == 0)
		fptr = Ifs1OpenFile(SYSTEM_PATH"kernelldrs/kernelldrgm_640_480.bin", FILE_MODE_READ);
	else if(strcmp(mode, "vesa800_500") == 0)
		fptr = Ifs1OpenFile(SYSTEM_PATH"kernelldrs/kernelldrgm_800_500.bin", FILE_MODE_READ);
	else if(strcmp(mode, "vesa800_600") == 0)
		fptr = Ifs1OpenFile(SYSTEM_PATH"kernelldrs/kernelldrgm_800_600.bin", FILE_MODE_READ);
	else if(strcmp(mode, "vesa896_672") == 0)
		fptr = Ifs1OpenFile(SYSTEM_PATH"kernelldrs/kernelldrgm_896_672.bin", FILE_MODE_READ);
	else if(strcmp(mode, "vesa1024_640") == 0)
		fptr = Ifs1OpenFile(SYSTEM_PATH"kernelldrs/kernelldrgm_1024_640.bin", FILE_MODE_READ);
	else if(strcmp(mode, "vesa1024_768") == 0)
		fptr = Ifs1OpenFile(SYSTEM_PATH"kernelldrs/kernelldrgm_1024_768.bin", FILE_MODE_READ);
	else if(strcmp(mode, "vesa1152_720") == 0)
		fptr = Ifs1OpenFile(SYSTEM_PATH"kernelldrs/kernelldrgm_1152_720.bin", FILE_MODE_READ);
	else if(strcmp(mode, "vesa1280_1024") == 0)
		fptr = Ifs1OpenFile(SYSTEM_PATH"kernelldrs/kernelldrgm_1280_1024.bin", FILE_MODE_READ);
	else if(strcmp(mode, "vesa1440_900") == 0)
		fptr = Ifs1OpenFile(SYSTEM_PATH"kernelldrs/kernelldrgm_1440_900.bin", FILE_MODE_READ);
	else if(strcmp(mode, "vesa1600_1200") == 0)
		fptr = Ifs1OpenFile(SYSTEM_PATH"kernelldrs/kernelldrgm_1600_1200.bin", FILE_MODE_READ);
	else
	{
		_ConCmdError(FORMAT(	"vmode {text"
						"|vesa320_200|vesa640_400|vesa640_480|vesa800_500"
						"|vesa800_600|vesa896_672|vesa1024_640|vesa1024_768"
						"|vesa1152_720|vesa1280_1024|vesa1440_900|vesa1600_1200}"));
		return 0;
	}
	if(fptr == NULL)
	{
		_ConCmdError("Failed to change video mode!");
		return 0;
	}
	uint8 * buffer = MemAlloc((flen(fptr) / 512 + 1) * 512);
	uint r;
	if(buffer == NULL || !(r = Ifs1ReadFile(fptr, buffer, flen(fptr))) )
	{
		_ConCmdError("Failed to change video mode!");
		Ifs1CloseFile(fptr);
		return 0;
	}
	Ifs1CloseFile(fptr);
	if(!DskWriteSectors(SYSTEM_DISK, 1, flen(fptr) / 512 + 1, buffer))
	{
		_ConCmdError("Failed to change video mode!");
		MemFree(buffer);
		return 0;
	}
	else
	{
		ScrPrintString("OK!");
		MemFree(buffer);
		return 1;
	}
}

/**
	@Function:		_ConCmdShowCpuInfo
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
_ConCmdShowCpuInfo(void)
{
	int8 vendor_id_string[13];
	int8 brand_string[49];
	CpuGetVendorIdString(vendor_id_string);
	ScrPrintStringWithProperty("Vendor ID String: ", CC_YELLOW);
	ScrPrintString(vendor_id_string);
	ScrPrintString("\n");
	CpuGetBrandString(brand_string);	
	trim(brand_string);
	ScrPrintStringWithProperty("Brand String: ", CC_YELLOW);
	ScrPrintString(brand_string);
	ScrPrintString("\n");
	struct CacheInfo L1;
	if(CpuGetL1(&L1))
	{
		ScrPrintStringWithProperty("L1: ", CC_YELLOW);
		printn(L1.size);
		ScrPrintString(", ");
		printn(L1.way);
		ScrPrintString(", ");
		printn(L1.linesize);
		ScrPrintString("\n");
	}
	struct CacheInfo L2;
	if(CpuGetL2(&L2))
	{
		ScrPrintStringWithProperty("L2: ", CC_YELLOW);
		printn(L2.size);
		ScrPrintString(", ");
		printn(L2.way);
		ScrPrintString(", ");
		printn(L2.linesize);
		ScrPrintString("\n");
	}
	struct CacheInfo L3;
	if(CpuGetL3(&L3))
	{
		ScrPrintStringWithProperty("L3: ", CC_YELLOW);
		printn(L3.size);
		ScrPrintString(", ");
		printn(L3.way);
		ScrPrintString(", ");
		printn(L3.linesize);
		ScrPrintString("\n");
	}
	ScrPrintStringWithProperty("Features: ", CC_YELLOW);

	ScrPrintStringWithProperty("SSE3 ", CPU_FEATURE_SSE3() ? CC_GREEN : CC_RED);
	ScrPrintStringWithProperty("PCLMUL ", CPU_FEATURE_PCLMUL() ? CC_GREEN : CC_RED);
	ScrPrintStringWithProperty("DTES64 ", CPU_FEATURE_DTES64() ? CC_GREEN : CC_RED);
	ScrPrintStringWithProperty("MONITOR ", CPU_FEATURE_MONITOR() ? CC_GREEN : CC_RED);
	ScrPrintStringWithProperty("DS-CPL ", CPU_FEATURE_DS_CPL() ? CC_GREEN : CC_RED);
	ScrPrintStringWithProperty("VMX ", CPU_FEATURE_VMX() ? CC_GREEN : CC_RED);
	ScrPrintStringWithProperty("SMX ", CPU_FEATURE_SMX() ? CC_GREEN : CC_RED);
	ScrPrintStringWithProperty("EST ", CPU_FEATURE_EST() ? CC_GREEN : CC_RED);
	ScrPrintStringWithProperty("TM2 ", CPU_FEATURE_TM2() ? CC_GREEN : CC_RED);
	ScrPrintStringWithProperty("SSSE3 ", CPU_FEATURE_SSSE3() ? CC_GREEN : CC_RED);
	ScrPrintStringWithProperty("CID ", CPU_FEATURE_CID() ? CC_GREEN : CC_RED);
	ScrPrintStringWithProperty("FMA ", CPU_FEATURE_FMA() ? CC_GREEN : CC_RED);
	ScrPrintStringWithProperty("CX16 ", CPU_FEATURE_CX16() ? CC_GREEN : CC_RED);
	ScrPrintStringWithProperty("ETPRD ", CPU_FEATURE_ETPRD() ? CC_GREEN : CC_RED);
	ScrPrintStringWithProperty("PDCM ", CPU_FEATURE_PDCM() ? CC_GREEN : CC_RED);
	ScrPrintStringWithProperty("PCID ", CPU_FEATURE_PCID() ? CC_GREEN : CC_RED);
	ScrPrintStringWithProperty("DCA ", CPU_FEATURE_DCA() ? CC_GREEN : CC_RED);
	ScrPrintStringWithProperty("SSE4.1 ", CPU_FEATURE_SSE4_1() ? CC_GREEN : CC_RED);
	ScrPrintStringWithProperty("SSE4.2 ", CPU_FEATURE_SSE4_2() ? CC_GREEN : CC_RED);
	ScrPrintStringWithProperty("X2APIC ", CPU_FEATURE_X2APIC() ? CC_GREEN : CC_RED);
	ScrPrintStringWithProperty("MOVBE ", CPU_FEATURE_MOVBE() ? CC_GREEN : CC_RED);
	ScrPrintStringWithProperty("POPCNT ", CPU_FEATURE_POPCNT() ? CC_GREEN : CC_RED);
	ScrPrintStringWithProperty("TSC-Deadline ", CPU_FEATURE_TSC_DEADLINE() ? CC_GREEN : CC_RED);
	ScrPrintStringWithProperty("AES ", CPU_FEATURE_AES() ? CC_GREEN : CC_RED);
	ScrPrintStringWithProperty("XSAVE ", CPU_FEATURE_XSAVE() ? CC_GREEN : CC_RED);
	ScrPrintStringWithProperty("OSXSAVE ", CPU_FEATURE_OSXSAVE() ? CC_GREEN : CC_RED);
	ScrPrintStringWithProperty("AVX ", CPU_FEATURE_AVX() ? CC_GREEN : CC_RED);
	ScrPrintStringWithProperty("F16C ", CPU_FEATURE_F16C() ? CC_GREEN : CC_RED);
	ScrPrintStringWithProperty("RDRAND ", CPU_FEATURE_RDRAND() ? CC_GREEN : CC_RED);

	ScrPrintStringWithProperty("FPU ", CPU_FEATURE_FPU() ? CC_GREEN : CC_RED);
	ScrPrintStringWithProperty("VME ", CPU_FEATURE_VME() ? CC_GREEN : CC_RED);
	ScrPrintStringWithProperty("DE ", CPU_FEATURE_DE() ? CC_GREEN : CC_RED);
	ScrPrintStringWithProperty("PSE ", CPU_FEATURE_PSE() ? CC_GREEN : CC_RED);
	ScrPrintStringWithProperty("TSC ", CPU_FEATURE_TSC() ? CC_GREEN : CC_RED);
	ScrPrintStringWithProperty("MSR ", CPU_FEATURE_MSR() ? CC_GREEN : CC_RED);
	ScrPrintStringWithProperty("PAE ", CPU_FEATURE_PAE() ? CC_GREEN : CC_RED);
	ScrPrintStringWithProperty("MCE ", CPU_FEATURE_MCE() ? CC_GREEN : CC_RED);
	ScrPrintStringWithProperty("CXB ", CPU_FEATURE_CXB() ? CC_GREEN : CC_RED);
	ScrPrintStringWithProperty("APIC ", CPU_FEATURE_APIC() ? CC_GREEN : CC_RED);
	ScrPrintStringWithProperty("SEP ", CPU_FEATURE_SEP() ? CC_GREEN : CC_RED);
	ScrPrintStringWithProperty("MTRR ", CPU_FEATURE_MTRR() ? CC_GREEN : CC_RED);
	ScrPrintStringWithProperty("PGE ", CPU_FEATURE_PGE() ? CC_GREEN : CC_RED);
	ScrPrintStringWithProperty("MCA ", CPU_FEATURE_MCA() ? CC_GREEN : CC_RED);
	ScrPrintStringWithProperty("CMOV ", CPU_FEATURE_CMOV() ? CC_GREEN : CC_RED);
	ScrPrintStringWithProperty("PAT ", CPU_FEATURE_PAT() ? CC_GREEN : CC_RED);
	ScrPrintStringWithProperty("PSE-36 ", CPU_FEATURE_PSE36() ? CC_GREEN : CC_RED);
	ScrPrintStringWithProperty("PN ", CPU_FEATURE_PN() ? CC_GREEN : CC_RED);
	ScrPrintStringWithProperty("CLF ", CPU_FEATURE_CLF() ? CC_GREEN : CC_RED);
	ScrPrintStringWithProperty("DTES ", CPU_FEATURE_DTES() ? CC_GREEN : CC_RED);
	ScrPrintStringWithProperty("ACPI ", CPU_FEATURE_ACPI() ? CC_GREEN : CC_RED);
	ScrPrintStringWithProperty("MMX ", CPU_FEATURE_MMX() ? CC_GREEN : CC_RED);
	ScrPrintStringWithProperty("FXSR ", CPU_FEATURE_FXSR() ? CC_GREEN : CC_RED);
	ScrPrintStringWithProperty("SSE ", CPU_FEATURE_SSE() ? CC_GREEN : CC_RED);
	ScrPrintStringWithProperty("SSE2 ", CPU_FEATURE_SSE2() ? CC_GREEN : CC_RED);
	ScrPrintStringWithProperty("SS ", CPU_FEATURE_SS() ? CC_GREEN : CC_RED);
	ScrPrintStringWithProperty("HTT ", CPU_FEATURE_HTT() ? CC_GREEN : CC_RED);
	ScrPrintStringWithProperty("TM1 ", CPU_FEATURE_TM1() ? CC_GREEN : CC_RED);
	ScrPrintStringWithProperty("PBE", CPU_FEATURE_PBE() ? CC_GREEN : CC_RED);
	return 1;
}

/**
	@Function:		_ConCmdSetProperty
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
_ConCmdSetProperty(	IN int8 * property, 
					IN int8 * value)
{
	if(strcmp(property, "cursor_color") == 0)
		ScrSetConsoleCursorColor((uchar)stoi(value));
	else if(strcmp(property, "char_color") == 0)
		ScrSetConsoleCharColor((uchar)stoi(value));
	else
	{
		_ConCmdError("Failed to set property!");
		return 0;
	}
	ScrPrintString("OK!");
	return 1;
}

/**
	@Function:		_ConCmdCreateSLink
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
_ConCmdCreateSLink(	IN int8 * path,
					IN int8 * name,
					IN int8 * link)
{
	if(strcmp(path, "") == 0 || strcmp(name, "") == 0 || strcmp(link, "") == 0)
	{
		_ConCmdError(FORMAT("mkslink {path} {name} {link}"));
		return FALSE;
	}
	int8 temp[MAX_PATH_BUFFER_LEN];
	if(!Ifs1GetAbsolutePath(path, current_path, temp))
	{
		_ConCmdError("Invalid path!");
		return FALSE;
	}
	if(Ifs1CreateSLink(temp, name, link))
	{
		ScrPrintString("OK!");
		return TRUE;
	}
	else
	{
		_ConCmdError("Failed to create a soft link!");
		return FALSE;
	}
}

/**
	@Function:		_ConCmdDeleteSLink
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
_ConCmdDeleteSLink(IN int8 * path)
{
	if(strcmp(path, "") == 0)
	{
		_ConCmdError(FORMAT("delslink {path}"));
		return FALSE;
	}
	int8 temp[MAX_PATH_BUFFER_LEN];
	if(!Ifs1GetAbsolutePath(path, current_path, temp))
	{
		_ConCmdError("Invalid path!");
		return FALSE;
	}
	if(Ifs1DeleteSLink(temp))
	{
		ScrPrintString("OK!");
		return TRUE;
	}
	else
	{
		_ConCmdError("Failed to delete a soft link!");
		return FALSE;
	}
}

/**
	@Function:		_ConCmdShowSLinkTarget
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
_ConCmdShowSLinkTarget(IN int8 * path)
{
	if(strcmp(path, "") == 0)
	{
		_ConCmdError(FORMAT("slink {path}"));
		return FALSE;
	}
	int8 temp[MAX_PATH_BUFFER_LEN];
	if(!Ifs1GetAbsolutePath(path, current_path, temp))
	{
		_ConCmdError("Invalid path!");
		return FALSE;
	}
	int8 link[1024];
	link[1023] = '\0';
	if(Ifs1GetSLinkTarget(temp, 1023, link))
	{
		ScrPrintString(link);
		return TRUE;
	}
	else
	{
		_ConCmdError("Failed to show link target of soft link!");
		return FALSE;
	}
}

static int32 _ConExecuteCommand(int8 * cmd, int8 * lines, uint32 * pointer, uint32 line, struct Vars * local_vars_s);

#define	BATCH_MAX_CMD_LEN		1024	//脚本的命令缓冲区的最大长度
#define	BATCH_MAX_CMD_LINES		1024	//脚本的命令的最大行数
#define	BATCH_MAX_VARS_COUNT	1024	//脚本的变量的最大数量

/**
	@Function:		__ConExecuteBatchScript
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
__ConExecuteBatchScript(IN int8 * path)
{
	int8 temp[MAX_PATH_BUFFER_LEN];
	if(!Ifs1GetAbsolutePath(path, current_path, temp))
	{
		_ConCmdError("Invalid path!");
		return 0;
	}
	struct Vars * local_vars_s;
	local_vars_s = KnlAllocVarList(BATCH_MAX_VARS_COUNT);
	if(local_vars_s == NULL)
		return 0;
	FileObject * fptr = Ifs1OpenFile(temp, FILE_MODE_READ);
	if(fptr == NULL)
	{
		KnlFreeVarList(local_vars_s);
		return 0;
	}
	uint32 file_len = flen(fptr);
	int8 * cmds = (int8 *)MemAlloc(file_len);
	if(cmds == NULL)
	{
		Ifs1CloseFile(fptr);
		KnlFreeVarList(local_vars_s);
		return 0;
	}
	Ifs1ReadFile(fptr, cmds, file_len);
	Ifs1CloseFile(fptr);
	uint32 ui;
	int8 cmd[BATCH_MAX_CMD_LEN];
	int8 * lines = MemAlloc(BATCH_MAX_CMD_LINES * BATCH_MAX_CMD_LEN);
	if(lines == NULL)
	{
		MemFree(cmds);
		KnlFreeVarList(local_vars_s);
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
			UtlCopyString(lines + line++ * BATCH_MAX_CMD_LEN,
						BATCH_MAX_CMD_LEN,
						cmd);
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
		// 检测是否退出批处理。
		if(_exit_batch)
		{
			_exit_batch = FALSE;
			break;
		}

		int8 * current_cmd = lines + pointer * BATCH_MAX_CMD_LEN;
		if(!_ConExecuteCommand(current_cmd, lines, &pointer, line, local_vars_s))
		{
			ScrPrintStringWithProperty("\nBatch have error. Command:\n\t", CC_RED);
			ScrPrintStringWithProperty(current_cmd, CC_RED);
			ScrPrintString("\n");
			MemFree(cmds);
			MemFree(lines);
			KnlFreeVarList(local_vars_s);
			return 0;
		}

		// 检测是否退出批处理。
		if(_exit_batch)
		{
			_exit_batch = FALSE;
			break;
		}
	}
	MemFree(cmds);
	MemFree(lines);
	KnlFreeVarList(local_vars_s);
	return 1;
}

/**
	@Function:		_ConExecuteBatchScript
	@Access:		Private
	@Description:
		运行脚本。
	@Parameters:
		path, int8 *, IN
			脚本文件的路径。
			这个函数会设置_run_in_batch全局静态变量。
	@Return:
		int32
			返回0则失败，否则成功。
*/
static
int32
_ConExecuteBatchScript(IN int8 * path)
{
	_run_in_batch = TRUE;
	int32 r = __ConExecuteBatchScript(path);
	_run_in_batch = FALSE;
	return r;
}

/**
	@Function:		_ConExecuteCommand
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
_ConExecuteCommand(	IN int8 * cmd, 
					IN int8 * lines, 
					IN OUT uint32 * pointer, 
					IN uint32 line, 
					IN struct Vars * local_vars_s)
{
	int32 r = 1;
	int8 word[1024];
	_PARSE_COMMAND(cmd, word, 1023);
	if(_PARSE_COMMAND(NULL, word, 1023) != 0 || strlen(word) != 0)
	{
		int8 * name = word;
		if(strcmp(name, "ver") == 0)
			_ConCmdVer();
		else if(strcmp(name, "print") == 0)
			if(_PARSE_COMMAND(NULL, word, 1023) != 0 || strlen(word) != 0)
			{
				_ConCmdPrint(word);
				r = 1;
				ScrPrintString("\n");
			}
			else
			{
				_ConCmdError(FORMAT("print {message}"));
				r = 0;			
			}
		else if(strcmp(name, "error") == 0)
		{
			_PARSE_COMMAND(NULL, word, 1023);
			_ConCmdError(word);
			r = 1;
		}
		else if(strcmp(name, "disks") == 0)
		{
			r = _ConCmdShowDiskInfo();
			ScrPrintString("\n");
		}
		else if(strcmp(name, "mm") == 0)
		{
			r = _ConCmdShowMemoryInfo();
			ScrPrintString("\n");
		}
		else if(strcmp(name, "date") == 0)
		{
			r = _ConCmdShowDate();
			ScrPrintString("\n");
		}
		else if(strcmp(name, "time") == 0)
		{
			r = _ConCmdShowTime(1);
			ScrPrintString("\n");
		}
		else if(strcmp(name, "dt") == 0)
		{
			r = _ConCmdShowDateTime();
			ScrPrintString("\n");
		}
		else if(strcmp(name, "setdate") == 0)
		{
			_PARSE_COMMAND(NULL, word, 1023);
			r = _ConCmdSetDate(word);
			ScrPrintString("\n");
		}
		else if(strcmp(name, "settime") == 0)
		{
			_PARSE_COMMAND(NULL, word, 1023);
			r = _ConCmdSetTime(word);
			ScrPrintString("\n");
		}
		else if(strcmp(name, "clock") == 0)
		{
			_PARSE_COMMAND(NULL, word, 1023);
			r = _ConCmdSetClock(word);
			ScrPrintString("\n");
		}
		else if(strcmp(name, "format") == 0)
		{
			_PARSE_COMMAND(NULL, word, 1023);
			trim(word);
			r = _ConCmdFormatDisk(word);
			ScrPrintString("\n");
		}
		else if(strcmp(name, "cdisk") == 0)
		{
			_PARSE_COMMAND(NULL, word, 1023);
			trim(word);
			r = _ConCmdCheckDisk(word);
			ScrPrintString("\n");
		}
		else if(strcmp(name, "mkdir") == 0)
		{
			int8 path[MAX_PATH_BUFFER_LEN];
			int8 name[MAX_DIRNAME_BUFFER_LEN];
			_PARSE_COMMAND(NULL, path, MAX_PATH_LEN);
			_PARSE_COMMAND(NULL, name, MAX_DIRNAME_LEN);
			trim(path);
			trim(name);
			r = _ConCmdCreateDir(path, name);
			ScrPrintString("\n");
		}
		else if(strcmp(name, "mkfile") == 0)
		{
			int8 path[MAX_PATH_BUFFER_LEN];
			int8 name[MAX_FILENAME_BUFFER_LEN];
			_PARSE_COMMAND(NULL, path, MAX_PATH_LEN);
			_PARSE_COMMAND(NULL, name, MAX_FILENAME_LEN);
			trim(path);
			trim(name);
			r = _ConCmdCreateFile(path, name);
			ScrPrintString("\n");
		}
		else if(strcmp(name, "deldir") == 0)
		{
			int8 path[MAX_PATH_BUFFER_LEN];
			_PARSE_COMMAND(NULL, path, MAX_PATH_LEN);
			trim(path);
			r = _ConCmdDeleteDir(path);
			ScrPrintString("\n");
		}
		else if(strcmp(name, "deldirs") == 0)
		{
			int8 path[MAX_PATH_BUFFER_LEN];
			_PARSE_COMMAND(NULL, path, MAX_PATH_LEN);
			trim(path);
			r = _ConCmdDeleteDirRecursively(path);
			ScrPrintString("\n");
		}
		else if(strcmp(name, "delfile") == 0)
		{
			int8 path[MAX_PATH_BUFFER_LEN];
			_PARSE_COMMAND(NULL, path, MAX_PATH_LEN);
			trim(path);
			r = _ConCmdDeleteFile(path);
			ScrPrintString("\n");
		}
		else if(strcmp(name, "rndir") == 0)
		{
			int8 path[MAX_PATH_BUFFER_LEN];
			int8 new_name[MAX_DIRNAME_BUFFER_LEN];
			_PARSE_COMMAND(NULL, path, MAX_PATH_LEN);
			_PARSE_COMMAND(NULL, new_name, MAX_DIRNAME_LEN);
			trim(path);
			trim(new_name);
			r = _ConCmdRenameDir(path, new_name);
			ScrPrintString("\n");
		}
		else if(strcmp(name, "rnfile") == 0)
		{
			int8 path[MAX_PATH_BUFFER_LEN];
			int8 new_name[MAX_FILENAME_BUFFER_LEN];
			_PARSE_COMMAND(NULL, path, MAX_PATH_LEN);
			_PARSE_COMMAND(NULL, new_name, MAX_FILENAME_LEN);
			trim(path);
			trim(new_name);
			r = _ConCmdRenameFile(path, new_name);
			ScrPrintString("\n");
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
				_PARSE_COMMAND(NULL, word, 1023);
				if(strcmp(word, "-d") == 0)
				{
					_PARSE_COMMAND(NULL, path, MAX_PATH_LEN);
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
					_ConCmdError("Invalid option!");
					err = 1;
					r = 0;
					break;
				}
			}
			if(!err)
				r = _ConCmdShowFileInfo(path, simple, part);
			ScrPrintString("\n");
		}
		else if(strcmp(name, "cd") == 0)
		{
			int8 path[MAX_PATH_BUFFER_LEN];
			_PARSE_COMMAND(NULL, path, MAX_PATH_LEN);
			trim(path);
			r = _ConCmdChangeDir(path);
			ScrPrintString("\n");
		}
		else if(strcmp(name, "cpfile") == 0)
		{
			int8 src_path[MAX_PATH_BUFFER_LEN];
			int8 dst_path[MAX_PATH_BUFFER_LEN];
			int8 dst_name[MAX_FILENAME_BUFFER_LEN];
			_PARSE_COMMAND(NULL, src_path, MAX_PATH_LEN);
			_PARSE_COMMAND(NULL, dst_path, MAX_PATH_LEN);
			_PARSE_COMMAND(NULL, dst_name, MAX_FILENAME_LEN);
			r = _ConCmdCopyFile(src_path, dst_path, dst_name);
			ScrPrintString("\n");
		}
		else if(strcmp(name, "kill") == 0)
		{
			int8 tid_s[30];
			_PARSE_COMMAND(NULL, tid_s, 29);
			r = _ConCmdKillTask(tid_s);
			ScrPrintString("\n");
		}
		else if(strcmp(name, "tasks") == 0)
		{
			int32 part = 0;
			int32 err = 0;
			while(1)
			{
				_PARSE_COMMAND(NULL, word, 1023);
				if(strcmp(word, "-p") == 0)
					part = 1;
				else if(strcmp(word, "") == 0)
					break;
				else
				{
					_ConCmdError("Invalid option!");
					err = 1;
					r = 0;
				}
			}
			if(!err)
				r = _ConCmdShowTaskInfo(part);
			ScrPrintString("\n");
		}
		else if(strcmp(name, "run") == 0)
		{
			int8 src_path[MAX_PATH_BUFFER_LEN];
			_PARSE_COMMAND(NULL, src_path, MAX_PATH_LEN);
			BOOL ran = FALSE;
			int8 buffer[10 * 1024];
			int8 path[1024];
			KnlGetVarValueWithSize(global_vars_s,
									"__path__",
									buffer,
									10 * 1024);
			split_string(NULL, NULL, 0, 0);
			while(split_string(path, buffer, ';', 1024) != NULL)
				if(Ifs1FileExists(path, src_path))
				{
					int8 temp[1024];
					UtlCopyString(temp, sizeof(temp), path);
					UtlConcatString(temp, sizeof(temp), src_path);
					r = _ConCmdExecuteApplication(temp, cmd) == -1 ? 0 : 1;
					ran = TRUE;
				}
			if(!ran)
				r = _ConCmdExecuteApplication(src_path, cmd) == -1 ? 0 : 1;
		}
		else if(strcmp(name, "clear") == 0)
		{
			ScrClearScreen();
			r = 1;
		}
		else if(strcmp(name, "help") == 0 || strcmp(name, "?") == 0)
		{
			_ConCmdShowHelpInfo();
			r = 1;
			ScrPrintString("\n");
		}
		else if(strcmp(name, "reboot") == 0)
			_ConCmdReboot();
		else if(strcmp(name, "shutdown") == 0)
			_ConCmdShutdown();
		else if(strcmp(name, "vmode") == 0)
		{
			_PARSE_COMMAND(NULL, word, 1023);
			r = _ConCmdChangeVideoMode(word);
			ScrPrintString("\n");
		}
		else if(strcmp(name, "cpuid") == 0)
		{
			r = _ConCmdShowCpuInfo();
			ScrPrintString("\n");
		}
		else if(strcmp(name, "set") == 0)
		{
			int8 property[1024];
			int8 value[1024];
			_PARSE_COMMAND(NULL, property, 1023);
			_PARSE_COMMAND(NULL, value, 1023);
			r = _ConCmdSetProperty(property, value);
			ScrPrintString("\n");
		}
		else if(strcmp(name, "flushlog") == 0)
		{
			LogWriteToDisk();
			LogClear();
			r = 1;
			ScrPrintString("\n");
		}
		else if(strcmp(name, "mkslink") == 0)
		{
			int8 path[1024];
			int8 name[1024];
			int8 link[1024];
			_PARSE_COMMAND(NULL, path, 1023);
			_PARSE_COMMAND(NULL, name, 1023);
			_PARSE_COMMAND(NULL, link, 1023);
			r = _ConCmdCreateSLink(path, name, link);
			ScrPrintString("\n");
		}
		else if(strcmp(name, "delslink") == 0)
		{
			int8 path[1024];
			_PARSE_COMMAND(NULL, path, 1023);
			r = _ConCmdDeleteSLink(path);
			ScrPrintString("\n");
		}
		else if(strcmp(name, "slink") == 0)
		{
			int8 path[1024];
			_PARSE_COMMAND(NULL, path, 1023);
			r = _ConCmdShowSLinkTarget(path);
			ScrPrintString("\n");
		}

		else if(strcmp(name, "hash") == 0)
		{
			#include <encryptionlib/md5.h>
			#include <encryptionlib/sha1.h>
			#include <encryptionlib/sha256.h>
			#include <encryptionlib/sha512.h>

			int8 text[1024];
			_PARSE_COMMAND(NULL, text, 1023);

			ScrPrintStringWithProperty("MD5:    ", CC_GREEN);
			Md5Context md5_context;
			MD5_HASH md5_hash;
			Md5Initialise(&md5_context);
			Md5Update(&md5_context, text, strlen(text));
			Md5Finalise(&md5_context, &md5_hash);
			uint32 ui;
			for(ui = 0; ui < MD5_HASH_SIZE; ui++)
				printuchex(md5_hash.bytes[ui]);
			ScrPrintString("\n");

			ScrPrintStringWithProperty("SHA1:   ", CC_GREEN);
			Sha1Context sha1_context;
			SHA1_HASH sha1_hash;
			Sha1Initialise(&sha1_context);
			Sha1Update(&sha1_context, text, strlen(text));
			Sha1Finalise(&sha1_context, &sha1_hash);
			for(ui = 0; ui < SHA1_HASH_SIZE; ui++)
				printuchex(sha1_hash.bytes[ui]);
			ScrPrintString("\n");

			ScrPrintStringWithProperty("SHA256: ", CC_GREEN);
			Sha256Context sha256_context;
			SHA256_HASH sha256_hash;
			Sha256Initialise(&sha256_context);
			Sha256Update(&sha256_context, text, strlen(text));
			Sha256Finalise(&sha256_context, &sha256_hash);
			for(ui = 0; ui < SHA256_HASH_SIZE; ui++)
				printuchex(sha256_hash.bytes[ui]);
			ScrPrintString("\n");

			ScrPrintStringWithProperty("SHA512: ", CC_GREEN);
			Sha512Context sha512_context;
			SHA512_HASH sha512_hash;
			Sha512Initialise(&sha512_context);
			Sha512Update(&sha512_context, text, strlen(text));
			Sha512Finalise(&sha512_context, &sha512_hash);
			for(ui = 0; ui < SHA512_HASH_SIZE; ui++)
				printuchex(sha512_hash.bytes[ui]);
			ScrPrintString("\n");
		}

		else if(strcmp(name, "serial") == 0)
		{
			#include "serial.h"
			
			int8 * text = "ISystem II x86\0";
			SrlWriteBuffer(PORT_COM1, text, strlen(text) + 1);
			
			while(TRUE)
				if(SrlHasData(PORT_COM1))
				{
					int8 c;
					SrlRead(PORT_COM1, &c);
					ScrPrintChar(c);
				}

			ScrPrintString("\n");
		}
		else if(strcmp(name, "message") == 0)
		{
			ASCCHAR title[1024];
			ASCCHAR text[1024];
			_PARSE_COMMAND(NULL, title, 1023);
			_PARSE_COMMAND(NULL, text, 1023);
			WinMsgShow(	title,
						text,
						MESSAGE_WINDOW_STYLE_CENTER
						| MESSAGE_WINDOW_STYLE_TOP,
						0,
						0,
						0xff000000,
						0xffffffff);
		}
		else if(strcmp(name, "detail") == 0)
		{
			ASCCHAR path[1024];
			_PARSE_COMMAND(NULL, path, 1023);
			Ifs1GetAbsolutePath(path, current_path, path);
			WinDetailShow(path);
		}
		#ifdef _KERNEL_DEBUG_
		// Kernel Unit Test
		else if(strcmp(name, "t") == 0)
		{
			#include "fs/ext2/ext2.h"

			printn(sizeof(Ext2SuperBlock));
			ScrPrintString("\n");
		}
		else if(strcmp(name, "kernel-unit-test") == 0)
		{
			#include "test.h"
			RUN_UNIT_TEST(UtlSfstr);
		}
		else if(strcmp(name, "net-test") == 0)
		{
			#include "net/helper.h"

			NetDevicePtr netdev = NetGet(0);
			void _(
				IN void * device,
				IN uint8 * ip_src,
				IN uint16 port_src,
				IN uint8 * ip_dst,
				IN uint16 port_dst,
				IN uint8 * data,
				IN uint16 len)
			{
				ScrPrintString("#UDP:\n");

				ScrPrintString("Source IP: ");
				ScrPrintIP(ip_src);
				ScrPrintString("\n");

				ScrPrintString("Source Port: ");
				printn(port_src);
				ScrPrintString("\n");

				ScrPrintString("Dest IP: ");
				ScrPrintIP(ip_dst);
				ScrPrintString("\n");

				ScrPrintString("Dest Port: ");
				printn(port_dst);
				ScrPrintString("\n");

				ScrPrintString("Len: ");
				printn(len);
				ScrPrintString("\n");

				ScrPrintString("Data: ");
				ScrPrintString(data);
				ScrPrintString("\n");
			}
			netdev->ProcessUDP[12345] = _;

		}
		else if (strcmp(name, "net") == 0)
		{
			ScrPrintString("NetEthernetFrame: ");
			printn(sizeof(NetEthernetFrame));
			ScrPrintString("\n");

			ScrPrintString("NetIPv4Frame: ");
			printn(sizeof(NetIPv4Frame));
			ScrPrintString("\n");
			
			ScrPrintString("NetUDPFrame: ");
			printn(sizeof(NetUDPFrame));
			ScrPrintString("\n");

			ScrPrintString("NetUDPFrameWithIPv4PseudoHeader: ");
			printn(sizeof(NetUDPFrameWithIPv4PseudoHeader));
			ScrPrintString("\n");

			ScrPrintString("IPv4UDPPacket: ");
			printn(sizeof(NetIPv4UDPPacket));
			ScrPrintString("\n");
		}
		else if (strcmp(name, "net-info") == 0)
		{
			uint32 count = NetGetCount();
			uint32 i;
			for (i = 0; i < count; i++)
			{
				NetDevicePtr netdev = NetGet(i);
				if (netdev != NULL)
				{
					ScrPrintString("#");
					printn(i);
					ScrPrintString("\n");

					ScrPrintString("    Name: ");
					ScrPrintString(netdev->GetName(netdev));
					ScrPrintString("\n");

					ScrPrintString("    MAC: ");
					uint8 * mac = netdev->GetMAC(netdev);
					ScrPrintMAC(mac);
					ScrPrintString("\n");

					ScrPrintString("    IP: ");
					uint8 * ip = netdev->GetIP(netdev);
					ScrPrintIP(ip);
					ScrPrintString("\n");
				}
			}
		}
		else if(strcmp(name, "net-set-ip") == 0)
		{
			ASCCHAR index[1024];
			ASCCHAR ip0[1024];
			ASCCHAR ip1[1024];
			ASCCHAR ip2[1024];
			ASCCHAR ip3[1024];
			_PARSE_COMMAND(NULL, index, 1023);
			_PARSE_COMMAND(NULL, ip0, 1023);
			_PARSE_COMMAND(NULL, ip1, 1023);
			_PARSE_COMMAND(NULL, ip2, 1023);
			_PARSE_COMMAND(NULL, ip3, 1023);
			uint32 idx = stoui(index);
			uint8 ip[4];
			ip[0] = (uint8)stoui(ip0);
			ip[1] = (uint8)stoui(ip1);
			ip[2] = (uint8)stoui(ip2);
			ip[3] = (uint8)stoui(ip3);
			NetDevicePtr netdev = NetGet(idx);
			netdev->SetIP(netdev, ip);
		}
		else if(strcmp(name, "net-arp-info") == 0)
		{
			uint32 count = NetGetARPRecordCount();
			uint32 i;
			for (i = 0; i < count; i++)
			{
				NetARPRecordPtr record = NetGetARPRecord(i);
				ScrPrintIP(record->ip);
				ScrPrintString(" -> ");
				ScrPrintMAC(record->mac);
				ScrPrintString("\n");
			}
		}
		else if(strcmp(name, "net-arp") == 0)
		{
			ASCCHAR index[1024];
			ASCCHAR ip0[1024];
			ASCCHAR ip1[1024];
			ASCCHAR ip2[1024];
			ASCCHAR ip3[1024];
			_PARSE_COMMAND(NULL, index, 1023);
			_PARSE_COMMAND(NULL, ip0, 1023);
			_PARSE_COMMAND(NULL, ip1, 1023);
			_PARSE_COMMAND(NULL, ip2, 1023);
			_PARSE_COMMAND(NULL, ip3, 1023);
			uint32 idx = stoui(index);
			NetDevicePtr netdev = NetGet(idx);

			uint8 mac_dst[6] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
			uint8 ip_dst[4];
			ip_dst[0] = (uint8)stoui(ip0);
			ip_dst[1] = (uint8)stoui(ip1);
			ip_dst[2] = (uint8)stoui(ip2);
			ip_dst[3] = (uint8)stoui(ip3);

			uint8 * mac_src = netdev->GetMAC(netdev);
			uint8 * ip_src = netdev->GetIP(netdev);

			NetARPPacket pk_arp;
			MemClear(&pk_arp, 0x00, sizeof(pk_arp));

			// 以太网帧。
			MemCopy(mac_dst, pk_arp.fr_eth.mac_dst, 6);
			MemCopy(mac_src, pk_arp.fr_eth.mac_src, 6);
			pk_arp.fr_eth.type = NetToBigEndian16(0x0806);

			// ARP帧。
			pk_arp.fr_arp.htype = NetToBigEndian16(0x0001);
			pk_arp.fr_arp.ptype = NetToBigEndian16(0x0800);
			pk_arp.fr_arp.hlen = 6;
			pk_arp.fr_arp.plen = 4;
			pk_arp.fr_arp.oper = NetToBigEndian16(0x0001);
			MemCopy(mac_src, pk_arp.fr_arp.sha, 6);
			MemCopy(ip_src, pk_arp.fr_arp.spa, 4);
			MemCopy(mac_dst, pk_arp.fr_arp.tha, 6);
			MemCopy(ip_dst, pk_arp.fr_arp.tpa, 4);

			netdev->SendPacket(netdev, &pk_arp, sizeof(pk_arp));
		}
		else if(strcmp(name, "net-send-msg") == 0)
		{
			int32 i;
			ASCCHAR index[1024];
			ASCCHAR port_src[1024];
			ASCCHAR ip0[1024];
			ASCCHAR ip1[1024];
			ASCCHAR ip2[1024];
			ASCCHAR ip3[1024];
			ASCCHAR port_dst[1024];
			ASCCHAR msg[128];
			_PARSE_COMMAND(NULL, index, 1023);
			_PARSE_COMMAND(NULL, port_src, 1023);
			_PARSE_COMMAND(NULL, ip0, 1023);
			_PARSE_COMMAND(NULL, ip1, 1023);
			_PARSE_COMMAND(NULL, ip2, 1023);
			_PARSE_COMMAND(NULL, ip3, 1023);
			_PARSE_COMMAND(NULL, port_dst, 1023);
			_PARSE_COMMAND(NULL, msg, 127);
			uint32 idx = stoui(index);
			NetDevicePtr netdev = NetGet(idx);

			uint8 ip_dst[4];
			ip_dst[0] = (uint8)stoui(ip0);
			ip_dst[1] = (uint8)stoui(ip1);
			ip_dst[2] = (uint8)stoui(ip2);
			ip_dst[3] = (uint8)stoui(ip3);
			

			NetSendUDP(netdev, (uint16)stoui(port_src), ip_dst, (uint16)stoui(port_dst), msg, strlen(msg) + 1);
		}
		else if (strcmp(name, "sb16init") == 0)
		{
			SB16Init();
		}
		else if (strcmp(name, "sb16start") == 0)
		{
			#include "wav/wav.h"
			WavPtr wav = WavCreateFromFile("VS:/isystem/demo.wav");
			printuihex(wav);
			ScrPrintString("\n");
			BOOL r = SB16Playback(
				&(wav->data_first_byte),
				wav->data_subchunk_size,
				wav->sample_rate,
				wav->bits_per_sample == 16,
				wav->num_channels == 2,
				FALSE);

			ScrPrintString("len: ");
			printn(wav->data_subchunk_size);
			ScrPrintString("\n");
			
			ScrPrintString("frequency: ");
			printuihex(wav->sample_rate);
			ScrPrintString("\n");

			ScrPrintString("bits16: ");
			printuihex(wav->bits_per_sample == 16);
			ScrPrintString("\n");

			ScrPrintString("stereo: ");
			printuihex(wav->num_channels == 2);
			ScrPrintString("\n");

			printuihex(r);
			ScrPrintString("\n");
		}
		else if (strcmp(name, "sb16stop") == 0)
		{
			SB16Stop();
		}
		else if (strcmp(name, "sb16pause") == 0)
		{
			SB16Pause();
		}
		else if (strcmp(name, "sb16resume") == 0)
		{
			SB16Resume();
		}
		#endif
		//Batch
		else if(strcmp(name, "goto") == 0)
		{
			uint32 ui;
			_PARSE_COMMAND(NULL, word, 1023);
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
			_PARSE_COMMAND_WITHOUT_VAR(NULL, var_name, MAX_VAR_NAME_LEN);
			if(strlen(var_name) < 2)
				r = 0;
			else
			{
				struct Vars * vars_s = NULL;
				if(var_name[0] == '$')
					vars_s = local_vars_s;
				else if(var_name[0] == '@')
					vars_s = global_vars_s;
				if(KnlNewVar(vars_s, var_name + 1))
				{
					_PARSE_COMMAND_WITHOUT_VAR(NULL, var_value, MAX_VAR_VALUE_LEN);
					if(KnlSetVarValue(vars_s, var_name + 1, var_value))
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
			_PARSE_COMMAND_WITHOUT_VAR(NULL, result, 1023);
			while(1)
			{
				_PARSE_COMMAND(NULL, word, 1023);
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
					UtlCopyString(stack[++stack_top], sizeof(stack[++stack_top]), dtos(buffer, n_r));
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
					UtlCopyString(stack[++stack_top], sizeof(stack[++stack_top]), dtos(buffer, n_r));
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
					UtlCopyString(stack[++stack_top], sizeof(stack[++stack_top]), dtos(buffer, n_r));
				}
				else if(strcmp(word, "strcat") == 0)
				{
					if(stack_top == -1)
						return 0;
					int8 * str = stack[stack_top--];
					if(stack_top == -1)
						return 0;
					UtlConcatString(stack[stack_top], sizeof(stack[stack_top]), str);
				}
				else
				{
					if(stack_top == 63)
						return 0;
					UtlCopyString(stack[++stack_top], sizeof(stack[++stack_top]), word);
				}
			}
			if(stack_top != 0)
				return 0;
			struct Vars * vars_s = NULL;
			if(result[0] == '$')
				KnlSetVarValue(local_vars_s, result + 1, stack[stack_top--]);
			else if(result[0] == '@')
				KnlSetVarValue(global_vars_s, result + 1, stack[stack_top--]);
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
		else if(strcmp(name, "%") == 0)
		{
			int8 stdin[1024];
			int8 stdout[1024];
			int8 stderr[1024];
			int8 app[1024];
			_PARSE_COMMAND(NULL, stdin, 1023);
			_PARSE_COMMAND(NULL, stdout, 1023);
			int32 remainedlen = _PARSE_COMMAND(NULL, stderr, 1023);
			int32 len = strlen(cmd) - remainedlen;
			if(strcmp(stdin, "%") != 0)
				Ifs1GetAbsolutePath(stdin, current_path, stdin);
			if(strcmp(stdout, "%") != 0)
				Ifs1GetAbsolutePath(stdout, current_path, stdout);
			if(strcmp(stderr, "%") != 0)
				Ifs1GetAbsolutePath(stderr, current_path, stderr);
			_PARSE_COMMAND(NULL, app, 1023);
			BOOL ran = FALSE;
			int8 buffer[10 * 1024];
			int8 path[1024];
			KnlGetVarValueWithSize(global_vars_s,
									"__path__",
									buffer,
									10 * 1024);
			split_string(NULL, NULL, 0, 0);
			while(split_string(path, buffer, ';', 1024) != NULL)
				if(Ifs1FileExists(path, app))
				{
					int8 temp[1024];
					UtlCopyString(temp, sizeof(temp), path);
					UtlConcatString(temp, sizeof(temp), app);
					r = run_wait(temp, cmd + len, stdin, stdout, stderr) == -1 ? 0 : 1;
					ran = TRUE;
				}
			if(!ran)
				r = run_wait(app, cmd + len, stdin, stdout, stderr) == -1 ? 0 : 1;
		}
		else if(strlen(name) >= 4 && strcmp(name + strlen(name) - 4, ".bat") == 0)
		{
			BOOL ran = FALSE;
			int8 buffer[10 * 1024];
			int8 path[1024];
			KnlGetVarValueWithSize(global_vars_s,
									"__path__",
									buffer,
									10 * 1024);
			split_string(NULL, NULL, 0, 0);
			while(split_string(path, buffer, ';', 1024) != NULL)
				if(Ifs1FileExists(path, name))
				{
					int8 temp[1024];
					UtlCopyString(temp, sizeof(temp), path);
					UtlConcatString(temp, sizeof(temp), name);
					r = _ConExecuteBatchScript(temp);
					ran = TRUE;
					break;
				}
			if(!ran)
				r = _ConExecuteBatchScript(name);
		}
		else
		{
			BOOL ran = FALSE;
			int8 buffer[10 * 1024];
			int8 path[1024];
			KnlGetVarValueWithSize(global_vars_s,
									"__path__",
									buffer,
									10 * 1024);
			split_string(NULL, NULL, 0, 0);
			while(split_string(path, buffer, ';', 1024) != NULL)
				if(Ifs1FileExists(path, name))
				{
					int8 temp[1024];
					UtlCopyString(temp, sizeof(temp), path);
					UtlConcatString(temp, sizeof(temp), name);
					r = run_wait(temp, cmd, NULL, NULL, NULL) == -1 ? 0 : 1;
					ran = TRUE;
				}
			if(!ran)
				r = run_wait(name, cmd, NULL, NULL, NULL) == -1 ? 0 : 1;
		}
	}

	// 等待一个程序运行结束。当wait_app_tid为-1时，代表等待的程序已结束，则退出等待。
	while(wait_app_tid != -1)
	{
		// 这里加锁是因为检测任务是否还可用时不被打断，否则总是会出奇怪的问题。
		lock();
		struct Task * task = TskmgrGetTaskInfoPtr(wait_app_tid);
		if(task == NULL)
			wait_app_tid = -1;
		unlock();
		uint32 ui;
		for(ui = 0; ui < 0x000fffff; ui++)
			asm volatile ("pause");
	}

	return r;
}

/**
	@Function:		_ConExecuteInitBatchScript
	@Access:		Private
	@Description:
		调用初始化脚本。
	@Parameters:
	@Return:
*/
static
void
_ConExecuteInitBatchScript(void)
{
	int8 buffer[100];
	//__video_mode__
	KnlNewVar(global_vars_s, "__video_mode__");
	if(!VesaIsEnabled())
		KnlSetVarValue(global_vars_s, "__video_mode__", "text");
	else	
	{
		uint32 width = VesaGetWidth();
		uint32 height = VesaGetHeight();
		if(width == 640 && height == 480)
			KnlSetVarValue(global_vars_s, "__video_mode__", "vesa640_480");
		else if(width == 800 && height == 600)
			KnlSetVarValue(global_vars_s, "__video_mode__", "vesa800_600");
		else if(width == 1024 && height == 768)
			KnlSetVarValue(global_vars_s, "__video_mode__", "vesa1024_768");
		else if(width == 1280 && height == 1024)
			KnlSetVarValue(global_vars_s, "__video_mode__", "vesa1280_1024");
	}

	//__master_ver__
	KnlNewVar(global_vars_s, "__master_ver__");
	KnlSetVarValue(global_vars_s, "__master_ver__", MASTER_VER);

	//__slave_ver__
	KnlNewVar(global_vars_s, "__slave_ver__");
	KnlSetVarValue(global_vars_s, "__slave_ver__", SLAVE_VER);

	_ConExecuteBatchScript(SYSTEM_INIT_BAT);
}

/**
	@Function:		ConEnterConsole
	@Access:		Public
	@Description:
		控制台入口函数。
	@Parameters:
	@Return:		
*/
void
ConEnterConsole(void)
{	
	int8 command[1024];
	global_vars_s = KnlAllocVarList(BATCH_MAX_VARS_COUNT);
	struct Vars * local_vars_s;
	local_vars_s = KnlAllocVarList(BATCH_MAX_VARS_COUNT);
	_ConExecuteInitBatchScript();
	BOOL bvalue = FALSE;
	if(config_system_console_get_bool("ShowVerInfoWhenBoot", &bvalue) && bvalue)
		_ConCmdVer();
	if(config_system_console_get_bool("ShowDiskInfoWhenBoot", &bvalue) && bvalue)
	{
		_ConCmdShowDiskInfo();
		ScrPrintString("\n");
	}
	if(config_system_console_get_bool("ShowMemoryInfoWhenBoot", &bvalue) && bvalue)
	{
		_ConCmdShowMemoryInfo();
		ScrPrintString("\n");
	}
	if(config_system_console_get_bool("ShowCPUInfoWhenBoot", &bvalue) && bvalue)
	{
		_ConCmdShowCpuInfo();
		ScrPrintString("\n");
	}
	ScrPrintString("\n");

	while(1)
	{
		int8 prompt[KB(1)];
		if(strcmp(current_path, "") == 0)
		{
			UtlCopyString(prompt, sizeof(prompt), promptns);
			ScrPrintStringWithProperty(prompt, promptns_color);
		}
		else
		{
			sprintf_s(prompt, sizeof(prompt), prompts, current_path);
			ScrPrintStringWithProperty(prompt, prompts_color);
		}
		get_strn(command, 1023);
		_ConExecuteCommand(command, NULL, 0, 0, local_vars_s);
	}
}

/**
	@Function:		ConUpdateClock
	@Access:		Public
	@Description:
		该函数可以在控制台右上角显示当前日期时间。
	@Parameters:
	@Return:		
*/
void
ConUpdateClock(void)
{
	if(enable_clock)
	{
		//YYYY-MM-DD WWWWWWWWW HH:NN
		ScrLockConsoleCursor();
		uint32 ui;
		uint16 max_chars = 26;
		uint16 startx = COLUMN - max_chars;	
		uint16 x, y;
		ScrGetConsoleCursor(&x, &y);
		ScrSetConsoleCursorPosition(startx, 0);
		for(ui = 0; ui < max_chars; ui++)
			ScrPrintString(" ");
		ScrSetConsoleCursorPosition(startx, 0);
		_ConCmdShowDate();
		ScrPrintString(" ");
		_ConCmdShowTime(0);
		ScrSetConsoleCursorPosition(x, y);
		ScrUnlockConsoleCursor();
	}
}

/**
	@Function:		ConGetCurrentApplicationTid
	@Access:		Public
	@Description:
		获取控制台正在等待的任务的任务ID。
	@Parameters:
	@Return:
		int32
			控制台正在等待的任务的任务ID。		
*/
int32
ConGetCurrentApplicationTid(void)
{
	return wait_app_tid;
}

/**
	@Function:		ConSetCurrentApplicationTid
	@Access:		Public
	@Description:
		设置控制台正在等待的任务的任务ID。
	@Parameters:
		tid, int32, IN
			控制台正在等待的任务的任务ID。
	@Return:	
*/
void
ConSetCurrentApplicationTid(IN int32 tid)
{
	wait_app_tid = tid;
}

/**
	@Function:		ConSetClock
	@Access:		Public
	@Description:
		设置控制台的时钟是否可用。
	@Parameters:
		enable, int32, IN
			值为0时不可用，否则可用。
	@Return:	
*/
void
ConSetClock(IN int32 enable)
{
	enable_clock = enable;
}

/**
	@Function:		ConGetCurrentDir
	@Access:		Public
	@Description:
		获取当前目录路径。
	@Parameters:
		path, int8 *, IN
			用于储存当前目录路径的缓冲区。
			该缓冲区的大小必须大于或等于MAX_PATH_BUFFER_LEN。
	@Return:	
*/
void
ConGetCurrentDir(OUT int8 * path)
{
	UtlCopyString(path, MAX_PATH_BUFFER_LEN, current_path);
}

/**
	@Function:		ConInit
	@Access:		Public
	@Description:
		初始化控制台。
	@Parameters:
	@Return:	
*/
BOOL
ConInit(void)
{
	config_system_console_get_bool("ShowClock", &enable_clock);
	config_system_console_get_string(	"PromptWhenDirIsNotSelected", 
										promptns,
										sizeof(promptns));
	config_system_console_get_string(	"PromptWhenDirIsSelected",
										prompts,
										sizeof(prompts));
	double dv = 0.0;
	config_system_console_get_number("PromptColorWhenDirIsNotSelected", &dv);
	promptns_color = (uint8)dv;
	config_system_console_get_number("PromptColorWhenDirIsSelected", &dv);
	prompts_color = (uint8)dv;
	return TRUE;
}

/**
	@Function:		ConExitBatchScript
	@Access:		Public
	@Description:
		退出批处理。
	@Parameters:
	@Return:	
*/
void
ConExitBatchScript(void)
{
	if(_run_in_batch)
		_exit_batch = TRUE;
}

/**
	@Function:		ConExecuteCommand
	@Access:		Public
	@Description:
		执行命令。
	@Parameters:
		cmd, CASCTEXT, IN
			指向储存命令字符串的缓冲区的指针。
	@Return:
		返回0则失败，否则成功。
*/
int32
ConExecuteCommand(IN CASCTEXT cmd)
{
	struct Vars * local_vars_s = NULL;
	local_vars_s = KnlAllocVarList(BATCH_MAX_VARS_COUNT);
	if(local_vars_s == NULL)
		goto err;
	int32 r = _ConExecuteCommand(cmd, NULL, 0, 0, local_vars_s);
	KnlFreeVarList(local_vars_s);
	return r;
err:
	return 0;
}
