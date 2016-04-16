/**
	@File:			blocks.c
	@Author:		Ihsoh
	@Date:			2014-6-5
	@Description:
		提供IFS1文件系统的块操作功能。
*/

#include "blocks.h"
#include "../../types.h"
#include "../../memory.h"
#include "../../disk.h"
#include "../../cmos.h"
#include "../../kmpool.h"

#include <ilib/string.h>
#include <dslib/linked_list.h>
#include <dslib/value.h>

#define	DEFAULT_INIT_BAT_COUNT	256

static DSLLinkedListPtr bat_va = NULL;	// Block Allocation Table - Virtual disk A
static DSLLinkedListPtr bat_vb = NULL;	// Block Allocation Table - Virtual disk B
static DSLLinkedListPtr bat_da = NULL;	// Block Allocation Table - Hard disk A
static DSLLinkedListPtr bat_db = NULL;	// Block Allocation Table - Hard disk B

/**
	@Function:		_Ifs1GetBAT
	@Access:		Private
	@Description:
		获取指定磁盘的块分配表。
	@Parameters:
		symbol, const int8 *, IN
			盘符。
	@Return:
		DSLLinkedListPtr
			块分配表的指针。		
*/
static
DSLLinkedListPtr
_Ifs1GetBAT(IN const int8 * symbol)
{
	if(symbol == NULL)
		return NULL;
	if(strcmp(symbol, "VA") == 0)
		return bat_va;
	else if(strcmp(symbol, "VB") == 0)
		return bat_vb;
	else if(strcmp(symbol, "DA") == 0)
		return bat_da;
	else if(strcmp(symbol, "DB") == 0)
		return bat_db;
	else
		return NULL;
}

/**
	@Function:		_Ifs1GetMaxBlockCount
	@Access:		Private
	@Description:
		获取指定磁盘的最大块数。
	@Parameters:
		symbol, const int8 *, IN
			盘符。
	@Return:
		uint32
			指定的磁盘的最大块数。		
*/
static
uint32
_Ifs1GetMaxBlockCount(IN const int8 * symbol)
{
	if(symbol == NULL)
		return 0;
	return (get_disk_size(symbol) * 2) / (BLOCK_SECTORS);
}

/**
	@Function:		_Ifs1FillBAT
	@Access:		Private
	@Description:
		扫描磁盘，填充块分配表。
	@Parameters:
		bat, DSLLinkedListPtr, IN OUT
			块分配表。
		symbol, const int8 *, IN
			盘符。
		max, uint32, IN
			获取到该数量的块后停止扫描和填充。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。		
*/
static
BOOL
_Ifs1FillBAT(	IN OUT DSLLinkedListPtr bat,
			IN const int8 * symbol,
			IN uint32 max)
{
	if(bat == NULL || symbol == NULL)
		return FALSE;
	uint32 n = 0;
	uint32 ui;
	uint32 count = _Ifs1GetMaxBlockCount(symbol);
	for(ui = START_BLOCK_ID; ui < count && max > 0; ui++)
	{
		struct RawBlock block;
		Ifs1GetBlock(symbol, ui, &block);
		if(!block.used)
		{
			DSLLinkedListNodePtr node;
			node = (DSLLinkedListNodePtr)kmpool_malloc(sizeof(DSLLinkedListNode));
			if(node == NULL)
				return FALSE;
			node->value.value.uint32_value = ui;
			node->value.type = DSLVALUE_UINT32;
			if(!dsl_lnklst_add_node(bat, node))
				return FALSE;
			max--;
		}
	}
	return max == 0;
}

