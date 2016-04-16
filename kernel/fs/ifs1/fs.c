/**
	@File:			fs.c
	@Author:		Ihsoh
	@Date:			2014-7-18
	@Description:
		提供IFS1文件系统的相关功能。
*/

#include "fs.h"
#include "blocks.h"
#include "../../types.h"
#include "../../cmos.h"
#include "../../screen.h"
#include "../../lock.h"
#include "../../config.h"
#include "../../disk.h"

#include <ilib/string.h>
#include <dslib/linked_list.h>

DEFINE_LOCK_IMPL(ifs1)

#define	_MAX_RW_OPTIMIZED_BYTES			KB(32)	// 必须是4的倍数。

static BOOL read_buffer_was_enabled		= FALSE;
static BOOL append_buffer_was_enabled	= FALSE;

/**
	@Function:		Ifs1Init
	@Access:		Public
	@Description:
		初始化IFS1文件系统。
	@Parameters:
	@Return:
		BOOL
			返回TRUE则成功，否则失败。		
*/
BOOL
Ifs1Init(void)
{
	config_system_ifs1_get_bool("EnableReadBuffer", 
								&read_buffer_was_enabled);
	config_system_ifs1_get_bool("EnableAppendBuffer",
								&append_buffer_was_enabled);
}

/**
	@Function:		Ifs1Format
	@Access:		Public
	@Description:
		格式化磁盘。
	@Parameters:
		symbol, int8 *, IN
			盘符。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。		
*/
BOOL
Ifs1Format(IN int8 * symbol)
{
	if(symbol == NULL || !Ifs1DeleteAllBlocks(symbol))
		return FALSE;
	struct DirBlock dir;
	Ifs1FillDirBlock("/", &dir);
	if(Ifs1AddBlock(symbol, (struct RawBlock *)&dir) != INVALID_BLOCK_ID)
		return TRUE;
	else
		return FALSE;
}

/**
	@Function:		Ifs1Check
	@Access:		Public
	@Description:
		检查磁盘是否为IFS1文件系统。
	@Parameters:
		symbol, int8 *, IN
			盘符。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。				
*/
BOOL
Ifs1Check(IN int8 * symbol)
{
	struct DirBlock dir;
	if(	symbol == NULL 
		|| !Ifs1GetBlock(symbol, START_BLOCK_ID, (struct RawBlock *)&dir))
		return FALSE;
	if(dir.type == BLOCK_TYPE_DIR && strcmp(dir.dirname, "/") == 0)
		return TRUE;
	else
		return FALSE;
}

/**
	@Function:		_Ifs1DirExists
	@Access:		Private
	@Description:
		确认指定磁盘的指定目录内是否存在指定名称的文件夹。
	@Parameters:
		symbol, int8 *, IN
			盘符。
		id, int8 *, IN
			目录的块ID。
		name, int8 *, IN
			要确认的文件夹的名称。
	@Return:
		BOOL
			返回TRUE则存在，否则不存在。	
*/
static
BOOL
_Ifs1DirExists(	IN int8 * symbol, 
				IN uint32 id, 
				IN int8 * name)
{
	struct DirBlock dir;
	if(	id == INVALID_BLOCK_ID
		|| symbol == NULL
		|| name == NULL
		|| !Ifs1GetBlock(symbol, id, (struct RawBlock *)&dir) 
		|| dir.used == 0 
		|| dir.type != BLOCK_TYPE_DIR)
		return FALSE;
	uint32 ui;
	for(ui = 0; ui < sizeof(dir.blockids) / sizeof(uint32); ui++)
	{
		uint32 subdir_id = dir.blockids[ui];
		struct DirBlock subdir;
		if(	subdir_id != INVALID_BLOCK_ID 
			&& Ifs1GetBlock(symbol, subdir_id, (struct RawBlock *)&subdir) 
			&& subdir.used
			&& subdir.type == BLOCK_TYPE_DIR
			&& strcmp(subdir.dirname, name) == 0)
			return TRUE;
	}
	return FALSE;
}

/**
	@Function:		_Ifs1FileExists
	@Access:		Private
	@Description:
		确认指定磁盘的指定目录内是否存在指定名称的文件。
	@Parameters:
		symbol, int8 *, IN
			盘符。
		id, int8 *, IN
			目录的块ID。
		name, int8 *, IN
			要确认的文件的名称。
	@Return:
		BOOL
			返回TRUE则存在，否则不存在。	
*/
static
BOOL
_Ifs1FileExists(IN int8 * symbol, 
				IN uint32 id, 
				IN int8 * name)
{
	struct DirBlock dir;
	if(	id == INVALID_BLOCK_ID
		|| symbol == NULL
		|| name == NULL
		|| !Ifs1GetBlock(symbol, id, (struct RawBlock *)&dir) 
		|| dir.used == 0 
		|| dir.type != BLOCK_TYPE_DIR)
		return FALSE;
	uint32 ui;
	for(ui = 0; ui < sizeof(dir.blockids) / sizeof(uint32); ui++)
	{
		uint32 subfile_id = dir.blockids[ui];
		struct FileBlock subfile;
		if(	subfile_id != INVALID_BLOCK_ID 
			&& Ifs1GetBlock(symbol, subfile_id, (struct RawBlock *)&subfile)
			&& subfile.used
			&& subfile.type == BLOCK_TYPE_FILE
			&& strcmp(subfile.filename, name) == 0)
			return TRUE;
	}
	return FALSE;
}

/**
	@Function:		_Ifs1SLinkExists
	@Access:		Private
	@Description:
		确认指定磁盘的指定目录内是否存在指定名称的软链接。
	@Parameters:
		symbol, int8 *, IN
			盘符。
		id, int8 *, IN
			目录的块ID。
		name, int8 *, IN
			要确认的软链接的名称。
	@Return:
		BOOL
			返回TRUE则存在，否则不存在。	
*/
static
BOOL
_Ifs1SLinkExists(	IN int8 * symbol,
					IN uint32 id,
					IN int8 * name)
{
	struct DirBlock dir;
	if(	id == INVALID_BLOCK_ID
		|| symbol == NULL
		|| name == NULL
		|| !Ifs1GetBlock(symbol, id, (struct RawBlock *)&dir) 
		|| dir.used == 0 
		|| dir.type != BLOCK_TYPE_DIR)
		return FALSE;
	uint32 ui;
	for(ui = 0; ui < sizeof(dir.blockids) / sizeof(uint32); ui++)
	{
		uint32 slink_id = dir.blockids[ui];
		struct SLinkBlock slink;
		if(	slink_id != INVALID_BLOCK_ID 
			&& Ifs1GetBlock(symbol, slink_id, (struct RawBlock *)&slink)
			&& slink.used
			&& slink.type == BLOCK_TYPE_SLINK
			&& strcmp(slink.filename, name) == 0)
			return TRUE;
	}
	return FALSE;
}

/**
	@Function:		_Ifs1CreateDir
	@Access:		Private
	@Description:
		在指定的磁盘的指定目录下创建文件夹。
		创建的文件夹的名称不能与目录下存在的软链接的名称相同。
		低级方式。
	@Parameters:
		symbol, int8 *, IN
			盘符。
		id, int8 *, IN
			目录的块ID。
		name, int8 *, IN
			将要创建的文件夹的名称。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。	
*/
static
BOOL
_Ifs1CreateDir(	IN int8 * symbol, 
				IN uint32 id, 
				IN int8 * name)
{
	struct DirBlock dir;
	if(	id == INVALID_BLOCK_ID
		|| symbol == NULL
		|| name == NULL
		|| strlen(name) > MAX_DIRNAME_LEN
		|| !Ifs1GetBlock(symbol, id, (struct RawBlock *)&dir) 
		|| dir.used == 0 
		|| dir.type != BLOCK_TYPE_DIR
		|| _Ifs1SLinkExists(symbol, id, name)
		|| _Ifs1DirExists(symbol, id, name))
		return FALSE;
	struct DirBlock new_dir;
	Ifs1FillDirBlock(name, &new_dir);
	uint32 new_dir_id = Ifs1AddBlock(symbol, (struct RawBlock *)&new_dir);
	if(new_dir_id == INVALID_BLOCK_ID)
		return FALSE;
	uint32 ui;
	for(ui = 0; ui < sizeof(dir.blockids) / sizeof(uint32); ui++)
		if(dir.blockids[ui] == INVALID_BLOCK_ID)
		{
			dir.blockids[ui] = new_dir_id;
			Ifs1SetBlock(symbol, id, (struct RawBlock *)&dir);
			return TRUE;
		}
	return FALSE;
}

/**
	@Function:		_Ifs1CreateFile
	@Access:		Private
	@Description:
		在指定的磁盘的指定目录下创建文件。
		创建的文件的名称不能与目录下存在的软链接的名称相同。
		低级方式。
	@Parameters:
		symbol, int8 *, IN
			盘符。
		id, int8 *, IN
			目录的块ID。
		name, int8 *, IN
			将要创建的文件的名称。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。	
*/
static
BOOL
_Ifs1CreateFile(IN int8 * symbol, 
				IN uint32 id, 
				IN int8 * name)
{
	struct DirBlock dir;
	if(	id == INVALID_BLOCK_ID
		|| symbol == NULL
		|| name == NULL
		|| strlen(name) > MAX_FILENAME_LEN
		|| !Ifs1GetBlock(symbol, id, (struct RawBlock *)&dir) 
		|| dir.used == 0 
		|| dir.type != BLOCK_TYPE_DIR
		|| _Ifs1SLinkExists(symbol, id, name)
		|| _Ifs1FileExists(symbol, id, name))
		return FALSE;
	struct FileBlock new_file;
	Ifs1FillFileBlock(name, &new_file);
	uint32 new_file_id = Ifs1AddBlock(symbol, (struct RawBlock *)&new_file);
	if(new_file_id == INVALID_BLOCK_ID)
		return FALSE;
	uint32 ui;
	for(ui = 0; ui < sizeof(dir.blockids) / sizeof(uint32); ui++)
		if(dir.blockids[ui] == INVALID_BLOCK_ID)
		{
			dir.blockids[ui] = new_file_id;
			Ifs1SetBlock(symbol, id, (struct RawBlock *)&dir);
			return TRUE;
		}
	return FALSE;
}

