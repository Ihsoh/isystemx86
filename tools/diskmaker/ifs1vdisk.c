//Filename:		ifs1vdisk.c
//Author:		Ihsoh
//Date:			2014-6-5
//Descriptor:	IFS1 Virtual Disk

#include "ifs1vdisk.h"
#include "memory.h"
#include "types.h"
#include <string.h>

static uchar * vdiska = NULL;
static uchar * vdiskb = NULL;

void init_vdisk(char * symbol)
{
	uint ui;
	if(strcmp(symbol, "VA") == 0)
	{
		vdiska = (uchar *)alloc_memory(VDISK_BUFFER_SIZE);
		for(ui = 0; ui < VDISK_BUFFER_SIZE; ui++)
			vdiska[ui] = 0;	
	}
	else if(strcmp(symbol, "VB") == 0)
	{
		vdiskb = (uchar *)alloc_memory(VDISK_BUFFER_SIZE);
		for(ui = 0; ui < VDISK_BUFFER_SIZE; ui++)
			vdiskb[ui] = 0;	
	}
}

void destroy_vdisk(char * symbol)
{
	if(strcmp(symbol, "VA") == 0)
		free_memory(vdiska);
	else if(strcmp(symbol, "VB") == 0)
		free_memory(vdiskb);
}

uint sector_count_v(void)
{
	return VDISK_SECTORS;
}

uint get_vdisk_size(char * symbol)
{
	if(strcmp(symbol, "VA") == 0)
		return VDISK_BUFFER_SIZE / 1024;
	else if(strcmp(symbol, "VB") == 0)
		return VDISK_BUFFER_SIZE / 1024;
	else
		return 0;
}

int read_sector_v(char * symbol, uint pos, uchar * buffer)
{
	uchar * start = NULL;
	uint ui;
	if(strcmp(symbol, "VA") == 0)
		start = vdiska;
	else if(strcmp(symbol, "VB") == 0)
		start = vdiskb;
	else
		return 0;
	if(pos >= VDISK_SECTORS)
		return 0;
	for(ui = 0; ui < VDISK_SECTOR_SIZE; ui++)
		*(buffer + ui) = *(start + pos * VDISK_SECTOR_SIZE + ui);
	return 1;
}

int write_sector_v(char * symbol, uint pos, uchar * buffer)
{
	uchar * start = NULL;
	uint ui;
	if(strcmp(symbol, "VA") == 0)
		start = vdiska;
	else if(strcmp(symbol, "VB") == 0)
		start = vdiskb;
	else
		return 0;
	if(pos >= VDISK_SECTORS)
		return 0;
	for(ui = 0; ui < VDISK_SECTOR_SIZE; ui++)
		*(start + pos * VDISK_SECTOR_SIZE + ui) = *(buffer + ui);
	return 1;
}

int read_sectors_v(char * symbol, uint pos, uchar count, uchar * buffer)
{
	int r = 1;
	int c;
	if(count == 0)
		c = 256;
	else
		c = count;
	while(c > 0)
	{
		if(!read_sector_v(symbol, pos, buffer))
		{
			r = 0;
			break;		
		}		
		pos++;
		buffer += 512;
		c--;
	}
	return r;
}

int write_sectors_v(char * symbol, uint pos, uchar count, uchar * buffer)
{
	int r = 1;
	int c;
	if(count == 0)
		c = 256;
	else
		c = count;
	while(c > 0)
	{
		if(!write_sector_v(symbol, pos, buffer))
		{
			r = 0;
			break;
		}
		pos++;
		buffer += 512;
		c--;
	}
	return r;
}

uchar * get_vdiska(void)
{
	return vdiska;
}