/**
	@Function:		_Ifs1NewBlockFromBAT
	@Access:		Private
	@Description:
		从块分配表获取一个可用块。
	@Parameters:
		symbol, const int8 *, IN
			盘符。
		block_id, uint32 *, OUT
			块ID。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
static
BOOL
_Ifs1NewBlockFromBAT(	IN const int8 * symbol,
						OUT uint32 * block_id)
{
	if(symbol == NULL || block_id == NULL)
		return FALSE;
	static DSLLinkedListPtr bat = NULL;
	if(strcmp(symbol, "VA") == 0)
	{
		if(bat_va == NULL)
			return FALSE;
		bat = bat_va;
	}
	else if(strcmp(symbol, "VB") == 0)
	{
		if(bat_vb == NULL)
			return FALSE;
		bat = bat_vb;
	}
	else if(strcmp(symbol, "DA") == 0)
	{
		if(bat_da == NULL)
			return FALSE;
		bat = bat_da;
	}
	else if(strcmp(symbol, "DB") == 0)
	{
		if(bat_db == NULL)
			return FALSE;
		bat = bat_db;
	}
	else
		return FALSE;
	if(dsl_lnklst_count(bat) == 0)
		return FALSE;
	DSLLinkedListNodePtr node = dsl_lnklst_shift_node(bat);
	if(node == NULL)
		return FALSE;
	*block_id = node->value.value.uint32_value;
	kmpool_free(node);
	return TRUE;
}

/**
	@Function:		Ifs1InitBlocks
	@Access:		Public
	@Description:
		初始化硬盘块处理模块。
	@Parameters:
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
Ifs1InitBlocks(void)
{
	bat_va = dsl_lnklst_new();
	_Ifs1FillBAT(bat_va, "VA", DEFAULT_INIT_BAT_COUNT);
	bat_vb = dsl_lnklst_new();
	_Ifs1FillBAT(bat_vb, "VB", DEFAULT_INIT_BAT_COUNT);
	bat_da = dsl_lnklst_new();
	_Ifs1FillBAT(bat_da, "DA", DEFAULT_INIT_BAT_COUNT);
	bat_db = dsl_lnklst_new();
	_Ifs1FillBAT(bat_db, "DB", DEFAULT_INIT_BAT_COUNT);
	return 	bat_va != NULL
			&& bat_vb != NULL
			&& bat_da != NULL
			&& bat_db != NULL;
}

/**
	@Function:		Ifs1GetBlock
	@Access:		Public
	@Description:
		获取指定的磁盘的指定块。
	@Parameters:
		symbol, const int8 *, IN
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
Ifs1GetBlock(	IN const int8 * symbol, 
				IN uint32 id, 
				OUT struct RawBlock * block)
{
	if(	symbol == NULL
		|| block == NULL
		|| id < START_BLOCK_ID 
		|| id >= _Ifs1GetMaxBlockCount(symbol))
		return FALSE;
	return read_sectors(symbol, id * BLOCK_SECTORS, BLOCK_SECTORS, (uint8 *)block);
}

/**
	@Function:		Ifs1SetBlock
	@Access:		Public
	@Description:
		设置指定的磁盘的指定块。
	@Parameters:
		symbol, const int8 *, IN
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
Ifs1SetBlock(	IN const int8 * symbol, 
				IN uint32 id, 
				IN struct RawBlock * block)
{
	if(	symbol == NULL
		|| block == NULL
		|| id < START_BLOCK_ID 
		|| id >= _Ifs1GetMaxBlockCount(symbol))
		return FALSE;
	return write_sectors(symbol, id * BLOCK_SECTORS, BLOCK_SECTORS, (uint8 *)block);
}

/**
	@Function:		Ifs1DeleteBlock
	@Access:		Public
	@Description:
		删除指定的磁盘的指定块。
	@Parameters:
		symbol, const int8 *, IN
			盘符。
		id, uint32, IN
			块ID。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。	
*/
BOOL
Ifs1DeleteBlock(IN const int8 * symbol, 
				IN uint32 id)
{
	if(	symbol == NULL
		|| id < START_BLOCK_ID 
		|| id >= _Ifs1GetMaxBlockCount(symbol))
		return FALSE;
	struct RawBlock temp;
	temp.used = 0;
	BOOL r = Ifs1SetBlock(symbol, id, &temp);
	if(r)
	{
		// 如果块被删除，则把这个块添加到块分配表中。
		DSLLinkedListPtr bat = _Ifs1GetBAT(symbol);
		if(bat != NULL)
		{
			DSLLinkedListNodePtr node;
			node = (DSLLinkedListNodePtr)kmpool_malloc(sizeof(DSLLinkedListNode));
			if(node != NULL)
			{
				node->value.value.uint32_value = id;
				node->value.type = DSLVALUE_UINT32;
				dsl_lnklst_add_node(bat, node);
			}
		}
	}
	return r;
}

/**
	@Function:		Ifs1DeleteAllBlocks
	@Access:		Public
	@Description:
		删除指定的磁盘的所有块。
	@Parameters:
		symbol, const int8 *, IN
			盘符。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。	
*/
BOOL
Ifs1DeleteAllBlocks(IN const int8 * symbol)
{
	if(symbol == NULL)
		return FALSE;
	uint32 block_count = _Ifs1GetMaxBlockCount(symbol);
	if(block_count == 0)
		return FALSE;
	uint32 ui;
	for(ui = START_BLOCK_ID; ui < block_count; ui++)
		if(!Ifs1DeleteBlock(symbol, ui))
			return FALSE;
	return TRUE;
}