/**
	@Function:		_Ifs1CreateSLink
	@Access:		Private
	@Description:
		在指定的磁盘的指定目录下创建软链接。
		创建的软链接的名称不能与目录下存在的文件或文件夹名称相同。
		低级方式。
	@Parameters:
		symbol, int8 *, IN
			盘符。
		id, int8 *, IN
			目录的块ID。
		name, int8 *, IN
			将要创建的软链接的名称。
		link, int8 *, IN
			软链接的目标。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。	
*/
static
BOOL
_Ifs1CreateSLink(	IN int8 * symbol,
					IN uint32 id,
					IN int8 * name,
					IN int8 * link)
{
	struct DirBlock dir;
	if(	id == INVALID_BLOCK_ID
		|| symbol == NULL
		|| name == NULL
		|| link == NULL
		|| strlen(name) > MAX_FILENAME_LEN
		|| strlen(link) >= SLINK_BLOCK_LINK_LEN
		|| !Ifs1GetBlock(symbol, id, (struct RawBlock *)&dir) 
		|| dir.used == 0 
		|| dir.type != BLOCK_TYPE_DIR
		|| _Ifs1SLinkExists(symbol, id, name)
		|| _Ifs1FileExists(symbol, id, name)
		|| _Ifs1DirExists(symbol, id, name))
		return FALSE;
	struct SLinkBlock new_slink;
	Ifs1FillSLinkBlock(name, link, &new_slink);
	uint32 new_slink_id = Ifs1AddBlock(symbol, (struct RawBlock *)&new_slink);
	if(new_slink_id == INVALID_BLOCK_ID)
		return FALSE;
	uint32 ui;
	for(ui = 0; ui < sizeof(dir.blockids) / sizeof(uint32); ui++)
		if(dir.blockids[ui] == INVALID_BLOCK_ID)
		{
			dir.blockids[ui] = new_slink_id;
			Ifs1SetBlock(symbol, id, (struct RawBlock *)&dir);
			return TRUE;
		}
	return FALSE;
}

/**
	@Function:		_Ifs1DeleteDir
	@Access:		Private
	@Description:
		删除指定磁盘的指定文件夹。
		低级方式。
	@Parameters:
		symbol, int8 *, IN
			盘符。
		id, uint32, IN
			文件夹的块ID。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。	
*/
static
BOOL
_Ifs1DeleteDir(	IN int8 * symbol, 
				IN uint32 id)
{
	struct DirBlock dir;
	if(	id == INVALID_BLOCK_ID
		|| symbol == NULL
		|| !Ifs1GetBlock(symbol, id, (struct RawBlock *)&dir) 
		|| dir.used == 0 
		|| dir.type != BLOCK_TYPE_DIR)
		return FALSE;
	uint32 ui;	
	for(ui = 0; ui < sizeof(dir.blockids) / sizeof(uint32); ui++)
		if(dir.blockids[ui] != INVALID_BLOCK_ID)
			return FALSE;
	dir.used = 0;
	return Ifs1SetBlock(symbol, id, (struct RawBlock *)&dir);
}

/**
	@Function:		_Ifs1DeleteFile
	@Access:		Private
	@Description:
		删除指定磁盘的指定文件。
		低级方式。
	@Parameters:
		symbol, int8 *, IN
			盘符。
		id, uint32, IN
			文件的块ID。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。	
*/
static
BOOL
_Ifs1DeleteFile(IN int8 * symbol, 
				IN uint32 id)
{
	struct FileBlock file;
	if(	id == INVALID_BLOCK_ID
		|| symbol == NULL
		|| !Ifs1GetBlock(symbol, id, (struct RawBlock *)&file)
		|| file.used == 0
		|| file.type != BLOCK_TYPE_FILE
		|| file.lock)
		return FALSE;
	file.used = 0;
	return Ifs1SetBlock(symbol, id, (struct RawBlock *)&file);
}

/**
	@Function:		_Ifs1DeleteSLink
	@Access:		Private
	@Description:
		删除指定磁盘的指定软链接。
		低级方式。
	@Parameters:
		symbol, int8 *, IN
			盘符。
		id, uint32, IN
			软链接的块ID。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。	
*/
static
BOOL
_Ifs1DeleteSLink(	IN int8 * symbol,
					IN uint32 id)
{
	struct SLinkBlock slink;
	if(	id == INVALID_BLOCK_ID
		|| symbol == NULL
		|| !Ifs1GetBlock(symbol, id, (struct RawBlock *)&slink)
		|| slink.used == 0
		|| slink.type != BLOCK_TYPE_SLINK)
		return FALSE;
	slink.used = 0;
	return Ifs1SetBlock(symbol, id, (struct RawBlock *)&slink);
}

/**
	@Function:		_Ifs1GetItemCount
	@Access:		Private
	@Description:
		获取指定磁盘的指定目录下的文件和文件夹数量的总和。
	@Parameters:
		symbol, int8 *, IN
			盘符。
		id, uint32, IN
			目录的块ID。
	@Return:
		int32
			指定磁盘的指定目录下的文件和文件夹数量的总和。
*/
static
int32
_Ifs1GetItemCount(	IN int8 * symbol, 
					IN uint32 id)
{
	int32 count = 0; 
	struct DirBlock dir;
	if(	id == INVALID_BLOCK_ID
		|| symbol == NULL
		|| !Ifs1GetBlock(symbol, id, (struct RawBlock *)&dir) 
		|| dir.used == 0 
		|| dir.type != BLOCK_TYPE_DIR)
		return -1;
	uint32 ui;
	for(ui = 0; ui < sizeof(dir.blockids) / sizeof(uint32); ui++)
	{
		uint32 sub_block_id = dir.blockids[ui];
		struct RawBlock sub_block;
		if(	sub_block_id != INVALID_BLOCK_ID
			&& Ifs1GetBlock(symbol, sub_block_id, &sub_block)
			&& sub_block.used)
			count++;
	}
	return count;
}

/**
	@Function:		_Ifs1GetItems
	@Access:		Private
	@Description:
		获取指定磁盘的指定目录下的文件和文件夹的块集合。
	@Parameters:
		symbol, int8 *, IN
			盘符。
		id, uint32, IN
			目录的块ID。
		blocks, struct RawBlock *, OUT
			指向用于储存块集合的缓冲区。
		max, uint32, IN
			储存块集合的缓冲区最大能储存块的个数。
	@Return:
		int32
			失败则返回-1，否则返回目录下的文件和文件夹数量的总和。
*/
static
int32
_Ifs1GetItems(	IN int8 * symbol, 
				IN uint32 id, 
				OUT struct RawBlock * blocks,
				IN uint32 max)
{
	int32 count = 0; 
	struct DirBlock dir;
	if(	id == INVALID_BLOCK_ID
		|| symbol == NULL
		|| blocks == NULL
		|| !Ifs1GetBlock(symbol, id, (struct RawBlock *)&dir) 
		|| dir.used == 0 
		|| dir.type != BLOCK_TYPE_DIR)
		return -1;
	uint32 ui;
	for(ui = 0; ui < sizeof(dir.blockids) / sizeof(uint32) && ui < max; ui++)
	{
		uint32 sub_blockid = dir.blockids[ui];
		struct RawBlock raw_block;
		if(	sub_blockid != INVALID_BLOCK_ID 
			&& Ifs1GetBlock(symbol, sub_blockid, &raw_block)
			&& raw_block.used)
		{
			memcpy(blocks + count, &raw_block, sizeof(struct RawBlock));
			count++;
		}
		else
			dir.blockids[ui] = INVALID_BLOCK_ID;
	}
	if(Ifs1SetBlock(symbol, id, (struct RawBlock *)&dir))
		return count;
	else
		return -1;
}

/**
	@Function:		_Ifs1GetItemList
	@Access:		Private
	@Description:
		获取指定磁盘的指定目录下的文件和文件夹的块集合。
	@Parameters:
		symbol, int8 *, IN
			盘符。
		id, uint32, IN
			目录的块ID。
		list, DSLLinkedList *, OUT
			用于储存块的列表。
	@Return:
		int32
			失败则返回-1，否则返回目录下的文件和文件夹数量的总和。
*/
static
int32
_Ifs1GetItemList(	IN int8 * symbol,
					IN uint32 id,
					OUT DSLLinkedList * list)
{
	int32 count = 0; 
	struct DirBlock dir;
	if(	id == INVALID_BLOCK_ID
		|| symbol == NULL
		|| list == NULL
		|| !Ifs1GetBlock(symbol, id, (struct RawBlock *)&dir) 
		|| dir.used == 0 
		|| dir.type != BLOCK_TYPE_DIR)
		return -1;
	uint32 ui;
	for(ui = 0; ui < sizeof(dir.blockids) / sizeof(uint32); ui++)
	{
		uint32 sub_blockid = dir.blockids[ui];
		struct RawBlock raw_block;
		if(	sub_blockid != INVALID_BLOCK_ID 
			&& Ifs1GetBlock(symbol, sub_blockid, &raw_block)
			&& raw_block.used)
		{
			struct RawBlock * new_raw_block = alloc_memory(sizeof(struct RawBlock));
			if(new_raw_block == NULL)
				return -1;
			memcpy(new_raw_block, &raw_block, sizeof(struct RawBlock));
			DSLLinkedListNode * node = dsl_lnklst_new_object_node(new_raw_block);
			dsl_lnklst_add_node(list, node);
			count++;
		}
		else
			dir.blockids[ui] = INVALID_BLOCK_ID;
	}
	if(Ifs1SetBlock(symbol, id, (struct RawBlock *)&dir))
		return count;
	else
		return -1;
}

