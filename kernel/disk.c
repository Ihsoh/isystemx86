/**
	@File:			disk.c
	@Author:		Ihsoh
	@Date:			2014-6-5
	@Description:
		提供磁盘操作功能。
*/

#include "disk.h"
#include "types.h"
#include "vdisk.h"
#include "hdisk.h"
#include <ilib/string.h>

static int8 disk_list[MAX_DISK_COUNT][3] = {{0, 0, 0},
											{0, 0, 0},
											{0, 0, 0},
											{0, 0, 0},
											{0, 0, 0},
											{0, 0, 0},
											{0, 0, 0},
											{0, 0, 0}};
static uint32 disk_count = 0;

/**
	@Function:		get_disk_symbol
	@Access:		Public
	@Description:
		获取盘符。
	@Parameters:
		index, uint32, IN
			索引。
		symbol, int8 *, OUT
			盘符。
	@Return:	
*/
void
get_disk_symbol(IN uint32 index, 
				OUT int8 * symbol)
{
	if(index < 0 || index >= MAX_DISK_COUNT)
		strcpy(symbol, "");
	else
		strcpy(symbol, disk_list[index]);
}

/**
	@Function:		get_disk_count
	@Access:		Public
	@Description:
		获取磁盘数量。
	@Parameters:
	@Return:
		uint32
			磁盘数量。		
*/
uint32
get_disk_count(void)
{
	return disk_count;
}

/**
	@Function:		get_disk_size
	@Access:		Public
	@Description:
		获取指定磁盘容量，单位为KB。
	@Parameters:
		symbol, int8 *, IN
			盘符。
	@Return:
		uint32
			指定磁盘容量。		
*/
uint32
get_disk_size(IN int8 * symbol)
{
	if(strcmp(symbol, "VA") == 0)
		return get_vdisk_size("VA");
	else if(strcmp(symbol, "VB") == 0)
		return get_vdisk_size("VB");
	else if(strcmp(symbol, "DA") == 0)
		return get_hdisk_size("DA");
	else if(strcmp(symbol, "DB") == 0)
		return get_hdisk_size("DB");
	else
		return 0;
}

/**
	@Function:		is_system_disk
	@Access:		Public
	@Description:
		检测指定磁盘是否为系统盘。
	@Parameters:
		symbol, int8 *, IN
			盘符。
	@Return:
		BOOL
			返回TRUE则为系统盘，否则不是。	
*/
BOOL
is_system_disk(IN int8 * symbol)
{
	uint32 disk_size = get_disk_size(symbol);
	if(disk_size != 0)
	{
		uchar buffer[512];
		if(!read_sector(symbol, 0, buffer))
			return FALSE;
		if(buffer[510] == 0x55 && buffer[511] == 0xaa)
			return TRUE;
		else
			return FALSE;
	}
	else
		return FALSE;
}

/**
	@Function:		init_disk
	@Access:		Public
	@Description:
		初始化磁盘。
	@Parameters:
		symbol, int8 *, IN
			盘符。
	@Return:	
*/
void
init_disk(IN int8 * symbol)
{
	if(strcmp(symbol, "VA") == 0 || strcmp(symbol, "VB") == 0)
	{
		init_vdisk(symbol);
		strcpy(disk_list[disk_count++], symbol);
	}
	else if(strcmp(symbol, "HD") == 0)
	{
		uint32 r = init_hdisk();
		if((r & 0x1) != 0)
			strcpy(disk_list[disk_count++], "DA");
		if((r & 0x2) != 0)
			strcpy(disk_list[disk_count++], "DB");
	}
}

/**
	@Function:		destroy_disk
	@Access:		Public
	@Description:
		销毁指定磁盘资源。
	@Parameters:
		symbol, int8 *, IN
			盘符。
	@Return:	
*/
void
destroy_disk(IN int8 * symbol)
{
	if(strcmp(symbol, "VA") == 0 || strcmp(symbol, "VB") == 0)
		destroy_vdisk(symbol);
}

