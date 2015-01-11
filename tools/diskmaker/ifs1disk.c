//Filename:		ifs1disk.c
//Author:		Ihsoh
//Date:			2014-6-5
//Descriptor:	Emulate IFS1 Disk

#include "ifs1disk.h"
#include "types.h"
#include "ifs1vdisk.h"
#include <string.h>

static char disk_list[256][3];
static uint disk_count = 0;

void get_disk_symbol(uint index, char * symbol)
{
	strcpy(symbol, disk_list[index]);
}

uint get_disk_count(void)
{
	return disk_count;
}

uint get_disk_size(char * symbol)
{
	if(strcmp(symbol, "VA") == 0)
		return get_vdisk_size("VA");
	else if(strcmp(symbol, "VB") == 0)
		return get_vdisk_size("VB");
	else
		return 0;
}

int is_system_disk(char * symbol)
{
	ulong disk_size = get_disk_size(symbol);
	if(disk_size != 0)
	{
		uchar buffer[512];
		read_sector(symbol, 0, buffer);
		return buffer[510] == 0x55 && buffer[511] == 0xaa;
	}
	else
		return 0;
}

void init_disk(char * symbol)
{
	if(strcmp(symbol, "VA") == 0 || strcmp(symbol, "VB") == 0)
	{
		init_vdisk(symbol);
		strcpy(disk_list[disk_count++], symbol);
	}
}

void destroy_disk(char * symbol)
{
	if(strcmp(symbol, "VA") == 0 || strcmp(symbol, "VB") == 0)
		destroy_vdisk(symbol);
}

uint sector_count(char * symbol)
{
	if(strcmp(symbol, "VA") == 0 || strcmp(symbol, "VB") == 0)
		return sector_count_v();
	else 
		return 0;
}

int read_sector(char * symbol, uint pos, uchar * buffer)
{
	if(strcmp(symbol, "VA") == 0 || strcmp(symbol, "VB") == 0)
		return read_sector_v(symbol, pos, buffer);
	else
		return 0;
}

int write_sector(char * symbol, uint pos, uchar * buffer)
{
	if(strcmp(symbol, "VA") == 0 || strcmp(symbol, "VB") == 0)
		return write_sector_v(symbol, pos, buffer);
	else
		return 0;
}

int read_sectors(char * symbol, uint pos, uchar count, uchar * buffer)
{
	if(strcmp(symbol, "VA") == 0 || strcmp(symbol, "VB") == 0)
		return read_sectors_v(symbol, pos, count, buffer);
	else 
		return 0;
}

int write_sectors(char * symbol, uint pos, uchar count, uchar * buffer)
{
	if(strcmp(symbol, "VA") == 0 || strcmp(symbol, "VB") == 0)
		return write_sectors_v(symbol, pos, count, buffer);
	else
		return 0;
}