/**
	@Function:		_Ifs1RenameDir
	@Access:		Private
	@Description:
		重命名文件夹。
	@Parameters:
		symbol, int8 *, IN
			盘符。
		id, uint32, IN
			文件夹的块ID。
		new_name, int8 *, IN
			新的文件夹名称。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。		
*/
static
BOOL
_Ifs1RenameDir(	IN int8 * symbol, 
				IN uint32 id, 
				IN int8 * new_name)
{
	struct DirBlock dir;
	if(	id == INVALID_BLOCK_ID
		|| symbol == NULL
		|| new_name == NULL
		|| strlen(new_name) > MAX_DIRNAME_LEN
		|| !Ifs1GetBlock(symbol, id, (struct RawBlock *)&dir) 
		|| dir.used == 0 
		|| dir.type != BLOCK_TYPE_DIR)
		return FALSE;
	strcpy_safe(dir.dirname, sizeof(dir.dirname), new_name);
	return Ifs1SetBlock(symbol, id, (struct RawBlock *)&dir);
}

/**
	@Function:		_Ifs1RenameFile
	@Access:		Private
	@Description:
		重命名文件。
	@Parameters:
		symbol, int8 *, IN
			盘符。
		id, uint32, IN
			文件的块ID。
		new_name, int8 *, IN
			新的文件名称。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。		
*/
static
BOOL
_Ifs1RenameFile(IN int8 * symbol, 
				IN uint32 id, 
				IN int8 * new_name)
{
	struct FileBlock file;
	if(	id == INVALID_BLOCK_ID
		|| symbol == NULL
		|| new_name == NULL
		|| strlen(new_name) > MAX_FILENAME_LEN
		|| !Ifs1GetBlock(symbol, id, (struct RawBlock *)&file)
		|| file.used == 0
		|| file.type != BLOCK_TYPE_FILE)
		return FALSE;
	strcpy_safe(file.filename, sizeof(file.filename), new_name);
	return Ifs1SetBlock(symbol, id, (struct RawBlock *)&file);
}

/**
	@Function:		_Ifs1GetSLinkTarget
	@Access:		Private
	@Description:
		获取软链接的链接目标。
		低级方式。
	@Parameters:
		symbol, int8 *, IN
			盘符。
		id, uint32, IN
			软链接的块ID。
		len, uint32, IN
			缓冲区大小。
		link, int8 *, OUT
			用于保存链接目标的缓冲区。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。		
*/
static
BOOL
_Ifs1GetSLinkTarget(IN int8 * symbol,
					IN uint32 id,
					IN uint32 len,
					OUT int8 * link)
{
	struct SLinkBlock slink;
	if(	id == INVALID_BLOCK_ID
		|| symbol == NULL
		|| link == NULL
		|| !Ifs1GetBlock(symbol, id, (struct RawBlock *)&slink)
		|| slink.used == 0
		|| slink.type != BLOCK_TYPE_SLINK)
		return FALSE;
	memcpy(link, slink.link, len);
	return TRUE;
}

/**
	@Function:		Ifs1GetSLinkTarget
	@Access:		Public
	@Description:
		获取软链接的链接目标。
		高级方式。
	@Parameters:
		path, int8 *, IN
			软链接的路径。
		len, uint32, IN
			缓冲区大小。
		link, int8 *, OUT
			用于保存链接目标的缓冲区。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。		
*/
BOOL
Ifs1GetSLinkTarget(	IN int8 * path,
					IN uint32 len,
					OUT int8 * link)
{
	int32 type;
	int8 symbol[3];
	uint32 id = Ifs1ParsePathEx(path, symbol, &type, TRUE);
	return _Ifs1GetSLinkTarget(symbol, id, len, link);
}

/**
	@Function:		Ifs1ParsePathEx
	@Access:		Public
	@Description:
		解析路径。根据路径获取盘符，块类型以及块ID。
	@Parameters:
		path, int8 *, IN
			路径。
		symbol, int8 *, OUT
			路径的盘符。
		type, int32 *, OUT
			路径所指定的资源的类型。
			如果值为 BLOCK_TYPE_FILE 则是文件，为 BLOCK_TYPE_DIR 则是目录。
		ret_slnk, BOOL, IN
			是否返回软链接。
	@Return:
		uint32
			如果返回0xffffffff则说明路径无效。否则返回块ID。
*/
uint32
Ifs1ParsePathEx(IN int8 * path, 
				OUT int8 * symbol, 
				OUT int32 * type,
				IN BOOL ret_slnk)
{
	if(	path == NULL
		|| symbol == NULL
		|| type == NULL)
		return INVALID_BLOCK_ID;
	int8 * s = path;	
	int8 chr;
	int32 state = 0;
	int8 disk[3] = {'\0', '\0', '\0'};
	int8 name[MAX_FILENAME_BUFFER_LEN];
	int8 * n = name;
	int32 processed = 0;
	*type = 0;
	uint32 blockid = INVALID_BLOCK_ID;
	name[0] = '\0';
	while((chr = *(s++)) != '\0')
		if(state == 0)
		{
			if(chr == '\0')
				break;
			disk[0] = chr;
			state = 1;
		}
		else if(state == 1)
		{
			if(chr == '\0')
				break;
			disk[1] = chr;
			state = 2;
		}
		else if(state == 2)
		{
			if(chr != ':')
				break;
			state = 3;		
		}
		else if(state == 3)
		{
			if(chr != '/')
				break;
			blockid = START_BLOCK_ID;
			*type = BLOCK_TYPE_DIR;
			processed = 1;
			state = 4;
		}
		else if(state == 4 && chr != '/')
		{
			if(strlen(name) > MAX_FILENAME_LEN)
				return INVALID_BLOCK_ID;
			*(n++) = chr;
			*n = '\0';
			processed = 0;
		}
		else if(state == 4 && chr == '/')
		{
			if(strcmp(name, "") == 0)
				return INVALID_BLOCK_ID;
			struct DirBlock dir;
			if(	!Ifs1GetBlock(disk, blockid, (struct RawBlock *)&dir) 
				|| dir.used == 0 
				|| dir.type != BLOCK_TYPE_DIR)
				return INVALID_BLOCK_ID;
			uint32 ui;
			for(ui = 0; ui < sizeof(dir.blockids) / sizeof(uint32); ui++)
			{
				uint32 block_id = dir.blockids[ui];
				struct RawBlock block;
				if(	block_id != INVALID_BLOCK_ID
					&& Ifs1GetBlock(disk, block_id, &block))
					if(	block.type == BLOCK_TYPE_DIR
						&& strcmp(((struct DirBlock *)&block)->dirname, name) == 0)
						{
							blockid = block_id;
							*type = BLOCK_TYPE_DIR;
							name[0] = '\0';
							n = name;
							processed = 1;
							break;
						}
					else if(block.type == BLOCK_TYPE_SLINK
							&& strcmp(((struct SLinkBlock *)&block)->filename, name) == 0)
					{
						uint32 ui;
						BOOL found = FALSE;
						for(ui = 0; ui < 0xffffffff; ui++)
						{
							int8 link[1024];
							link[1023] = '\0';
							if(_Ifs1GetSLinkTarget(disk, block_id, 1023, link))
								if(strlen(link) != 0 && link[strlen(link) - 1] == '/')
								{
									uint32 id = Ifs1ParsePath(link, symbol, type);
									if(id != INVALID_BLOCK_ID && *type == BLOCK_TYPE_DIR)
									{
										blockid = id;
										*type = BLOCK_TYPE_DIR;
										name[0] = '\0';
										n = name;
										processed = 1;
										found = TRUE;
										break;
									}
									else if(id != INVALID_BLOCK_ID && *type == BLOCK_TYPE_SLINK)
										block_id = Ifs1ParsePath(link, symbol, type);
									else
										return INVALID_BLOCK_ID;
								}
								else
									return INVALID_BLOCK_ID;
							else
								return INVALID_BLOCK_ID;
						}
						if(!found)
							return INVALID_BLOCK_ID;
						else
							break;
					}
			}
		}
	if(!processed)
	{
		*type = BLOCK_TYPE_FILE;
		struct DirBlock dir;
		if(	!Ifs1GetBlock(disk, blockid, (struct RawBlock *)&dir) 
			|| dir.used == 0 
			|| dir.type != BLOCK_TYPE_DIR)
			return INVALID_BLOCK_ID;
		uint32 ui;
		for(ui = 0; ui < sizeof(dir.blockids) / sizeof(uint32); ui++)
		{
			uint32 block_id = dir.blockids[ui];
			struct RawBlock block;
			if(	block_id != INVALID_BLOCK_ID 
				&& Ifs1GetBlock(disk, block_id, &block))
				if(	block.type == BLOCK_TYPE_FILE
					&& strcmp(((struct FileBlock *)&block)->filename, name) == 0)
				{
					strcpy_safe(symbol, DISK_SYMBOL_BUFFER_SIZE, disk);
					return block_id;
				}
				else if(block.type == BLOCK_TYPE_SLINK
						&& strcmp(((struct SLinkBlock *)&block)->filename, name) == 0)
					if(ret_slnk)
					{
						strcpy_safe(symbol, DISK_SYMBOL_BUFFER_SIZE, disk);
						return block_id;
					}
					else
					{
						int8 link[1024];
						link[1023] = '\0';
						if(_Ifs1GetSLinkTarget(disk, block_id, 1023, link))
							if(strlen(link) != 0 && link[strlen(link) - 1] != '/')
								return Ifs1ParsePath(link, symbol, type);
							else
								return INVALID_BLOCK_ID;
					}
		}
		return INVALID_BLOCK_ID;
	}
	strcpy_safe(symbol, DISK_SYMBOL_BUFFER_SIZE, disk);
	return blockid;
}

