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

static BOOL read_buffer_was_enabled		= FALSE;
static BOOL append_buffer_was_enabled	= FALSE;

/**
	@Function:		ifs1fs_init
	@Access:		Public
	@Description:
		初始化IFS1文件系统。
	@Parameters:
	@Return:
		BOOL
			返回TRUE则成功，否则失败。		
*/
BOOL
ifs1fs_init(void)
{
	config_system_ifs1_get_bool("EnableReadBuffer", 
								&read_buffer_was_enabled);
	config_system_ifs1_get_bool("EnableAppendBuffer",
								&append_buffer_was_enabled);
}

/**
	@Function:		format_disk
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
format_disk(IN int8 * symbol)
{
	if(symbol == NULL || !del_all_blocks(symbol))
		return FALSE;
	struct DirBlock dir;
	fill_dir_block("/", &dir);
	if(add_block(symbol, (struct RawBlock *)&dir) != INVALID_BLOCK_ID)
		return TRUE;
	else
		return FALSE;
}

/**
	@Function:		check_disk
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
check_disk(IN int8 * symbol)
{
	struct DirBlock dir;
	if(	symbol == NULL 
		|| !get_block(symbol, START_BLOCK_ID, (struct RawBlock *)&dir))
		return FALSE;
	if(dir.type == BLOCK_TYPE_DIR && strcmp(dir.dirname, "/") == 0)
		return TRUE;
	else
		return FALSE;
}

/**
	@Function:		_exists_dir
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
_exists_dir(IN int8 * symbol, 
			IN uint32 id, 
			IN int8 * name)
{
	struct DirBlock dir;
	if(	id == INVALID_BLOCK_ID
		|| symbol == NULL
		|| name == NULL
		|| !get_block(symbol, id, (struct RawBlock *)&dir) 
		|| dir.used == 0 
		|| dir.type != BLOCK_TYPE_DIR)
		return FALSE;
	uint32 ui;
	for(ui = 0; ui < sizeof(dir.blockids) / sizeof(uint32); ui++)
	{
		uint32 subdir_id = dir.blockids[ui];
		struct DirBlock subdir;
		if(	subdir_id != INVALID_BLOCK_ID 
			&& get_block(symbol, subdir_id, (struct RawBlock *)&subdir) 
			&& subdir.used
			&& subdir.type == BLOCK_TYPE_DIR
			&& strcmp(subdir.dirname, name) == 0)
			return TRUE;
	}
	return FALSE;
}

/**
	@Function:		_exists_file
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
_exists_file(	IN int8 * symbol, 
				IN uint32 id, 
				IN int8 * name)
{
	struct DirBlock dir;
	if(	id == INVALID_BLOCK_ID
		|| symbol == NULL
		|| name == NULL
		|| !get_block(symbol, id, (struct RawBlock *)&dir) 
		|| dir.used == 0 
		|| dir.type != BLOCK_TYPE_DIR)
		return FALSE;
	uint32 ui;
	for(ui = 0; ui < sizeof(dir.blockids) / sizeof(uint32); ui++)
	{
		uint32 subfile_id = dir.blockids[ui];
		struct FileBlock subfile;
		if(	subfile_id != INVALID_BLOCK_ID 
			&& get_block(symbol, subfile_id, (struct RawBlock *)&subfile)
			&& subfile.used
			&& subfile.type == BLOCK_TYPE_FILE
			&& strcmp(subfile.filename, name) == 0)
			return TRUE;
	}
	return FALSE;
}

/**
	@Function:		_exists_slink
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
_exists_slink(	IN int8 * symbol,
				IN uint32 id,
				IN int8 * name)
{
	struct DirBlock dir;
	if(	id == INVALID_BLOCK_ID
		|| symbol == NULL
		|| name == NULL
		|| !get_block(symbol, id, (struct RawBlock *)&dir) 
		|| dir.used == 0 
		|| dir.type != BLOCK_TYPE_DIR)
		return FALSE;
	uint32 ui;
	for(ui = 0; ui < sizeof(dir.blockids) / sizeof(uint32); ui++)
	{
		uint32 slink_id = dir.blockids[ui];
		struct SLinkBlock slink;
		if(	slink_id != INVALID_BLOCK_ID 
			&& get_block(symbol, slink_id, (struct RawBlock *)&slink)
			&& slink.used
			&& slink.type == BLOCK_TYPE_SLINK
			&& strcmp(slink.filename, name) == 0)
			return TRUE;
	}
	return FALSE;
}

/**
	@Function:		_create_dir
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
_create_dir(IN int8 * symbol, 
			IN uint32 id, 
			IN int8 * name)
{
	struct DirBlock dir;
	if(	id == INVALID_BLOCK_ID
		|| symbol == NULL
		|| name == NULL
		|| strlen(name) > MAX_DIRNAME_LEN
		|| !get_block(symbol, id, (struct RawBlock *)&dir) 
		|| dir.used == 0 
		|| dir.type != BLOCK_TYPE_DIR
		|| _exists_slink(symbol, id, name)
		|| _exists_dir(symbol, id, name))
		return FALSE;
	struct DirBlock new_dir;
	fill_dir_block(name, &new_dir);
	uint32 new_dir_id = add_block(symbol, (struct RawBlock *)&new_dir);
	if(new_dir_id == INVALID_BLOCK_ID)
		return FALSE;
	uint32 ui;
	for(ui = 0; ui < sizeof(dir.blockids) / sizeof(uint32); ui++)
		if(dir.blockids[ui] == INVALID_BLOCK_ID)
		{
			dir.blockids[ui] = new_dir_id;
			set_block(symbol, id, (struct RawBlock *)&dir);
			return TRUE;
		}
	return FALSE;
}

/**
	@Function:		_create_file
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
_create_file(	IN int8 * symbol, 
				IN uint32 id, 
				IN int8 * name)
{
	struct DirBlock dir;
	if(	id == INVALID_BLOCK_ID
		|| symbol == NULL
		|| name == NULL
		|| strlen(name) > MAX_FILENAME_LEN
		|| !get_block(symbol, id, (struct RawBlock *)&dir) 
		|| dir.used == 0 
		|| dir.type != BLOCK_TYPE_DIR
		|| _exists_slink(symbol, id, name)
		|| _exists_file(symbol, id, name))
		return FALSE;
	struct FileBlock new_file;
	fill_file_block(name, &new_file);
	uint32 new_file_id = add_block(symbol, (struct RawBlock *)&new_file);
	if(new_file_id == INVALID_BLOCK_ID)
		return FALSE;
	uint32 ui;
	for(ui = 0; ui < sizeof(dir.blockids) / sizeof(uint32); ui++)
		if(dir.blockids[ui] == INVALID_BLOCK_ID)
		{
			dir.blockids[ui] = new_file_id;
			set_block(symbol, id, (struct RawBlock *)&dir);
			return TRUE;
		}
	return FALSE;
}

/**
	@Function:		_create_slink
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
_create_slink(	IN int8 * symbol,
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
		|| !get_block(symbol, id, (struct RawBlock *)&dir) 
		|| dir.used == 0 
		|| dir.type != BLOCK_TYPE_DIR
		|| _exists_slink(symbol, id, name)
		|| _exists_file(symbol, id, name)
		|| _exists_dir(symbol, id, name))
		return FALSE;
	struct SLinkBlock new_slink;
	fill_slink_block(name, link, &new_slink);
	uint32 new_slink_id = add_block(symbol, (struct RawBlock *)&new_slink);
	if(new_slink_id == INVALID_BLOCK_ID)
		return FALSE;
	uint32 ui;
	for(ui = 0; ui < sizeof(dir.blockids) / sizeof(uint32); ui++)
		if(dir.blockids[ui] == INVALID_BLOCK_ID)
		{
			dir.blockids[ui] = new_slink_id;
			set_block(symbol, id, (struct RawBlock *)&dir);
			return TRUE;
		}
	return FALSE;
}

/**
	@Function:		_del_dir
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
_del_dir(	IN int8 * symbol, 
			IN uint32 id)
{
	struct DirBlock dir;
	if(	id == INVALID_BLOCK_ID
		|| symbol == NULL
		|| !get_block(symbol, id, (struct RawBlock *)&dir) 
		|| dir.used == 0 
		|| dir.type != BLOCK_TYPE_DIR)
		return FALSE;
	uint32 ui;	
	for(ui = 0; ui < sizeof(dir.blockids) / sizeof(uint32); ui++)
		if(dir.blockids[ui] != INVALID_BLOCK_ID)
			return FALSE;
	dir.used = 0;
	return set_block(symbol, id, (struct RawBlock *)&dir);
}

/**
	@Function:		_del_file
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
_del_file(	IN int8 * symbol, 
			IN uint32 id)
{
	struct FileBlock file;
	if(	id == INVALID_BLOCK_ID
		|| symbol == NULL
		|| !get_block(symbol, id, (struct RawBlock *)&file)
		|| file.used == 0
		|| file.type != BLOCK_TYPE_FILE
		|| file.lock)
		return FALSE;
	file.used = 0;
	return set_block(symbol, id, (struct RawBlock *)&file);
}

/**
	@Function:		_del_slink
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
_del_slink(	IN int8 * symbol,
			IN uint32 id)
{
	struct SLinkBlock slink;
	if(	id == INVALID_BLOCK_ID
		|| symbol == NULL
		|| !get_block(symbol, id, (struct RawBlock *)&slink)
		|| slink.used == 0
		|| slink.type != BLOCK_TYPE_SLINK)
		return FALSE;
	slink.used = 0;
	return set_block(symbol, id, (struct RawBlock *)&slink);
}

/**
	@Function:		_df_count
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
_df_count(	IN int8 * symbol, 
			IN uint32 id)
{
	int32 count = 0; 
	struct DirBlock dir;
	if(	id == INVALID_BLOCK_ID
		|| symbol == NULL
		|| !get_block(symbol, id, (struct RawBlock *)&dir) 
		|| dir.used == 0 
		|| dir.type != BLOCK_TYPE_DIR)
		return -1;
	uint32 ui;
	for(ui = 0; ui < sizeof(dir.blockids) / sizeof(uint32); ui++)
	{
		uint32 sub_block_id = dir.blockids[ui];
		struct RawBlock sub_block;
		if(	sub_block_id != INVALID_BLOCK_ID
			&& get_block(symbol, sub_block_id, &sub_block)
			&& sub_block.used)
			count++;
	}
	return count;
}

/**
	@Function:		_df
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
_df(IN int8 * symbol, 
	IN uint32 id, 
	OUT struct RawBlock * blocks,
	IN uint32 max)
{
	int32 count = 0; 
	struct DirBlock dir;
	if(	id == INVALID_BLOCK_ID
		|| symbol == NULL
		|| blocks == NULL
		|| !get_block(symbol, id, (struct RawBlock *)&dir) 
		|| dir.used == 0 
		|| dir.type != BLOCK_TYPE_DIR)
		return -1;
	uint32 ui;
	for(ui = 0; ui < sizeof(dir.blockids) / sizeof(uint32) && ui < max; ui++)
	{
		uint32 sub_blockid = dir.blockids[ui];
		struct RawBlock raw_block;
		if(	sub_blockid != INVALID_BLOCK_ID 
			&& get_block(symbol, sub_blockid, &raw_block)
			&& raw_block.used)
		{
			memcpy(blocks + count, &raw_block, sizeof(struct RawBlock));
			count++;
		}
		else
			dir.blockids[ui] = INVALID_BLOCK_ID;
	}
	if(set_block(symbol, id, (struct RawBlock *)&dir))
		return count;
	else
		return -1;
}

/**
	@Function:		_dir_list
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
_dir_list(	IN int8 * symbol,
			IN uint32 id,
			OUT DSLLinkedList * list)
{
	int32 count = 0; 
	struct DirBlock dir;
	if(	id == INVALID_BLOCK_ID
		|| symbol == NULL
		|| list == NULL
		|| !get_block(symbol, id, (struct RawBlock *)&dir) 
		|| dir.used == 0 
		|| dir.type != BLOCK_TYPE_DIR)
		return -1;
	uint32 ui;
	for(ui = 0; ui < sizeof(dir.blockids) / sizeof(uint32); ui++)
	{
		uint32 sub_blockid = dir.blockids[ui];
		struct RawBlock raw_block;
		if(	sub_blockid != INVALID_BLOCK_ID 
			&& get_block(symbol, sub_blockid, &raw_block)
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
	if(set_block(symbol, id, (struct RawBlock *)&dir))
		return count;
	else
		return -1;
}

/**
	@Function:		_rename_dir
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
_rename_dir(IN int8 * symbol, 
			IN uint32 id, 
			IN int8 * new_name)
{
	struct DirBlock dir;
	if(	id == INVALID_BLOCK_ID
		|| symbol == NULL
		|| new_name == NULL
		|| strlen(new_name) > MAX_DIRNAME_LEN
		|| !get_block(symbol, id, (struct RawBlock *)&dir) 
		|| dir.used == 0 
		|| dir.type != BLOCK_TYPE_DIR)
		return FALSE;
	strcpy_safe(dir.dirname, sizeof(dir.dirname), new_name);
	return set_block(symbol, id, (struct RawBlock *)&dir);
}

/**
	@Function:		_rename_file
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
_rename_file(	IN int8 * symbol, 
				IN uint32 id, 
				IN int8 * new_name)
{
	struct FileBlock file;
	if(	id == INVALID_BLOCK_ID
		|| symbol == NULL
		|| new_name == NULL
		|| strlen(new_name) > MAX_FILENAME_LEN
		|| !get_block(symbol, id, (struct RawBlock *)&file)
		|| file.used == 0
		|| file.type != BLOCK_TYPE_FILE)
		return FALSE;
	strcpy_safe(file.filename, sizeof(file.filename), new_name);
	return set_block(symbol, id, (struct RawBlock *)&file);
}

/**
	@Function:		_get_slink_link
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
_get_slink_link(IN int8 * symbol,
				IN uint32 id,
				IN uint32 len,
				OUT int8 * link)
{
	struct SLinkBlock slink;
	if(	id == INVALID_BLOCK_ID
		|| symbol == NULL
		|| link == NULL
		|| !get_block(symbol, id, (struct RawBlock *)&slink)
		|| slink.used == 0
		|| slink.type != BLOCK_TYPE_SLINK)
		return FALSE;
	memcpy(link, slink.link, len);
	return TRUE;
}

/**
	@Function:		get_slink_link
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
get_slink_link(	IN int8 * path,
				IN uint32 len,
				OUT int8 * link)
{
	int32 type;
	int8 symbol[3];
	uint32 id = parse_path_ex(path, symbol, &type, TRUE);
	return _get_slink_link(symbol, id, len, link);
}

/**
	@Function:		parse_path_ex
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
parse_path_ex(	IN int8 * path, 
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
			if(	!get_block(disk, blockid, (struct RawBlock *)&dir) 
				|| dir.used == 0 
				|| dir.type != BLOCK_TYPE_DIR)
				return INVALID_BLOCK_ID;
			uint32 ui;
			for(ui = 0; ui < sizeof(dir.blockids) / sizeof(uint32); ui++)
			{
				uint32 block_id = dir.blockids[ui];
				struct RawBlock block;
				if(	block_id != INVALID_BLOCK_ID
					&& get_block(disk, block_id, &block))
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
							if(_get_slink_link(disk, block_id, 1023, link))
								if(strlen(link) != 0 && link[strlen(link) - 1] == '/')
								{
									uint32 id = parse_path(link, symbol, type);
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
										block_id = parse_path(link, symbol, type);
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
		if(	!get_block(disk, blockid, (struct RawBlock *)&dir) 
			|| dir.used == 0 
			|| dir.type != BLOCK_TYPE_DIR)
			return INVALID_BLOCK_ID;
		uint32 ui;
		for(ui = 0; ui < sizeof(dir.blockids) / sizeof(uint32); ui++)
		{
			uint32 block_id = dir.blockids[ui];
			struct RawBlock block;
			if(	block_id != INVALID_BLOCK_ID 
				&& get_block(disk, block_id, &block))
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
						if(_get_slink_link(disk, block_id, 1023, link))
							if(strlen(link) != 0 && link[strlen(link) - 1] != '/')
								return parse_path(link, symbol, type);
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
	@Function:		parse_path
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
parse_path(	IN int8 * path, 
			OUT int8 * symbol, 
			OUT int32 * type)
{
	return parse_path_ex(path, symbol, type, FALSE);
}

/**
	@Function:		df_count
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
df_count(IN int8 * path)
{
	int32 type;
	int8 symbol[3];
	uint32 id = parse_path(path, symbol, &type);
	if(type != BLOCK_TYPE_DIR)
		return -1;
	return _df_count(symbol, id);
}

/**
	@Function:		df
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
df(	IN int8 * path, 
	OUT struct RawBlock * blocks,
	IN uint32 max)
{
	int32 type;
	int8 symbol[3];
	uint32 id = parse_path(path, symbol, &type);
	if(type != BLOCK_TYPE_DIR)
		return -1;
	return _df(symbol, id, blocks, max);
}

/**
	@Function:		dir_list
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
dir_list(	IN int8 * path,
			OUT DSLLinkedList * list)
{
	int32 type;
	int8 symbol[3];
	uint32 id = parse_path(path, symbol, &type);
	if(type != BLOCK_TYPE_DIR)
		return -1;
	return _dir_list(symbol, id, list);
}

/**
	@Function:		create_dir
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
create_dir(	IN int8 * path, 
			IN int8 * name)
{
	int32 type;
	int8 symbol[3];
	uint32 id = parse_path(path, symbol, &type);
	return _create_dir(symbol, id, name);
}

/**
	@Function:		create_file
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
create_file(IN int8 * path, 
			IN int8 * name)
{
	int32 type;
	int8 symbol[3];
	uint32 id = parse_path(path, symbol, &type);
	return _create_file(symbol, id, name);
}

/**
	@Function:		create_slink
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
create_slink(	IN int8 * path, 
				IN int8 * name,
				IN int8 * link)
{
	int32 type;
	int8 symbol[3];
	uint32 id = parse_path(path, symbol, &type);
	return _create_slink(symbol, id, name, link);
}

/**
	@Function:		del_dir
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
del_dir(IN int8 * path)
{
	int32 type;
	int8 symbol[3];
	uint32 id = parse_path(path, symbol, &type);
	if(!_del_dir(symbol, id))
		return FALSE;
	int8 temp[MAX_PATH_BUFFER_LEN];
	prev_dir(path, temp);
	uint32 pdir_id = parse_path(temp, symbol, &type);
	struct DirBlock pdir;
	if(!get_block(symbol, pdir_id, (struct RawBlock *)&pdir))
		return FALSE;
	uint32 ui;	
	for(ui = 0; ui < sizeof(pdir.blockids) / sizeof(uint32); ui++)
		if(pdir.blockids[ui] == id)
		{
			pdir.blockids[ui] = INVALID_BLOCK_ID;
			if(!set_block(symbol, pdir_id, (struct RawBlock *)&pdir))
				return FALSE;
			break;
		}
	return TRUE;
}

/**
	@Function:		del_file
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
del_file(IN int8 * path)
{
	int32 type;
	int8 symbol[3];
	uint32 id = parse_path(path, symbol, &type);
	if(!_del_file(symbol, id))
		return FALSE;
	int8 temp[MAX_PATH_BUFFER_LEN];
	file_dir(path, temp);
	uint32 pdir_id = parse_path(temp, symbol, &type);
	struct DirBlock pdir;
	if(!get_block(symbol, pdir_id, (struct RawBlock *)&pdir))
		return FALSE;
	uint32 ui;
	for(ui = 0; ui < sizeof(pdir.blockids) / sizeof(uint32); ui++)
		if(pdir.blockids[ui] == id)
		{
			pdir.blockids[ui] = INVALID_BLOCK_ID;
			if(!set_block(symbol, pdir_id, (struct RawBlock *)&pdir))
				return FALSE;
			break;
		}
	return TRUE;
}

/**
	@Function:		del_dirs
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
del_dirs(IN int8 * path)
{
	uint32 count = df_count(path);
	if(count != 0)
	{
		struct RawBlock * subs = (struct RawBlock *)alloc_memory(count * sizeof(struct RawBlock));
		if(subs == NULL)
			return FALSE;
		df(path, subs, count);
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
				if(!del_file(temp))
					return FALSE;
			}
			else if(sub->type == BLOCK_TYPE_DIR)
			{
				struct DirBlock * dir = (struct DirBlock *)sub;
				int8 temp[MAX_PATH_BUFFER_LEN];
				strcpy_safe(temp, sizeof(temp), path);
				strcat_safe(temp, sizeof(temp), dir->dirname);
				strcat_safe(temp, sizeof(temp), "/");
				if(!del_dirs(temp))
					return FALSE;
			}
			else
				return FALSE;
		}
		free_memory(subs);
	}	
	return del_dir(path);
}

/**
	@Function:		del_slink
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
del_slink(IN int8 * path)
{
	int32 type;
	int8 symbol[3];
	uint32 id = parse_path_ex(path, symbol, &type, TRUE);
	return _del_slink(symbol, id);
}

/**
	@Function:		is_valid_df_name
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
is_valid_df_name(IN int8 * name)
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
	@Function:		prev_dir
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
prev_dir(	IN int8 * path, 
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
	@Function:		file_dir
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
file_dir(	IN int8 * path, 
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
	@Function:		is_valid_path
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
is_valid_path(IN int8 * path)
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
			if(!is_valid_df_name(name))
				return FALSE;
			n = name;
			name[0] = '\0';
		}
	return TRUE;		
}

/**
	@Function:		rename_dir
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
rename_dir(	IN int8 * path, 
			IN int8 * new_name)
{
	int32 type;
	int8 symbol[3];
	uint32 id = parse_path(path, symbol, &type);
	return _rename_dir(symbol, id, new_name);
}

/**
	@Function:		rename_file
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
rename_file(IN int8 * path, 
			IN int8 * new_name)
{
	int32 type;
	int8 symbol[3];
	uint32 id = parse_path(path, symbol, &type);
	return _rename_file(symbol, id, new_name);
}

/**
	@Function:		fix_path
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
fix_path(	IN int8 * path, 
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
					prev_dir(temp, temp);
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
				prev_dir(temp, temp);
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
	@Function:		is_sub_dir
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
is_sub_dir(	IN int8 * dir,
			IN int8 * sub_dir)
{
	if(strlen(sub_dir) < strlen(dir))
		return FALSE;
	int8 temp[MAX_PATH_BUFFER_LEN];
	strncpy(temp, sub_dir, strlen(dir));
	temp[strlen(dir)] = '\0';
	if(strcmp(dir, temp) == 0)
		return TRUE;
	else
		return FALSE;
}

/**
	@Function:		copy_file
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
copy_file(	IN int8 * src_path, 
			IN int8 * dst_path, 
			IN int8 * dst_name)
{
	int8 temp[MAX_PATH_BUFFER_LEN];
	strcpy_safe(temp, sizeof(temp), dst_path);	
	strcat_safe(temp, sizeof(temp), dst_name);
	if(	!exists_df(src_path)
		|| exists_df(temp)
		|| !create_file(dst_path, dst_name))
		return FALSE;
	FileObject * src = open_file(src_path, FILE_MODE_READ);
	if(src == NULL)
		return FALSE;
	FileObject * dst = open_file(temp, FILE_MODE_APPEND);
	if(dst == NULL)
	{
		close_file(src);
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
		if(!read_file(src, buffer, copy_len))
		{
			close_file(src);
			close_file(dst);
			return FALSE;
		}
		if(!append_file(dst, buffer, copy_len))
		{
			close_file(src);
			close_file(dst);
			return FALSE;
		}
		len -= copy_len;
	}
	close_file(src);
	close_file(dst);
	return TRUE;
}

/**
	@Function:		exists_dir
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
exists_dir(	IN int8 * path,
			IN int8 * name)
{
	int32 type;
	int8 symbol[3];
	uint32 id = parse_path(path, symbol, &type);
	return _exists_dir(symbol, id, name);
}

/**
	@Function:		exists_file
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
exists_file(IN int8 * path,
			IN int8 * name)
{
	int32 type;
	int8 symbol[3];
	uint32 id = parse_path(path, symbol, &type);
	return _exists_file(symbol, id, name);
}

/**
	@Function:		exists_df
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
exists_df(IN int8 * path)
{
	if(!is_valid_path(path))
		return FALSE;
	uint32 len = strlen(path);
	if(len == 4)
	{
		int8 symbol[3];
		memcpy(symbol, path, 2);
		symbol[2] = '\0';
		return check_disk(symbol);
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
		return exists_dir(temp, name);
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
		return exists_file(temp, name);
	}
}

/**
	@Function:		_open_file_unsafe
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
_open_file_unsafe(	IN int8 * path, 
					IN int32 mode)
{
	FileObject * fptr = (FileObject *)alloc_memory(sizeof(FileObject));
	if(fptr == NULL)
		return NULL;
	int32 type;
	int8 symbol[3];
	uint32 id = parse_path(path, symbol, &type);
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
	if(!get_block(symbol, id, (struct RawBlock *)file_block) || file_block->lock)
	{
		free_memory(fptr->file_block);
		free_memory(fptr);
		return NULL;
	}
	file_block->lock = 1;
	if(!set_block(symbol, id, (struct RawBlock *)file_block))
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
	@Function:		open_file
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
open_file(	IN int8 * path, 
			IN int32 mode)
{
	lock();
	FileObject * fptr = _open_file_unsafe(path, mode);
	unlock();
	return fptr;
}

static
BOOL
_append_file_without_buffer_unsafe(	IN FileObject * fptr, 
									IN uint8 * buffer, 
									IN uint32 len);

/**
	@Function:		_close_file_unsafe
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
_close_file_unsafe(IN FileObject * fptr)
{
	if(fptr == NULL)
		return FALSE;
	int32 r = TRUE;
	fptr->file_block->lock = 0;
	
	if(	append_buffer_was_enabled
		&& fptr->append_buffer_was_enabled 
		&& fptr->append_buffer_byte_count != 0)
	{
		if(	!_append_file_without_buffer_unsafe(fptr, 
											fptr->append_buffer_bytes, 
											fptr->append_buffer_byte_count))
			return FALSE;
		fptr->append_buffer_byte_count = 0;
	}

	if(!set_block(fptr->symbol, fptr->file_block_id, (struct RawBlock *)(fptr->file_block)))
		r = FALSE;
	free_memory(fptr->file_block);
	free_memory(fptr);
	return r;
}

/**
	@Function:		close_file
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
close_file(IN FileObject * fptr)
{
	lock();
	BOOL r = _close_file_unsafe(fptr);
	unlock();
	return r;
}

/**
	@Function:		_write_file_unsafe
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
_write_file_unsafe(	IN FileObject * fptr, 
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
			if(!set_block(fptr->symbol, block_id, (struct RawBlock *)&data_block))
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
		uint32 data_block_id = add_block(fptr->symbol, (struct RawBlock *)&data_block);
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

	return set_block(fptr->symbol, fptr->file_block_id, (struct RawBlock *)fptr->file_block);
}

/**
	@Function:		write_file
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
write_file(	IN FileObject * fptr, 
			IN uint8 * buffer, 
			IN uint32 len)
{
	lock();
	BOOL r = _write_file_unsafe(fptr, buffer, len);
	unlock();
	return r;
}

/**
	@Function:		_read_file_unsafe
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
_read_file_unsafe(	IN FileObject * fptr, 
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
		if(!get_block(	fptr->symbol, 
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
				if(!get_block(	fptr->symbol, 
								fptr->file_block->blockids[block_index], 
								(struct RawBlock *)&(fptr->read_buffer_data_block)))
					break;
				// 记录读取缓冲区信息。
				fptr->read_buffer_block_index = fptr->file_block->blockids[block_index];
				fptr->read_buffer_is_valid = TRUE;
			}
		}

		*(buffer++) = fptr->read_buffer_data_block.data[fptr->next_block_pos++];
		if(fptr->next_block_pos == DATA_BLOCK_DATA_LEN)
		{
			fptr->next_block_index++;
			fptr->next_block_pos = 0;
		}
		len--;
		real_len++;
	}
	return real_len;
}

/**
	@Function:		_read_file_without_buffer_unsafe
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
_read_file_without_buffer_unsafe(	IN FileObject * fptr, 
									OUT uint8 * buffer, 
									IN uint32 len)
{
	if((fptr->mode & FILE_MODE_READ) == 0 || len == 0)
		return 0;
	uint32 real_len = 0;
	uint32 block_index = fptr->next_block_index;
	struct DataBlock data_block;
	if(!get_block(	fptr->symbol,
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
			if(	!get_block(fptr->symbol,
				fptr->file_block->blockids[block_index],
				(struct RawBlock *)&data_block))
				break;
		}
		*(buffer++) = data_block.data[fptr->next_block_pos++];
		if(fptr->next_block_pos == DATA_BLOCK_DATA_LEN)
		{
			fptr->next_block_index++;
			fptr->next_block_pos = 0;
		}
		len--;
		real_len++;
	}
	return real_len;
}

/**
	@Function:		read_file
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
read_file(	IN FileObject * fptr, 
			OUT uint8 * buffer, 
			IN uint32 len)
{
	lock();
	uint32 r = FALSE;
	if(read_buffer_was_enabled)
		r = _read_file_unsafe(fptr, buffer, len);
	else
		r = _read_file_without_buffer_unsafe(fptr, buffer, len);
	unlock();
	return r;
}

/**
	@Function:		reset_file
	@Access:		Public
	@Description:
		重置文件的读取指针。
	@Parameters:
		fptr, FileObject *, IN
			文件指针。
	@Return:	
*/
void
reset_file(IN FileObject * fptr)
{
	fptr->next_block_index = 0;
	fptr->next_block_pos = 0;
}