/**
	@Function:		sector_count
	@Access:		Public
	@Description:
		获取指定磁盘的扇区数量。
	@Parameters:
		symbol, int8 *, IN
			盘符。
	@Return:
		uint32
			扇区数量。		
*/
uint32
sector_count(IN int8 * symbol)
{
	if(strcmp(symbol, "VA") == 0 || strcmp(symbol, "VB") == 0)
		return sector_count_v();
	else if(strcmp(symbol, "DA") == 0 || strcmp(symbol, "DB") == 0)
		return sector_count_h(symbol);
	else 
		return 0;
}

/**
	@Function:		read_sector
	@Access:		Public
	@Description:
		读取一个扇区的数据。
	@Parameters:
		symbol, int8 *, IN
			磁盘标识符。
		pos, uint32, IN
			LBA表示法的逻辑地址。
		buffer, uint8 *, OUT
			指向用于储存扇区数据的缓冲区。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
read_sector(IN int8 * symbol, 
			IN uint32 pos, 
			OUT uint8 * buffer)
{
	if(strcmp(symbol, "VA") == 0 || strcmp(symbol, "VB") == 0)
		return read_sector_v(symbol, pos, buffer);
	else if(strcmp(symbol, "DA") == 0 || strcmp(symbol, "DB") == 0)
		return read_sector_h(symbol, pos, buffer);
	else
		return FALSE;
}

/**
	@Function:		write_sector
	@Access:		Public
	@Description:
		写入数据到一个扇区。
	@Parameters:
		symbol, int8 *, IN
			磁盘标识符。
		pos, uint32, IN
			LBA表示法的逻辑地址。
		buffer, uint8 *, IN
			指向包含将要写入扇区的数据的缓冲区。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。	
*/
BOOL
write_sector(	IN int8 * symbol, 
				IN uint32 pos, 
				IN uint8 * buffer)
{
	if(strcmp(symbol, "VA") == 0 || strcmp(symbol, "VB") == 0)
		return write_sector_v(symbol, pos, buffer);
	else if(strcmp(symbol, "DA") == 0 || strcmp(symbol, "DB") == 0)
		return write_sector_h(symbol, pos, buffer);
	else
		return FALSE;
}

/**
	@Function:		read_sectors
	@Access:		Public
	@Description:
		读取一个或多个扇区的数据。最多一次可以读取256个扇区的数据。
	@Parameters:
		symbol, int8 *, IN
			磁盘标识符。
		pos, uint32, IN
			LBA表示法的逻辑地址。
		count, uint8, IN
			要读取的扇区的数量。当该参数为0时代表要读取256个扇区。
		buffer, uint8 *, OUT
			指向用于储存扇区数据的缓冲区。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。		
*/
BOOL
read_sectors(	IN int8 * symbol, 
				IN uint32 pos, 
				IN uint8 count, 
				OUT uint8 * buffer)
{
	if(strcmp(symbol, "VA") == 0 || strcmp(symbol, "VB") == 0)
		return read_sectors_v(symbol, pos, count, buffer);
	else if(strcmp(symbol, "DA") == 0 || strcmp(symbol, "DB") == 0)
		return read_sectors_h(symbol, pos, count, buffer);
	else 
		return FALSE;
}

/**
	@Function:		write_sectors
	@Access:		Public
	@Description:
		写入数据到一个或多个扇区。最多一次可以写入256个扇区的数据。
	@Parameters:
		symbol, int8 *, IN
			磁盘标识符。
		pos, uint32, IN
			LBA表示法的逻辑地址。
		count, uint8, IN
			要写入数据的扇区的数量。当该参数为0时代表要读取256个扇区。
		buffer, uint8 *, IN
			指向包含将要写入扇区的数据的缓冲区。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
write_sectors(	IN int8 * symbol, 
				IN uint32 pos, 
				IN uint8 count, 
				IN uint8 * buffer)
{
	if(strcmp(symbol, "VA") == 0 || strcmp(symbol, "VB") == 0)
		return write_sectors_v(symbol, pos, count, buffer);
	else if(strcmp(symbol, "DA") == 0 || strcmp(symbol, "DB") == 0)
		return write_sectors_h(symbol, pos, count, buffer);
	else
		return FALSE;
}