/**
	@Function:		Ifs1ParsePath
	@Access:		Public
	@Description:
		解析路径。根据路径获取盘符，块类型以及块ID。
	@Parameters:
		path, int8 *, IN
			路径。
		symbol, int8 *, OUT
			路径的盘符。
		type, int32 *, OUT
			路径所指定的资源的类型。
			如果值为 BLOCK_TYPE_FILE 则是文件，为 BLOCK_TYPE_DIR 则是目录。
	@Return:
		uint32
			如果返回0xffffffff则说明路径无效。否则返回块ID。
*/
uint32
Ifs1ParsePath(	IN int8 * path, 
			OUT int8 * symbol, 
			OUT int32 * type)
{
	return Ifs1ParsePathEx(path, symbol, type, FALSE);
}

/**
	@Function:		Ifs1GetItemCount
	@Access:		Public
	@Description:
		获取指定磁盘的指定目录下的文件和文件夹数量的总和。
	@Parameters:
		path, int8 *, IN
			路径。
	@Return:
		int32
			指定磁盘的指定目录下的文件和文件夹数量的总和。		
*/
int32
Ifs1GetItemCount(IN int8 * path)
{
	int32 type;
	int8 symbol[3];
	uint32 id = Ifs1ParsePath(path, symbol, &type);
	if(type != BLOCK_TYPE_DIR)
		return -1;
	return _Ifs1GetItemCount(symbol, id);
}

/**
	@Function:		Ifs1GetItems
	@Access:		Public
	@Description:
		获取指定磁盘的指定目录下的文件和文件夹的块集合。
	@Parameters:
		path, int8 *, IN
			路径。
		blocks, struct RawBlock *, OUT
			指向用于储存块集合的缓冲区。
		max, uint32, IN
			储存块集合的缓冲区最大能储存块的个数。
	@Return:
		int32
			失败则返回-1，否则返回目录下的文件和文件夹数量的总和。
*/
int32
Ifs1GetItems(	IN int8 * path, 
				OUT struct RawBlock * blocks,
				IN uint32 max)
{
	int32 type;
	int8 symbol[3];
	uint32 id = Ifs1ParsePath(path, symbol, &type);
	if(type != BLOCK_TYPE_DIR)
		return -1;
	return _Ifs1GetItems(symbol, id, blocks, max);
}

/**
	@Function:		Ifs1GetItemList
	@Access:		Public
	@Description:
		获取指定磁盘的指定目录下的文件和文件夹的块集合。
	@Parameters:
		path, int8 *, IN
			路径。
		list, DSLLinkedList *, OUT
			用于储存块的列表。
	@Return:
		int32
			失败则返回-1，否则返回目录下的文件和文件夹数量的总和。
*/
int32
Ifs1GetItemList(IN int8 * path,
				OUT DSLLinkedList * list)
{
	int32 type;
	int8 symbol[3];
	uint32 id = Ifs1ParsePath(path, symbol, &type);
	if(type != BLOCK_TYPE_DIR)
		return -1;
	return _Ifs1GetItemList(symbol, id, list);
}

/**
	@Function:		Ifs1CreateDir
	@Access:		Public
	@Description:
		在指定的磁盘的指定目录下创建文件夹。
	@Parameters:
		path, int8 *, IN
			目录路径。
		name, int8 *, IN
			将要创建的文件夹的名称。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。	
*/
BOOL
Ifs1CreateDir(	IN int8 * path, 
				IN int8 * name)
{
	int32 type;
	int8 symbol[3];
	uint32 id = Ifs1ParsePath(path, symbol, &type);
	return _Ifs1CreateDir(symbol, id, name);
}

/**
	@Function:		Ifs1CreateFile
	@Access:		Public
	@Description:
		在指定的磁盘的指定目录下创建文件。
	@Parameters:
		path, int8 *, IN
			目录路径。
		name, int8 *, IN
			将要创建的文件的名称。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。	
*/
BOOL
Ifs1CreateFile(	IN int8 * path, 
				IN int8 * name)
{
	int32 type;
	int8 symbol[3];
	uint32 id = Ifs1ParsePath(path, symbol, &type);
	return _Ifs1CreateFile(symbol, id, name);
}

/**
	@Function:		Ifs1CreateSLink
	@Access:		Public
	@Description:
		在指定的磁盘的指定目录下创建软链接。
		创建的软链接的名称不能与目录下存在的文件或文件夹名称相同。
		高级方式。
	@Parameters:
		path, int8 *, IN
			目录路径。
		name, int8 *, IN
			将要创建的软链接的名称。
		link, int8 *, IN
			软链接的目标。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。	
*/
BOOL
Ifs1CreateSLink(IN int8 * path, 
				IN int8 * name,
				IN int8 * link)
{
	int32 type;
	int8 symbol[3];
	uint32 id = Ifs1ParsePath(path, symbol, &type);
	return _Ifs1CreateSLink(symbol, id, name, link);
}

/**
	@Function:		Ifs1DeleteDir
	@Access:		Public
	@Description:
		删除指定磁盘的指定文件夹。文件夹必须不包含内容。
	@Parameters:
		path, int8 *, IN
			文件夹路径。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。	
*/
BOOL
Ifs1DeleteDir(IN int8 * path)
{
	int32 type;
	int8 symbol[3];
	uint32 id = Ifs1ParsePath(path, symbol, &type);
	if(!_Ifs1DeleteDir(symbol, id))
		return FALSE;
	int8 temp[MAX_PATH_BUFFER_LEN];
	Ifs1GetParentDir(path, temp);
	uint32 pdir_id = Ifs1ParsePath(temp, symbol, &type);
	struct DirBlock pdir;
	if(!Ifs1GetBlock(symbol, pdir_id, (struct RawBlock *)&pdir))
		return FALSE;
	uint32 ui;	
	for(ui = 0; ui < sizeof(pdir.blockids) / sizeof(uint32); ui++)
		if(pdir.blockids[ui] == id)
		{
			pdir.blockids[ui] = INVALID_BLOCK_ID;
			if(!Ifs1SetBlock(symbol, pdir_id, (struct RawBlock *)&pdir))
				return FALSE;
			break;
		}
	return TRUE;
}

/**
	@Function:		Ifs1DeleteFile
	@Access:		Public
	@Description:
		删除指定磁盘的指定文件。
	@Parameters:
		path, int8 *, IN
			文件路径。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。	
*/
BOOL
Ifs1DeleteFile(IN int8 * path)
{
	int32 type;
	int8 symbol[3];
	uint32 id = Ifs1ParsePath(path, symbol, &type);
	if(!_Ifs1DeleteFile(symbol, id))
		return FALSE;
	int8 temp[MAX_PATH_BUFFER_LEN];
	Ifs1GetFileLocation(path, temp);
	uint32 pdir_id = Ifs1ParsePath(temp, symbol, &type);
	struct DirBlock pdir;
	if(!Ifs1GetBlock(symbol, pdir_id, (struct RawBlock *)&pdir))
		return FALSE;
	uint32 ui;
	for(ui = 0; ui < sizeof(pdir.blockids) / sizeof(uint32); ui++)
		if(pdir.blockids[ui] == id)
		{
			pdir.blockids[ui] = INVALID_BLOCK_ID;
			if(!Ifs1SetBlock(symbol, pdir_id, (struct RawBlock *)&pdir))
				return FALSE;
			break;
		}
	return TRUE;
}

/**
	@Function:		Ifs1DeleteDirRecursively
	@Access:		Public
	@Description:
		删除指定磁盘的指定文件夹。
	@Parameters:
		path, int8 *, IN
			文件夹路径。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。	
*/
BOOL
Ifs1DeleteDirRecursively(IN int8 * path)
{
	uint32 count = Ifs1GetItemCount(path);
	if(count != 0)
	{
		struct RawBlock * subs = (struct RawBlock *)alloc_memory(count * sizeof(struct RawBlock));
		if(subs == NULL)
			return FALSE;
		Ifs1GetItems(path, subs, count);
		uint32 ui;		
		for(ui = 0; ui < count; ui++)
		{
			struct RawBlock * sub = subs + ui;
			if(sub->type == BLOCK_TYPE_FILE)
			{
				struct FileBlock * file = (struct FileBlock *)sub;
				int8 temp[MAX_PATH_BUFFER_LEN];
				strcpy_safe(temp, sizeof(temp), path);
				strcat_safe(temp, sizeof(temp), file->filename);
				if(!Ifs1DeleteFile(temp))
					return FALSE;
			}
			else if(sub->type == BLOCK_TYPE_DIR)
			{
				struct DirBlock * dir = (struct DirBlock *)sub;
				int8 temp[MAX_PATH_BUFFER_LEN];
				strcpy_safe(temp, sizeof(temp), path);
				strcat_safe(temp, sizeof(temp), dir->dirname);
				strcat_safe(temp, sizeof(temp), "/");
				if(!Ifs1DeleteDirRecursively(temp))
					return FALSE;
			}
			else
				return FALSE;
		}
		free_memory(subs);
	}	
	return Ifs1DeleteDir(path);
}

/**
	@Function:		Ifs1DeleteSLink
	@Access:		Public
	@Description:
		删除指定磁盘的指定软链接。
		高级方式。
	@Parameters:
		path, int8 *, IN
			软链接的路径。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。	
*/
BOOL
Ifs1DeleteSLink(IN int8 * path)
{
	int32 type;
	int8 symbol[3];
	uint32 id = Ifs1ParsePathEx(path, symbol, &type, TRUE);
	return _Ifs1DeleteSLink(symbol, id);
}