/**
	@Function:		is_eof
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
is_eof(IN FileObject * fptr)
{
	uint32 next = 	fptr->next_block_index
					* DATA_BLOCK_DATA_LEN
					+ fptr->next_block_pos;
	return next == flen(fptr);
}

/**
	@Function:		_append_file_without_buffer_unsafe
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
_append_file_without_buffer_unsafe(	IN FileObject * fptr, 
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
		uint32 new_block_id = add_block(fptr->symbol, (struct RawBlock *)&data_block);
		if(new_block_id == INVALID_BLOCK_ID)
			return FALSE;	
		fptr->file_block->blockids[block_index] = new_block_id;
	}

	//获取下一个要写入的Data Block.
	if(!get_block(fptr->symbol, fptr->file_block->blockids[cblock_index], (struct RawBlock *)&data_block))
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
				if(!set_block(fptr->symbol, fptr->file_block->blockids[cblock_index - 1], (struct RawBlock *)&data_block))
					return FALSE;
			}
			else
			{
				//不存在. 新建一个Data Block并追加到File Block里.
				data_block.used = 1;
				data_block.type = BLOCK_TYPE_DATA;
				uint32 new_block_id = add_block(fptr->symbol, (struct RawBlock *)&data_block);
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
			if(!set_block(fptr->symbol, id, (struct RawBlock *)&data_block))
				return FALSE;
		}
		else
		{
			id = add_block(fptr->symbol, (struct RawBlock *)&data_block);
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

	return set_block(fptr->symbol, fptr->file_block_id, (struct RawBlock *)(fptr->file_block));
}

/**
	@Function:		_append_file_unsafe
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
_append_file_unsafe(IN FileObject * fptr, 
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
			if(!_append_file_without_buffer_unsafe(	fptr, 
												fptr->append_buffer_bytes, 
												fptr->append_buffer_byte_count))
				return FALSE;
		if(len > sizeof(fptr->append_buffer_bytes))
		{
			if(!_append_file_without_buffer_unsafe(fptr, buffer, len))
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
	@Function:		append_file
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
append_file(IN FileObject * fptr, 
			IN uint8 * buffer, 
			IN uint32 len)
{
	lock();
	BOOL r = FALSE;
	if(append_buffer_was_enabled)
		r = _append_file_unsafe(fptr, buffer, len);
	else
		r = _append_file_without_buffer_unsafe(fptr, buffer, len);
	unlock();
	return r;
}

static int8 repairing_path[MAX_PATH_BUFFER_LEN];

#define	resume_repairing_path()	\
{	\
	repairing_path[strlen(repairing_path) - strlen(dir->dirname) - 1] = '\0';\
}

/**
	@Function:		repair_files
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
repair_files(	IN int8 * symbol, 
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
		if(!get_block(symbol, id, &raw_block))
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
						if(set_block(symbol, id, &raw_block))
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
					if(!repair_files(symbol, (struct DirBlock *)&raw_block))
						r = FALSE;	//修复该子文件夹时发生错误
					break;
				default:
					r = FALSE;	//文件夹块ID列表包含一个错误的类型的块
					break;
			}
		else
			r = FALSE;	//文件夹的块列表包含一个错误的ID
	}
	resume_repairing_path();
	return r;	//返回
}

/**
	@Function:		repair_ifs1
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
repair_ifs1(IN int8 * symbol)
{
	struct DirBlock dir;
	if(	!get_block(symbol, START_BLOCK_ID, (struct RawBlock *)&dir)
		|| dir.used == 0
		|| dir.type != BLOCK_TYPE_DIR)
		return FALSE;
	strcpy_safe(repairing_path, sizeof(repairing_path), symbol);
	strcat_safe(repairing_path, sizeof(repairing_path), ":/");
	return repair_files(symbol, &dir);
}