/**
	@Function:		Ifs1AddBlock
	@Access:		Public
	@Description:
		向指定的磁盘添加块。
	@Parameters:
		symbol, const int8 *, IN
			盘符。
		block, struct RawBlock *, IN
			指向包含块的数据的缓冲区的指针。
	@Return:
		BOOL
			返回INVALID_BLOCK_ID则失败，否则成功。		
*/
uint32
Ifs1AddBlock(	IN const int8 * symbol, 
				IN struct RawBlock * block)
{
	if(symbol == NULL || block == NULL)
		return INVALID_BLOCK_ID;

	// 先确认块分配表是否有可用的块。
	// 如果有可用的块则直接返回块ID。
	uint32 bat_block_id;
	if(_Ifs1NewBlockFromBAT(symbol, &bat_block_id))
	{
		struct RawBlock temp;
		if(!Ifs1GetBlock(symbol, bat_block_id, &temp) || temp.used)
			return INVALID_BLOCK_ID;
		if(!Ifs1SetBlock(symbol, bat_block_id, (struct RawBlock *)block))
			return INVALID_BLOCK_ID;
		return bat_block_id;
	}

	uint32 block_count = _Ifs1GetMaxBlockCount(symbol);
	uint32 ui;
	for(ui = START_BLOCK_ID; ui < block_count; ui++)
	{
		struct RawBlock temp;
		if(!Ifs1GetBlock(symbol, ui, &temp))
			return INVALID_BLOCK_ID;
		if(!temp.used)
		{
			if(!Ifs1SetBlock(symbol, ui, (struct RawBlock *)block))
				return INVALID_BLOCK_ID;
			return ui;
		}
	}
	return INVALID_BLOCK_ID;
}

/**
	@Function:		Ifs1FillDirBlock
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
Ifs1FillDirBlock(	IN int8 * name, 
					IN struct DirBlock * dir)
{
	if(name == NULL || dir == NULL)
		return FALSE;
	if(strlen(name) >= sizeof(dir->dirname))
		return FALSE;
	uint32 ui;
	struct CMOSDateTime dt;
	get_cmos_date_time(&dt);
	dir->used = 1;
	dir->type = BLOCK_TYPE_DIR;
	UtlCopyString(dir->dirname, sizeof(dir->dirname), name);
	memcpy(&(dir->create), &dt, sizeof(struct CMOSDateTime));
	memcpy(&(dir->change), &dt, sizeof(struct CMOSDateTime));
	for(ui = 0; ui < sizeof(dir->reserve); ui++)
		dir->reserve[ui] = 0;
	for(ui = 0; ui < sizeof(dir->blockids) / sizeof(uint); ui++)
		dir->blockids[ui] = INVALID_BLOCK_ID;
	return TRUE;
}

/**
	@Function:		Ifs1FillFileBlock
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
Ifs1FillFileBlock(	IN int8 * name, 
					OUT struct FileBlock * file)
{
	if(name == NULL || file == NULL)
		return FALSE;
	if(strlen(name) >= sizeof(file->filename))
		return FALSE;
	uint32 ui;
	struct CMOSDateTime dt;
	get_cmos_date_time(&dt);
	file->used = 1;
	file->type = BLOCK_TYPE_FILE;
	file->length = 0;
	UtlCopyString(file->filename, sizeof(file->filename), name);
	memcpy(&(file->create), &dt, sizeof(struct CMOSDateTime));
	memcpy(&(file->change), &dt, sizeof(struct CMOSDateTime));
	file->lock = 0;
	for(ui = 0; ui < sizeof(file->reserve); ui++)
		file->reserve[ui] = 0;
	for(ui = 0; ui < sizeof(file->blockids) / sizeof(uint32); ui++)
		file->blockids[ui] = INVALID_BLOCK_ID;
	return TRUE;
}

/**
	@Function:		Ifs1FillSLinkBlock
	@Access:		Public
	@Description:
		填充软链接块结构体。
	@Parameters:
		name, int8 *, IN
			软链接名称。
		link, int8 *, IN
			目标路径。
		slink, struct SLinkBlock *, OUT
			指向软链接块结构体的指针。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。		
*/
BOOL
Ifs1FillSLinkBlock(	IN int8 * name,
					IN int8 * link,
					OUT struct SLinkBlock * slink)
{
	if(name == NULL || link == NULL || slink == NULL)
		return FALSE;
	if(	strlen(name) >= sizeof(slink->filename) 
		|| strlen(link) >= sizeof(slink->link))
		return FALSE;
	slink->used = 1;
	slink->type = BLOCK_TYPE_SLINK;
	UtlCopyString(slink->filename, sizeof(slink->filename), name);
	uint32 ui;
	for(ui = 0; ui < sizeof(slink->reserve); ui++)
		slink->reserve[ui] = 0;
	UtlCopyString(slink->link, sizeof(slink->filename), link);
	return TRUE;
}