/**
	@Function:		Ifs1IsValidName
	@Access:		Public
	@Description:
		是否为合法的文件夹名称或文件名称。
	@Parameters:
		name, int8 *, IN
			名称。
	@Return:
		BOOL
			返回TRUE则合法，否则不合法。
*/
BOOL
Ifs1IsValidName(IN int8 * name)
{
	uint32 len = strlen(name);
	if(len > MAX_FILENAME_LEN || len == 0)
		return FALSE;
	uint32 ui;
	for(ui = 0; ui < len; ui++)
	{
		int8 chr = name[ui];
		if(	!(chr >= 'a' && chr <= 'z') 
			&& !(chr >= 'A' && chr <= 'Z') 
			&& !(chr >= '0' && chr <= '9') 
			&& chr != '.' && chr != '_')
			return FALSE;
	}
	return TRUE;
}

/**
	@Function:		Ifs1GetParentDir
	@Access:		Public
	@Description:
		获取指定目录路径的上一级目录路径。
	@Parameters:
		path, int8 *, IN
			目录路径。
		new_path, int8 *, OUT
			上一级目录路径。
			缓冲区大小必须大于或等于MAX_PATH_BUFFER_LEN。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。		
*/
BOOL
Ifs1GetParentDir(	IN int8 * path, 
					OUT int8 * new_path)
{
	strcpy_safe(new_path, MAX_PATH_BUFFER_LEN, path);
	uint32 len = strlen(new_path);
	if(len > 4)
	{
		new_path[len - 1] = '\0';
		int32 i;
		for(i = len - 2; i >= 0 && new_path[i] != '/'; i--);
		new_path[i + 1] = '\0';
		return TRUE;
	}
	else
		return FALSE;
}

/**
	@Function:		Ifs1GetFileLocation
	@Access:		Public
	@Description:
		获取文件路径中的目录部分。
	@Parameters:
		path, int8 *, IN
			路径。
		new_path, int8 *, OUT
			路径中得目录部分。
			缓冲区大小必须大于或等于MAX_PATH_BUFFER_LEN。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。		
*/
BOOL
Ifs1GetFileLocation(IN int8 * path, 
					OUT int8 * new_path)
{
	strcpy_safe(new_path, MAX_PATH_BUFFER_LEN, path);
	uint32 len = strlen(new_path);
	if(len > 4)
	{
		int32 i;
		for(i = len - 2; i >= 0 && new_path[i] != '/'; i--);
		new_path[i + 1] = '\0';
		return TRUE;
	}
	else
		return FALSE;
}

/**
	@Function:		Ifs1IsValidPath
	@Access:		Public
	@Description:
		检查路径是否为合法的路径。
	@Parameters:
		path, int8 *, IN
			路径。
	@Return:
		BOOL
			返回TRUE则合法，否则不合法。
*/
BOOL
Ifs1IsValidPath(IN int8 * path)
{
	int8 * s = path;
	int32 state = 0;
	int8 chr;
	int8 name[MAX_FILENAME_BUFFER_LEN];
	int8 * n = name;
	name[0] = '\0';
	while((chr = *(s++)) != '\0')
		if(state == 0)
			if(!(chr >= 'A' && chr <= 'Z'))
				return FALSE;
			else
				state = 1;
		else if(state == 1)
			if(!(chr >= 'A' && chr <= 'Z'))
				return FALSE;
			else
				state = 2;
		else if(state == 2)
			if(chr != ':')
				return FALSE;
			else
				state = 3;
		else if(state == 3)
			if(chr != '/')
				return FALSE;
			else
				state = 4;
		else if(state == 4 && chr != '/')
		{
			if(strlen(name) > MAX_FILENAME_LEN)
				return FALSE;
			*(n++) = chr;
			*n = '\0';
		}
		else if(state == 4 && chr == '/')
		{
			if(!Ifs1IsValidName(name))
				return FALSE;
			n = name;
			name[0] = '\0';
		}
	return TRUE;		
}

/**
	@Function:		Ifs1RenameDir
	@Access:		Public
	@Description:
		重命名文件夹。
	@Parameters:
		path, int8 *, IN
			文件夹的路径。
		new_name, int8 *, IN
			新的文件夹名称。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。		
*/
BOOL
Ifs1RenameDir(	IN int8 * path, 
				IN int8 * new_name)
{
	int32 type;
	int8 symbol[3];
	uint32 id = Ifs1ParsePath(path, symbol, &type);
	return _Ifs1RenameDir(symbol, id, new_name);
}

/**
	@Function:		Ifs1RenameFile
	@Access:		Public
	@Description:
		重命名文件。
	@Parameters:
		path, int8 *, IN
			文件的路径。
		new_name, int8 *, IN
			新的文件名称。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。		
*/
BOOL
Ifs1RenameFile(IN int8 * path, 
			IN int8 * new_name)
{
	int32 type;
	int8 symbol[3];
	uint32 id = Ifs1ParsePath(path, symbol, &type);
	return _Ifs1RenameFile(symbol, id, new_name);
}

/**
	@Function:		Ifs1GetAbsolutePath
	@Access:		Public
	@Description:
		根据当前路径和指定的相对路径产生一个绝对路径。
	@Parameters:
		path, int8 *, IN
			相对路径。
		current_path, int8 *, IN
			当前路径。
		new_path, int8 *, OUT
			绝对路径。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。		
*/
BOOL
Ifs1GetAbsolutePath(IN int8 * path, 
					IN int8 * current_path, 
					OUT int8 * new_path)
{
	if(strlen(path) == 4 && path[2] == ':' && path[3] == '/')
	{
		strcpy_safe(new_path, MAX_PATH_BUFFER_LEN, path);
		return TRUE;
	}
	else if(strcmp(path, "/") == 0 && strcmp(current_path, "") != 0)
	{
		strcpy_safe(new_path, MAX_PATH_BUFFER_LEN, current_path);
		new_path[4] = '\0';
		return TRUE;
	}
	else if(strcmp(current_path, "") != 0 || (strlen(path) > 4 && path[2] == ':' && path[3] == '/'))
	{
		int8 temp[MAX_PATH_BUFFER_LEN];
		strcpy_safe(temp, sizeof(temp), current_path);
		if(strlen(path) > 4 && path[2] == ':' && path[3] == '/')
		{
			memcpy_safe(temp, sizeof(temp), path, 4);
			temp[4] = '\0';
			path += 4;
		}
		else if(path[0] == '/')
		{
			temp[4] = '\0';
			path++;
		}
		int8 name[MAX_FILENAME_BUFFER_LEN];
		int8 * n = name;
		name[0] = '\0';
		int8 chr;
		int32 processed = 1;
		while((chr = *(path++)) != '\0')
			if(chr != '/')
			{
				if(strlen(name) > MAX_FILENAME_LEN)
					return FALSE;
				*(n++) = chr;
				*n = '\0';
				processed = 0;
			}
			else
			{
				if(strcmp(name, "..") == 0)
					Ifs1GetParentDir(temp, temp);
				else if(strcmp(name, ".") == 0)
					;
				else
				{
					strcat_safe(temp, sizeof(temp), name);
					strcat_safe(temp, sizeof(temp), "/");
				}
				n = name;
				name[0] = '\0';
				processed = 1;
			}
		if(!processed)
			if(strcmp(name, "..") == 0)
				Ifs1GetParentDir(temp, temp);
			else if(strcmp(name, ".") == 0)
				;
			else
				strcat_safe(temp, sizeof(temp), name);
		strcpy_safe(new_path, MAX_PATH_BUFFER_LEN, temp);
		return TRUE;
	}
	else
		return FALSE;
}

/**
	@Function:		Ifs1IsChildPath
	@Access:		Public
	@Description:
		确认一个路径是否为另一个路径的子路径。
	@Parameters:
		dir, int8 *, IN
			父目录路径。
		sub_dir, int8 *, IN
			子目录路径。
	@Return:
		BOOL
			返回TRUE则是，否则不是。		
*/
BOOL
Ifs1IsChildPath(IN int8 * dir,
				IN int8 * sub_dir)
{
	uint32 dir_len = strlen(dir);
	if(strlen(sub_dir) < dir_len)
		return FALSE;
	int8 temp[MAX_PATH_BUFFER_LEN];
	memcpy_safe(temp, sizeof(temp), sub_dir, dir_len);
	temp[dir_len] = '\0';
	if(strcmp(dir, temp) == 0)
		return TRUE;
	else
		return FALSE;
}

/**
	@Function:		Ifs1CopyFile
	@Access:		Public
	@Description:
		复制文件。
	@Parameters:
		src_path, int8 *, IN
			源文件路径。
		dst_path, int8 *, IN
			目标目录路径。
		dst_name, int8 *, IN
			目标文件文件名。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。			
*/
BOOL
Ifs1CopyFile(	IN int8 * src_path,
				IN int8 * dst_path,
				IN int8 * dst_name)
{
	int8 temp[MAX_PATH_BUFFER_LEN];
	strcpy_safe(temp, sizeof(temp), dst_path);	
	strcat_safe(temp, sizeof(temp), dst_name);
	if(	!Ifs1Exists(src_path)
		|| Ifs1Exists(temp)
		|| !Ifs1CreateFile(dst_path, dst_name))
		return FALSE;
	FileObject * src = Ifs1OpenFile(src_path, FILE_MODE_READ);
	if(src == NULL)
		return FALSE;
	FileObject * dst = Ifs1OpenFile(temp, FILE_MODE_APPEND);
	if(dst == NULL)
	{
		Ifs1CloseFile(src);
		return FALSE;
	}
	uint32 len = flen(src);
	uint8 buffer[1024];
	while(len > 0)
	{
		uint32 copy_len;
		if(len >= 1024)
			copy_len = 1024;
		else
			copy_len = len;
		if(!Ifs1ReadFile(src, buffer, copy_len))
		{
			Ifs1CloseFile(src);
			Ifs1CloseFile(dst);
			return FALSE;
		}
		if(!Ifs1AppendFile(dst, buffer, copy_len))
		{
			Ifs1CloseFile(src);
			Ifs1CloseFile(dst);
			return FALSE;
		}
		len -= copy_len;
	}
	Ifs1CloseFile(src);
	Ifs1CloseFile(dst);
	return TRUE;
}

