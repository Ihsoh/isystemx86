/**
	@File:			ifs1blocks.c
	@Author:		Ihsoh
	@Date:			2014-6-5
	@Description:
		提供IFS1文件系统的块操作功能。
*/

#include "ifs1blocks.h"
#include "types.h"
#include "memory.h"
#include "ifs1disk.h"
#include "screen.h"
#include "cmos.h"

/**
	@Function:		max_block_count
	@Access:		Private
	@Description:
		获取指定磁盘的最大块数。
	@Parameters:
		symbol, int8 *, IN
			盘符。
	@Return:
		uint32
			指定的磁盘的最大块数。		
*/
static
uint32
max_block_count(IN int8 * symbol)
{
	return (get_disk_size(symbol) * 2) / (BLOCK_SECTORS);
}

/**
	@Function:		get_block
	@Access:		Public
	@Description:
		获取指定的磁盘的指定块。
	@Parameters:
		symbol, int8 *, IN
			盘符。
		id, uint32, IN
			块ID。
		block, struct RawBlock *, OUT
			指向用于储存块的数据的缓冲区的指针。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。	
*/
BOOL
get_block(	IN int8 * symbol, 
			IN uint32 id, 
			OUT struct RawBlock * block)
{
	if(id < START_BLOCK_ID || id >= max_block_count(symbol))
		return FALSE;
	return read_sectors(symbol, id * BLOCK_SECTORS, BLOCK_SECTORS, (uint8 *)block);
}

/**
	@Function:		set_block
	@Access:		Public
	@Description:
		设置指定的磁盘的指定块。
	@Parameters:
		symbol, int8 *, IN
			盘符。
		id, uint32, IN
			块ID。
		block, struct RawBlock *, IN
			指向包含块的数据的缓冲区的指针。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。	
*/
BOOL
set_block(	IN int8 * symbol, 
			IN uint32 id, 
			IN struct RawBlock * block)
{
	if(id < START_BLOCK_ID || id >= max_block_count(symbol))
		return FALSE;
	return write_sectors(symbol, id * BLOCK_SECTORS, BLOCK_SECTORS, (uint8 *)block);
}

/**
	@Function:		del_block
	@Access:		Public
	@Description:
		删除指定的磁盘的指定块。
	@Parameters:
		symbol, int8 *, IN
			盘符。
		id, uint32, IN
			块ID。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。	
*/
BOOL
del_block(	IN int8 * symbol, 
			IN uint32 id)
{
	if(id < START_BLOCK_ID || id >= max_block_count(symbol))
		return FALSE;
	struct RawBlock temp;
	temp.used = 0;
	return set_block(symbol, id, &temp);
}

/**
	@Function:		del_all_blocks
	@Access:		Public
	@Description:
		删除指定的磁盘的所有块。
	@Parameters:
		symbol, int8 *, IN
			盘符。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。	
*/
BOOL
del_all_blocks(IN int8 * symbol)
{
	uint32 block_count = max_block_count(symbol);
	if(block_count == 0)
		return FALSE;
	uint32 ui;
	for(ui = START_BLOCK_ID; ui < block_count; ui++)
		if(!del_block(symbol, ui))
			return FALSE;
	return TRUE;
}

/**
	@Function:		add_block
	@Access:		Public
	@Description:
		向指定的磁盘添加块。
	@Parameters:
		symbol, int8 *, IN
			盘符。
		block, struct RawBlock *, IN
			指向包含块的数据的缓冲区的指针。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。		
*/
uint32
add_block(	IN int8 * symbol, 
			IN struct RawBlock * block)
{
	uint32 block_count = max_block_count(symbol);
	uint32 ui;
	for(ui  = START_BLOCK_ID; ui < block_count; ui++)
	{
		struct RawBlock temp;
		if(!get_block(symbol, ui, &temp))
			return 0xFFFFFFFF;
		if(!temp.used)
		{
			if(!set_block(symbol, ui, (struct RawBlock *)block))
				return 0xFFFFFFFF;
			return ui;
		}
	}
	return 0xFFFFFFFF;
}

/**
	@Function:		fill_dir_block
	@Access:		Public
	@Description:
		填充目录块结构体。
	@Parameters:
		name, int8 *, IN
			目录名称。
		dir, struct DirBlock *, OUT
			指向目录块结构体的指针。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。		
*/
BOOL
fill_dir_block(	IN int8 * name, 
				IN struct DirBlock * dir)
{
	if(strlen(name) >= sizeof(dir->dirname))
		return FALSE;
	uint32 ui;
	struct CMOSDateTime dt;
	get_cmos_date_time(&dt);
	dir->used = 1;
	dir->type = BLOCK_TYPE_DIR;
	strcpy(dir->dirname, name);
	memcpy(&(dir->create), &dt, sizeof(struct CMOSDateTime));
	memcpy(&(dir->change), &dt, sizeof(struct CMOSDateTime));
	for(ui = 0; ui < sizeof(dir->reserve); ui++)
		dir->reserve[ui] = 0;
	for(ui = 0; ui < sizeof(dir->blockids) / sizeof(uint); ui++)
		dir->blockids[ui] = 0xFFFFFFFF;
	return TRUE;
}

/**
	@Function:		fill_file_block
	@Access:		Public
	@Description:
		填充文件块结构体。
	@Parameters:
		name, int8 *, IN
			文件名称。
		dir, struct FileBlock *, OUT
			指向文件块结构体的指针。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。		
*/
BOOL
fill_file_block(IN int8 * name, 
				OUT struct FileBlock * file)
{
	if(strlen(name) > sizeof(file->filename))
		return FALSE;
	uint32 ui;
	struct CMOSDateTime dt;
	get_cmos_date_time(&dt);
	file->used = 1;
	file->type = BLOCK_TYPE_FILE;
	file->length = 0;
	strcpy(file->filename, name);
	memcpy(&(file->create), &dt, sizeof(struct CMOSDateTime));
	memcpy(&(file->change), &dt, sizeof(struct CMOSDateTime));
	file->lock = 0;
	for(ui = 0; ui < sizeof(file->reserve); ui++)
		file->reserve[ui] = 0;
	for(ui = 0; ui < sizeof(file->blockids) / sizeof(uint32); ui++)
		file->blockids[ui] = 0xFFFFFFFF;
	return TRUE;
}
