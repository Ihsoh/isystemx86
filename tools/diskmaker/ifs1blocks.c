//Filename:		ifs1blocks.c
//Author:		Ihsoh
//Date:			2014-6-5
//Descriptor:	IFS1 Blocks

#include "ifs1blocks.h"
#include "types.h"
#include "memory.h"
#include "ifs1disk.h"
#include "cmos.h"
#include <string.h>

static uint max_block_count(char * symbol)
{
	//return (get_disk_size(symbol) * 1024) / (BLOCK_SECTORS * 512);
	return (get_disk_size(symbol) * 2) / (BLOCK_SECTORS);
}

int get_block(char * symbol, uint id, struct RawBlock * block)
{
	if(id < START_BLOCK_ID || id >= max_block_count(symbol))
		return 0;
	return read_sectors(symbol, id * BLOCK_SECTORS, BLOCK_SECTORS, (uchar *)block);
}

int set_block(char * symbol, uint id, struct RawBlock * block)
{
	if(id < START_BLOCK_ID || id >= max_block_count(symbol))
		return 0;
	return write_sectors(symbol, id * BLOCK_SECTORS, BLOCK_SECTORS, (uchar *)block);
}

int del_block(char * symbol, uint id)
{
	if(id < START_BLOCK_ID || id >= max_block_count(symbol))
		return 0;
	struct RawBlock temp;
	temp.used = 0;
	return set_block(symbol, id, &temp);
}

int del_all_blocks(char * symbol)
{
	uint block_count = max_block_count(symbol);
	if(block_count == 0)
		return 0;
	uint ui;
	for(ui = START_BLOCK_ID; ui < block_count; ui++)
		if(!del_block(symbol, ui))
			return 0;
	return 1;
}

uint add_block(char * symbol, struct RawBlock * block)
{
	uint block_count = max_block_count(symbol);
	uint ui;
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

int fill_dir_block(char * name, struct DirBlock * dir)
{
	if(strlen(name) > sizeof(dir->dirname))
		return 0;
	uint ui;
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
	return 1;
}

int fill_file_block(char * name, struct FileBlock * file)
{
	if(strlen(name) > sizeof(file->filename))
		return 0;
	uint ui;
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
	for(ui = 0; ui < sizeof(file->blockids) / sizeof(uint); ui++)
		file->blockids[ui] = 0xFFFFFFFF;
	return 1;
}