/**
	@Function:		Ifs1DirExists
	@Access:		Public
	@Description:
		确认指定磁盘的指定目录内是否存在指定名称的文件夹。
	@Parameters:
		path, int8 *, IN
			目录路径。
		name, int8 *, IN
			要确认的文件夹的名称。
	@Return:
		BOOL
			返回TRUE则存在，否则不存在。	
*/
BOOL
Ifs1DirExists(	IN int8 * path,
				IN int8 * name)
{
	int32 type;
	int8 symbol[3];
	uint32 id = Ifs1ParsePath(path, symbol, &type);
	return _Ifs1DirExists(symbol, id, name);
}

/**
	@Function:		Ifs1FileExists
	@Access:		Public
	@Description:
		确认指定磁盘的指定目录内是否存在指定名称的文件。
	@Parameters:
		path, int8 *, IN
			目录路径。
		name, int8 *, IN
			要确认的文件的名称。
	@Return:
		BOOL
			返回TRUE则存在，否则不存在。	
*/
BOOL
Ifs1FileExists(	IN int8 * path,
				IN int8 * name)
{
	int32 type;
	int8 symbol[3];
	uint32 id = Ifs1ParsePath(path, symbol, &type);
	return _Ifs1FileExists(symbol, id, name);
}

/**
	@Function:		Ifs1Exists
	@Access:		Public
	@Description:
		检查指定路径的资源是否存在。
	@Parameters:
		path, int8 *, IN
			路径。
	@Return:
		BOOL
			返回TRUE则存在，否则不存在。	
*/
BOOL
Ifs1Exists(IN int8 * path)
{
	if(!Ifs1IsValidPath(path))
		return FALSE;
	uint32 len = strlen(path);
	if(len == 4)
	{
		int8 symbol[3];
		memcpy(symbol, path, 2);
		symbol[2] = '\0';
		return Ifs1Check(symbol);
	}
	else if(path[len - 1] == '/')
	{
		int8 temp[MAX_PATH_BUFFER_LEN];
		int8 name[MAX_DIRNAME_BUFFER_LEN];
		int32 i;
		strcpy_safe(temp, sizeof(temp), path);
		temp[len - 1] = '\0';
		len--;
		for(i = len - 1; i >= 0 && temp[i] != '/'; i--);
		strcpy_safe(name, sizeof(name), temp + i + 1);
		temp[i + 1] = '\0';
		return Ifs1DirExists(temp, name);
	}
	else
	{
		int8 temp[MAX_PATH_BUFFER_LEN];
		int8 name[MAX_FILENAME_BUFFER_LEN];
		int32 i;
		strcpy_safe(temp, sizeof(temp), path);
		for(i = len - 1; i >= 0 && path[i] != '/'; i--);
		temp[i + 1] = '\0';
		strcpy_safe(name, sizeof(name), path + i + 1);
		return Ifs1FileExists(temp, name);
	}
}

/**
	@Function:		_Ifs1OpenFileUnsafely
	@Access:		Private
	@Description:
		打开一个文件。非安全版本。
	@Parameters:
		path, int8 *, IN
			文件路径。
		mode, int32, IN
			打开模式。
	@Return:
		FileObject *
			文件结构体指针。		
*/
static
FileObject * 
_Ifs1OpenFileUnsafely(	IN int8 * path, 
						IN int32 mode)
{
	FileObject * fptr = (FileObject *)alloc_memory(sizeof(FileObject));
	if(fptr == NULL)
		return NULL;
	int32 type;
	int8 symbol[3];
	uint32 id = Ifs1ParsePath(path, symbol, &type);
	if(type != BLOCK_TYPE_FILE || id == INVALID_BLOCK_ID)
	{
		free_memory(fptr);
		return NULL;
	}
	fptr->mode = mode;
	strcpy_safe(fptr->symbol, sizeof(fptr->symbol), symbol);
	fptr->file_block_id = id;
	fptr->next_block_index = 0;
	fptr->next_block_pos = 0;
	struct FileBlock * file_block = NULL;
	file_block = (struct FileBlock *)alloc_memory(sizeof(struct FileBlock));	
	if(file_block == NULL)
	{
		free_memory(fptr);
		return NULL;
	}
	if(!Ifs1GetBlock(symbol, id, (struct RawBlock *)file_block) || file_block->lock)
	{
		free_memory(fptr->file_block);
		free_memory(fptr);
		return NULL;
	}
	file_block->lock = 1;
	if(!Ifs1SetBlock(symbol, id, (struct RawBlock *)file_block))
	{
		free_memory(fptr->file_block);
		free_memory(fptr);
		return NULL;
	}
	fptr->file_block = file_block;
	fptr->read_buffer_was_enabled = read_buffer_was_enabled;
	fptr->read_buffer_is_valid = FALSE;
	fptr->read_buffer_block_index = 0;
	fptr->append_buffer_was_enabled = append_buffer_was_enabled;
	fptr->append_buffer_byte_count = 0;
	return fptr;
}

/**
	@Function:		Ifs1OpenFile
	@Access:		Public
	@Description:
		打开一个文件。
	@Parameters:
		path, int8 *, IN
			文件路径。
		mode, int32, IN
			打开模式。
	@Return:
		FileObject *
			文件结构体指针。		
*/
FileObject * 
Ifs1OpenFile(	IN int8 * path,
				IN int32 mode)
{
	lock();
	FileObject * fptr = _Ifs1OpenFileUnsafely(path, mode);
	unlock();
	return fptr;
}

static
BOOL
_Ifs1AppendFileWithoutBufferUnsafely(IN FileObject * fptr, 
									IN uint8 * buffer, 
									IN uint32 len);

/**
	@Function:		_Ifs1CloseFileUnsafely
	@Access:		Private
	@Description:
		关闭文件。非安全版本。
	@Parameters:
		fptr, FileObject *, IN
			文件指针。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。		
*/
static
BOOL
_Ifs1CloseFileUnsafely(IN FileObject * fptr)
{
	if(fptr == NULL)
		return FALSE;
	int32 r = TRUE;
	fptr->file_block->lock = 0;
	
	if(	append_buffer_was_enabled
		&& fptr->append_buffer_was_enabled 
		&& fptr->append_buffer_byte_count != 0)
	{
		if(	!_Ifs1AppendFileWithoutBufferUnsafely(	fptr, 
													fptr->append_buffer_bytes, 
													fptr->append_buffer_byte_count))
			return FALSE;
		fptr->append_buffer_byte_count = 0;
	}

	if(!Ifs1SetBlock(fptr->symbol, fptr->file_block_id, (struct RawBlock *)(fptr->file_block)))
		r = FALSE;
	free_memory(fptr->file_block);
	free_memory(fptr);
	return r;
}

/**
	@Function:		Ifs1CloseFile
	@Access:		Public
	@Description:
		关闭文件。
	@Parameters:
		fptr, FileObject *, IN
			文件指针。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。		
*/
BOOL
Ifs1CloseFile(IN FileObject * fptr)
{
	lock();
	BOOL r = _Ifs1CloseFileUnsafely(fptr);
	unlock();
	return r;
}

/**
	@Function:		_Ifs1WriteFileUnsafely
	@Access:		Private
	@Description:
		写文件。非安全版本。
	@Parameters:
		fptr, FileObject *, IN
			文件指针。
		buffer, uint8 *, IN
			数据缓冲区。
		len, uint32, IN
			写入长度。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。		
*/
static
BOOL
_Ifs1WriteFileUnsafely(	IN FileObject * fptr, 
						IN uint8 * buffer, 
						IN uint32 len)
{
	if((fptr->mode & FILE_MODE_WRITE) == 0 || len > MAX_FILE_LEN)
		return FALSE;
	uint32 ui;
	for(ui = 0; ui < sizeof(fptr->file_block->blockids) / sizeof(uint32); ui++)
	{
		uint32 block_id = fptr->file_block->blockids[ui];
		if(block_id != INVALID_BLOCK_ID)
		{
			struct DataBlock data_block;
			data_block.used = 0;
			if(!Ifs1SetBlock(fptr->symbol, block_id, (struct RawBlock *)&data_block))
				return FALSE;
			fptr->file_block->blockids[ui] = INVALID_BLOCK_ID;
		}
	}
	int32 i;
	uint32 index = 0;
	for(i = len; i > 0; i -= DATA_BLOCK_DATA_LEN)
	{
		struct DataBlock data_block;
		data_block.used = 1;
		data_block.type = BLOCK_TYPE_DATA;
		if(i >= DATA_BLOCK_DATA_LEN)
			data_block.length = DATA_BLOCK_DATA_LEN;
		else
			data_block.length = i;
		for(ui = 0; ui < sizeof(data_block.reserve); ui++)
			data_block.reserve[ui] = 0;
		memcpy(data_block.data, buffer, data_block.length);
		buffer += data_block.length;
		uint32 data_block_id = Ifs1AddBlock(fptr->symbol, (struct RawBlock *)&data_block);
		fptr->file_block->blockids[index++] = data_block_id;
	}
	fptr->file_block->length = len;
	get_cmos_date_time(&(fptr->file_block->change));

	if((fptr->mode & FILE_MODE_READ) != 0)
	{
		// 如果打开文件的方式包含读方式，
		// 则改写文件后需要把FileObject结构体的读取缓冲区设为无效。
		fptr->read_buffer_is_valid = FALSE;
		fptr->read_buffer_block_index = 0;
	}

	return Ifs1SetBlock(fptr->symbol, fptr->file_block_id, (struct RawBlock *)fptr->file_block);
}

/**
	@Function:		Ifs1WriteFile
	@Access:		Public
	@Description:
		写文件。
	@Parameters:
		fptr, FileObject *, IN
			文件指针。
		buffer, uint8 *, IN
			数据缓冲区。
		len, uint32, IN
			写入长度。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。		
*/
BOOL
Ifs1WriteFile(	IN FileObject * fptr, 
			IN uint8 * buffer, 
			IN uint32 len)
{
	lock();
	BOOL r = _Ifs1WriteFileUnsafely(fptr, buffer, len);
	unlock();
	return r;
}

/**
	@Function:		_Ifs1ReadFileUnsafely
	@Access:		Private
	@Description:
		读文件。非安全版本。
		该版本包含读取缓冲区的功能。
	@Parameters:
		fptr, FileObject *, IN
			文件指针。
		buffer, uint8 *, OUT
			数据缓冲区。
		len, uint32, IN
			最大读入长度。
	@Return:
		uint32
			实际读入长度。		
*/
static
uint32
_Ifs1ReadFileUnsafely(	IN FileObject * fptr, 
						OUT uint8 * buffer, 
						IN uint32 len)
{
	if((fptr->mode & FILE_MODE_READ) == 0 || len == 0)
		return 0;
	uint32 real_len = 0;
	uint32 block_index = fptr->next_block_index;
	if(	!fptr->read_buffer_is_valid 
		|| fptr->read_buffer_block_index != fptr->file_block->blockids[block_index])
	{
		if(!Ifs1GetBlock(	fptr->symbol, 
							fptr->file_block->blockids[block_index], 
							(struct RawBlock *)&(fptr->read_buffer_data_block)))
			return 0;
		// 记录读取缓冲区信息。
		fptr->read_buffer_block_index = fptr->file_block->blockids[block_index];
		fptr->read_buffer_is_valid = TRUE;
	}

	while(	len > 0
			&& 	fptr->next_block_index * DATA_BLOCK_DATA_LEN + fptr->next_block_pos
				< fptr->file_block->length)
	{
		if(block_index != fptr->next_block_index)
		{
			block_index = fptr->next_block_index;
			if(	!fptr->read_buffer_is_valid 
				|| fptr->read_buffer_block_index != fptr->file_block->blockids[block_index])
			{
				if(!Ifs1GetBlock(	fptr->symbol, 
									fptr->file_block->blockids[block_index], 
									(struct RawBlock *)&(fptr->read_buffer_data_block)))
					break;
				// 记录读取缓冲区信息。
				fptr->read_buffer_block_index = fptr->file_block->blockids[block_index];
				fptr->read_buffer_is_valid = TRUE;
			}
		}
		/*
			读取优化的触发条件为：
				1. 剩余的读取字节数必须大于或等于_MAX_RW_OPTIMIZED_BYTES。
				2. 优化的读取逻辑执行完毕后，下一个读取位置必须小于或等于DATA_BLOCK_DATA_LEN。
				3. 优化的读取逻辑的最后一个字节的位置在文件内。
		*/
		if(	len >= _MAX_RW_OPTIMIZED_BYTES
			&& fptr->next_block_pos + _MAX_RW_OPTIMIZED_BYTES <= DATA_BLOCK_DATA_LEN
			&& 	fptr->next_block_index * DATA_BLOCK_DATA_LEN + fptr->next_block_pos + _MAX_RW_OPTIMIZED_BYTES - 1
				< fptr->file_block->length)
		{
			// 优化的读取逻辑。
			uint32 ui;
			uint32 nread = _MAX_RW_OPTIMIZED_BYTES / 4;
			uint32 * dst = (uint32 *)buffer;
			uint32 * src = (uint32 *)(fptr->read_buffer_data_block.data + fptr->next_block_pos);
			for(ui = 0; ui < nread; ui++)
				*(dst++) = *(src++);
			buffer += _MAX_RW_OPTIMIZED_BYTES;
			fptr->next_block_pos += _MAX_RW_OPTIMIZED_BYTES;
			if(fptr->next_block_pos == DATA_BLOCK_DATA_LEN)
			{
				fptr->next_block_index++;
				fptr->next_block_pos = 0;
			}
			len -= _MAX_RW_OPTIMIZED_BYTES;
			real_len += _MAX_RW_OPTIMIZED_BYTES;
		}
		else
		{
			// 未优化的读取逻辑。
			*(buffer++) = fptr->read_buffer_data_block.data[fptr->next_block_pos++];
			if(fptr->next_block_pos == DATA_BLOCK_DATA_LEN)
			{
				fptr->next_block_index++;
				fptr->next_block_pos = 0;
			}
			len--;
			real_len++;
		}
	}
	return real_len;
}

/**
	@Function:		_Ifs1ReadFileWithoutBufferUnsafely
	@Access:		Private
	@Description:
		读文件。非安全版本。
	@Parameters:
		fptr, FileObject *, IN
			文件指针。
			该版本不包含读取缓冲区的功能。
		buffer, uint8 *, OUT
			数据缓冲区。
		len, uint32, IN
			最大读入长度。
	@Return:
		uint32
			实际读入长度。		
*/
static
uint32
_Ifs1ReadFileWithoutBufferUnsafely(	IN FileObject * fptr, 
									OUT uint8 * buffer, 
									IN uint32 len)
{
	if((fptr->mode & FILE_MODE_READ) == 0 || len == 0)
		return 0;
	uint32 real_len = 0;
	uint32 block_index = fptr->next_block_index;
	struct DataBlock data_block;
	if(!Ifs1GetBlock(	fptr->symbol,
					fptr->file_block->blockids[block_index],
					(struct RawBlock *)&data_block))
		return 0;
	while(	len > 0
			&& 	fptr->next_block_index * DATA_BLOCK_DATA_LEN + fptr->next_block_pos
				< fptr->file_block->length)
	{
		if(block_index != fptr->next_block_index)
		{
			block_index = fptr->next_block_index;
			if(	!Ifs1GetBlock(fptr->symbol,
				fptr->file_block->blockids[block_index],
				(struct RawBlock *)&data_block))
				break;
		}
		/*
			读取优化的触发条件为：
				1. 剩余的读取字节数必须大于或等于_MAX_RW_OPTIMIZED_BYTES。
				2. 优化的读取逻辑执行完毕后，下一个读取位置必须小于或等于DATA_BLOCK_DATA_LEN。
				3. 优化的读取逻辑的最后一个字节的位置在文件内。
		*/
		if(	len >= _MAX_RW_OPTIMIZED_BYTES
			&& fptr->next_block_pos + _MAX_RW_OPTIMIZED_BYTES <= DATA_BLOCK_DATA_LEN
			&& 	fptr->next_block_index * DATA_BLOCK_DATA_LEN + fptr->next_block_pos + _MAX_RW_OPTIMIZED_BYTES - 1
				< fptr->file_block->length)
		{
			// 优化的读取逻辑。
			uint32 ui;
			uint32 nread = _MAX_RW_OPTIMIZED_BYTES / 4;
			uint32 * dst = (uint32 *)buffer;
			uint32 * src = (uint32 *)(data_block.data + fptr->next_block_pos);
			for(ui = 0; ui < nread; ui++)
				*(dst++) = *(src++);
			buffer += _MAX_RW_OPTIMIZED_BYTES;
			fptr->next_block_pos += _MAX_RW_OPTIMIZED_BYTES;
			if(fptr->next_block_pos == DATA_BLOCK_DATA_LEN)
			{
				fptr->next_block_index++;
				fptr->next_block_pos = 0;
			}
			len -= _MAX_RW_OPTIMIZED_BYTES;
			real_len += _MAX_RW_OPTIMIZED_BYTES;
		}
		else
		{
			// 未优化的读取逻辑。
			*(buffer++) = data_block.data[fptr->next_block_pos++];
			if(fptr->next_block_pos == DATA_BLOCK_DATA_LEN)
			{
				fptr->next_block_index++;
				fptr->next_block_pos = 0;
			}
			len--;
			real_len++;
		}
	}
	return real_len;
}

/**
	@Function:		Ifs1ReadFile
	@Access:		Public
	@Description:
		读文件。
	@Parameters:
		fptr, FileObject *, IN
			文件指针。
		buffer, uint8 *, OUT
			数据缓冲区。
		len, uint32, IN
			最大读入长度。
	@Return:
		uint32
			实际读入长度。		
*/
uint32
Ifs1ReadFile(	IN FileObject * fptr, 
				OUT uint8 * buffer, 
				IN uint32 len)
{
	lock();
	uint32 r = FALSE;
	if(read_buffer_was_enabled)
		r = _Ifs1ReadFileUnsafely(fptr, buffer, len);
	else
		r = _Ifs1ReadFileWithoutBufferUnsafely(fptr, buffer, len);
	unlock();
	return r;
}

/**
	@Function:		Ifs1ResetFile
	@Access:		Public
	@Description:
		重置文件的读取指针。
	@Parameters:
		fptr, FileObject *, IN
			文件指针。
	@Return:	
*/
void
Ifs1ResetFile(IN FileObject * fptr)
{
	fptr->next_block_index = 0;
	fptr->next_block_pos = 0;
}

/**
	@Function:		Ifs1IsEOF
	@Access:		Public
	@Description:
		检查文件指针是否已到达文件尾。
	@Parameters:
		fptr, FileObject *, IN
			文件指针。
	@Return:
		BOOL
			返回TRUE则到达文件尾，否则未到达。
*/
BOOL
Ifs1IsEOF(IN FileObject * fptr)
{
	uint32 next = 	fptr->next_block_index
					* DATA_BLOCK_DATA_LEN
					+ fptr->next_block_pos;
	return next == flen(fptr);
}

/**
	@Function:		_Ifs1AppendFileWithoutBufferUnsafely
	@Access:		Private
	@Description:
		追加文件。非安全版本。
		该版本不包含追加缓冲区的功能。
	@Parameters:
		fptr, FileObject *, IN
			文件指针。
		buffer, uint8 *, IN
			数据缓冲区。
		len, uint32, IN
			追加的数据的长度。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。		
*/
static
BOOL
_Ifs1AppendFileWithoutBufferUnsafely(	IN FileObject * fptr, 
										IN uint8 * buffer, 
										IN uint32 len)
{
	if(	fptr == NULL
		|| buffer == NULL
		|| len == 0
		|| (fptr->mode & FILE_MODE_APPEND) == 0 
		|| fptr->file_block->length + len > MAX_FILE_LEN)
		return FALSE;
	uint32 append_len = len;	
	uint32 block_index = fptr->file_block->length / DATA_BLOCK_DATA_LEN;
	uint32 cblock_index = block_index;
	uint32 block_pos = fptr->file_block->length % DATA_BLOCK_DATA_LEN;
	struct DataBlock data_block;
	//第一个要写入的位置是不是刚好位于下一个Data Block的开始. 如果是则先新建好下一个Data Block.
	//因为下一个Data Block不存在.
	if(block_pos == 0)
	{
		data_block.used = 1;
		data_block.type = BLOCK_TYPE_DATA;
		data_block.length = 0;
		uint32 new_block_id = Ifs1AddBlock(fptr->symbol, (struct RawBlock *)&data_block);
		if(new_block_id == INVALID_BLOCK_ID)
			return FALSE;	
		fptr->file_block->blockids[block_index] = new_block_id;
	}

	//获取下一个要写入的Data Block.
	if(!Ifs1GetBlock(fptr->symbol, fptr->file_block->blockids[cblock_index], (struct RawBlock *)&data_block))
		return FALSE;
	
	int32 is_save = 0;	//Data Block是否已保存.
	int32 is_next = 0;	//写入位置是否移动到了下一个Data Block.
	while(len > 0)
	{
		//检测是否下一个写入的位置再下一个Data Block.
		if(cblock_index != block_index)
		{
			//检测上一个写完的Data Block是否已经存在.
			if((cblock_index - 1) * DATA_BLOCK_DATA_LEN < fptr->file_block->length)
			{
				//存在. 直接保存.
				if(!Ifs1SetBlock(fptr->symbol, fptr->file_block->blockids[cblock_index - 1], (struct RawBlock *)&data_block))
					return FALSE;
			}
			else
			{
				//不存在. 新建一个Data Block并追加到File Block里.
				data_block.used = 1;
				data_block.type = BLOCK_TYPE_DATA;
				uint32 new_block_id = Ifs1AddBlock(fptr->symbol, (struct RawBlock *)&data_block);
				if(new_block_id == INVALID_BLOCK_ID)
					return FALSE;
				fptr->file_block->blockids[cblock_index - 1] = new_block_id;
			}
			is_save = 1;
			data_block.length = 0;
			block_pos = 0;
			block_index++;
		}
		data_block.data[block_pos++] = *(buffer++);
		data_block.length++;
		len--;
		is_save = 0;
		is_next = 0;
		//检查一个Data Block是否已写满
		if(data_block.length == DATA_BLOCK_DATA_LEN)
		{
			cblock_index++;
			is_next = 1;
		}
	}
	//检查是否未保存Data Block.
	if(!is_save)
	{
		//获取未保存的Data Block的ID.
		uint32 id = fptr->file_block->blockids[(is_next ? cblock_index - 1 : cblock_index)];
		
		//检查未保存Data Block是否存在.
		if(id != INVALID_BLOCK_ID)
		{
			if(!Ifs1SetBlock(fptr->symbol, id, (struct RawBlock *)&data_block))
				return FALSE;
		}
		else
		{
			id = Ifs1AddBlock(fptr->symbol, (struct RawBlock *)&data_block);
			if(id == INVALID_BLOCK_ID)
				return FALSE;
			fptr->file_block->blockids[(is_next ? cblock_index - 1 : cblock_index)] = id;
		}
	}
	fptr->file_block->length += append_len;
	get_cmos_date_time(&(fptr->file_block->change));

	if((fptr->mode & FILE_MODE_READ) != 0)
	{
		// 如果打开文件的方式包含读方式，
		// 则改写文件后需要把FileObject结构体的读取缓冲区设为无效。
		fptr->read_buffer_is_valid = FALSE;
		fptr->read_buffer_block_index = 0;
	}

	return Ifs1SetBlock(fptr->symbol, fptr->file_block_id, (struct RawBlock *)(fptr->file_block));
}

/**
	@Function:		_Ifs1AppendFileUnsafely
	@Access:		Private
	@Description:
		追加文件。非安全版本。
		该版本包含追加缓冲区的功能。
	@Parameters:
		fptr, FileObject *, IN
			文件指针。
		buffer, uint8 *, IN
			数据缓冲区。
		len, uint32, IN
			追加的数据的长度。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。		
*/
static
BOOL
_Ifs1AppendFileUnsafely(IN FileObject * fptr, 
						IN uint8 * buffer, 
						IN uint32 len)
{
	if(	fptr == NULL
		|| buffer == NULL
		|| len == 0
		|| (fptr->mode & FILE_MODE_APPEND) == 0 
		|| fptr->file_block->length + len > MAX_FILE_LEN)
		return FALSE;
	if(fptr->append_buffer_byte_count + len > sizeof(fptr->append_buffer_bytes))
	{
		if(fptr->append_buffer_byte_count != 0)
			if(!_Ifs1AppendFileWithoutBufferUnsafely(	fptr, 
														fptr->append_buffer_bytes, 
														fptr->append_buffer_byte_count))
				return FALSE;
		if(len > sizeof(fptr->append_buffer_bytes))
		{
			if(!_Ifs1AppendFileWithoutBufferUnsafely(fptr, buffer, len))
				return FALSE;
			fptr->append_buffer_byte_count = 0;
		}
		else
		{
			memcpy(fptr->append_buffer_bytes, buffer, len);
			fptr->append_buffer_byte_count = len;
		}
	}
	else
	{
		memcpy(	fptr->append_buffer_bytes + fptr->append_buffer_byte_count,
				buffer,
				len);
		fptr->append_buffer_byte_count += len;
	}
	return TRUE;
}

/**
	@Function:		Ifs1AppendFile
	@Access:		Public
	@Description:
		追加文件。
	@Parameters:
		fptr, FileObject *, IN
			文件指针。
		buffer, uint8 *, IN
			数据缓冲区。
		len, uint32, IN
			追加的数据的长度。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
Ifs1AppendFile(	IN FileObject * fptr, 
				IN uint8 * buffer, 
				IN uint32 len)
{
	lock();
	BOOL r = FALSE;
	if(append_buffer_was_enabled)
		r = _Ifs1AppendFileUnsafely(fptr, buffer, len);
	else
		r = _Ifs1AppendFileWithoutBufferUnsafely(fptr, buffer, len);
	unlock();
	return r;
}

static int8 repairing_path[MAX_PATH_BUFFER_LEN];

#define	_RESUME_REPAIRING_PATH()	\
{	\
	repairing_path[strlen(repairing_path) - strlen(dir->dirname) - 1] = '\0';\
}

/**
	@Function:		_Ifs1RepairFiles
	@Access:		Private
	@Description:
		修复指定目录下的文件以及文件夹。
	@Parameters:
		symbol, int8 *, IN
			盘符。
		dir, struct DirBlock *, IN
			目录块ID。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。		
*/
static
BOOL
_Ifs1RepairFiles(	IN int8 * symbol, 
					IN struct DirBlock * dir)
{
	int32 r = TRUE;
	uint32 * blockids = dir->blockids;
	uint32 ui;
	if(strcmp(dir->dirname, "/") != 0)
	{
		strcat_safe(repairing_path, sizeof(repairing_path), dir->dirname);
		strcat_safe(repairing_path, sizeof(repairing_path), "/");
	}
	for(ui = 0; ui < sizeof(dir->blockids) / sizeof(uint32); ui++)
	{		
		uint32 id = blockids[ui];
		if(id == INVALID_BLOCK_ID)
			continue;
		struct RawBlock raw_block;
		if(!Ifs1GetBlock(symbol, id, &raw_block))
		{
			//无法通过symbol和id获取一个块
			r = FALSE;
			continue;
		}
		if(raw_block.used)
			switch(raw_block.type)
			{
				case BLOCK_TYPE_FILE:
				{
					struct FileBlock * file_block = (struct FileBlock *)&raw_block;				
					if(file_block->lock)
					{
						file_block->lock = 0;
						int8 temp_path[MAX_PATH_BUFFER_LEN];
						strcpy_safe(temp_path, sizeof(temp_path), repairing_path);
						strcat_safe(temp_path, sizeof(temp_path), file_block->filename);
						if(Ifs1SetBlock(symbol, id, &raw_block))
						{
							print_str_p("Unlocked ", CC_GREEN);
							print_str(temp_path);
							print_str("\n");
						}
						else
						{
							print_str_p("Failed to unlock ", CC_RED);
							print_str(temp_path);
							print_str("\n");
						}
					}
					break;
				}
				case BLOCK_TYPE_DIR:
					if(!_Ifs1RepairFiles(symbol, (struct DirBlock *)&raw_block))
						r = FALSE;	//修复该子文件夹时发生错误
					break;
				default:
					r = FALSE;	//文件夹块ID列表包含一个错误的类型的块
					break;
			}
		else
			r = FALSE;	//文件夹的块列表包含一个错误的ID
	}
	_RESUME_REPAIRING_PATH();
	return r;	//返回
}

/**
	@Function:		Ifs1Repair
	@Access:		Public
	@Description:
		修复指定磁盘的IFS1文件系统。
	@Parameters:
		symbol, int8 *, IN
			盘符。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
Ifs1Repair(IN int8 * symbol)
{
	struct DirBlock dir;
	if(	!Ifs1GetBlock(symbol, START_BLOCK_ID, (struct RawBlock *)&dir)
		|| dir.used == 0
		|| dir.type != BLOCK_TYPE_DIR)
		return FALSE;
	strcpy_safe(repairing_path, sizeof(repairing_path), symbol);
	strcat_safe(repairing_path, sizeof(repairing_path), ":/");
	return _Ifs1RepairFiles(symbol, &dir);
}
