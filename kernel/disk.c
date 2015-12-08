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
#include "atapi.h"
#include <ilib/string.h>

static int8 disk_list[MAX_DISK_COUNT][3];
static uint64 _disk_rbytes[MAX_DISK_COUNT];
static uint64 _disk_wbytes[MAX_DISK_COUNT];
static uint32 disk_count = 0;

#define	_INIT_DISK_RWBYTES(_idx)	\
	_disk_rbytes[(_idx)] = 0;	\
	_disk_wbytes[(_idx)] = 0;

#define	_INC_DISK_RBYTES(_idx, _v)	\
	_disk_rbytes[(_idx)] += (_v);

#define	_INC_DISK_WBYTES(_idx, _v)	\
	_disk_wbytes[(_idx)] += (_v);

#define	_DISK_RBYTES(_idx)	(_disk_rbytes[(_idx)])

#define	_DISK_WBYTES(_idx)	(_disk_wbytes[(_idx)])

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
			盘符缓冲区必须大于或等于DISK_SYMBOL_BUFFER_SIZE。
	@Return:	
*/
void
get_disk_symbol(IN uint32 index, 
				OUT int8 * symbol)
{
	if(index < 0 || index >= MAX_DISK_COUNT)
		strcpy_safe(symbol, DISK_SYMBOL_BUFFER_SIZE, "");
	else
		strcpy_safe(symbol, DISK_SYMBOL_BUFFER_SIZE, disk_list[index]);
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
	else if(strcmp(symbol, "CA") == 0
			|| strcmp(symbol, "CB") == 0
			|| strcmp(symbol, "CC") == 0
			|| strcmp(symbol, "CD") == 0)
		return ATAPI_MAX_SIZE / 1024;
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
	if(symbol == NULL)
		return;
	if(strcmp(symbol, "VA") == 0 || strcmp(symbol, "VB") == 0)
	{
		init_vdisk(symbol);
		_INIT_DISK_RWBYTES(disk_count);
		strcpy_safe(disk_list[disk_count++], DISK_SYMBOL_BUFFER_SIZE, symbol);
	}
	else if(strcmp(symbol, "HD") == 0)
	{
		uint32 r = init_hdisk();
		if((r & 0x1) != 0)
		{
			_INIT_DISK_RWBYTES(disk_count);
			strcpy_safe(disk_list[disk_count++], DISK_SYMBOL_BUFFER_SIZE, "DA");
		}
		if((r & 0x2) != 0)
		{
			_INIT_DISK_RWBYTES(disk_count);
			strcpy_safe(disk_list[disk_count++], DISK_SYMBOL_BUFFER_SIZE, "DB");
		}
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
	else if(strcmp(symbol, "CA") == 0
			|| strcmp(symbol, "CB") == 0
			|| strcmp(symbol, "CC") == 0
			|| strcmp(symbol, "CD") == 0)
		return ATAPI_SECTOR512_COUNT;
	else 
		return 0;
}

/**
	@Function:		_disk_index
	@Access:		Public
	@Description:
		获取一个磁盘的索引。
	@Parameters:
		symbol, CASCTEXT, IN
			磁盘标识符。
	@Return:
		uint32
			磁盘索引。
*/
static
uint32
_disk_index(IN CASCTEXT symbol)
{
	if(symbol == NULL)
		return 0xffffffff;
	uint32 idx;
	for(idx = 0; idx < disk_count; idx++)
		if(strcmp(disk_list[idx], symbol) == 0)
			return idx;
	return 0xffffffff;
}

/**
	@Function:		disk_wbytes
	@Access:		Public
	@Description:
		获取一个磁盘的写入的字节数。
	@Parameters:
		symbol, CASCTEXT, IN
			磁盘标识符。
	@Return:
		uint64
			磁盘的写入的字节数。
*/
uint64
disk_wbytes(IN CASCTEXT symbol)
{
	if(symbol == NULL)
		return 0;
	uint32 idx = _disk_index(symbol);
	return _DISK_WBYTES(idx);
}

/**
	@Function:		disk_rbytes
	@Access:		Public
	@Description:
		获取一个磁盘的读出的字节数。
	@Parameters:
		symbol, CASCTEXT, IN
			磁盘标识符。
	@Return:
		uint64
			磁盘的读出的字节数。
*/
uint64
disk_rbytes(IN CASCTEXT symbol)
{
	if(symbol == NULL)
		return 0;
	uint32 idx = _disk_index(symbol);
	return _DISK_RBYTES(idx);
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
	BOOL r = FALSE;
	if(strcmp(symbol, "VA") == 0 || strcmp(symbol, "VB") == 0)
		r = read_sector_v(symbol, pos, buffer);
	else if(strcmp(symbol, "DA") == 0 || strcmp(symbol, "DB") == 0)
		r = read_sector_h(symbol, pos, buffer);
	else if(strcmp(symbol, "CA") == 0)
		r = atapi_read_sector512(	ATA_BUS_PRIMARY,
									ATA_DRIVE_MASTER,
									pos,
									buffer);
	else if(strcmp(symbol, "CB") == 0)
		r = atapi_read_sector512(	ATA_BUS_PRIMARY,
									ATA_DRIVE_SLAVE,
									pos,
									buffer);
	else if(strcmp(symbol, "CC") == 0)
		r = atapi_read_sector512(	ATA_BUS_SECONDARY,
									ATA_DRIVE_MASTER,
									pos,
									buffer);
	else if(strcmp(symbol, "CD") == 0)
		r = atapi_read_sector512(	ATA_BUS_SECONDARY,
									ATA_DRIVE_SLAVE,
									pos,
									buffer);
	else
		return FALSE;
	if(r)
	{
		uint32 idx = _disk_index(symbol);
		_INC_DISK_RBYTES(idx, DISK_BYTES_PER_SECTOR);
	}
	return r;
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
	BOOL r = FALSE;
	if(strcmp(symbol, "VA") == 0 || strcmp(symbol, "VB") == 0)
		r = write_sector_v(symbol, pos, buffer);
	else if(strcmp(symbol, "DA") == 0 || strcmp(symbol, "DB") == 0)
		r = write_sector_h(symbol, pos, buffer);
	else if(strcmp(symbol, "CA") == 0
			|| strcmp(symbol, "CB") == 0
			|| strcmp(symbol, "CC") == 0
			|| strcmp(symbol, "CD") == 0)
		return FALSE;
	else
		return FALSE;
	if(r)
	{
		uint32 idx = _disk_index(symbol);
		_INC_DISK_WBYTES(idx, DISK_BYTES_PER_SECTOR);
	}
	return r;
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
	BOOL r = FALSE;
	if(strcmp(symbol, "VA") == 0 || strcmp(symbol, "VB") == 0)
		r = read_sectors_v(symbol, pos, count, buffer);
	else if(strcmp(symbol, "DA") == 0 || strcmp(symbol, "DB") == 0)
		r = read_sectors_h(symbol, pos, count, buffer);
	else if(strcmp(symbol, "CA") == 0)
		r = atapi_read_sector512s(	ATA_BUS_PRIMARY,
									ATA_DRIVE_MASTER,
									pos,
									count,
									buffer);
	else if(strcmp(symbol, "CB") == 0)
		r = atapi_read_sector512s(	ATA_BUS_PRIMARY,
									ATA_DRIVE_SLAVE,
									pos,
									count,
									buffer);
	else if(strcmp(symbol, "CC") == 0)
		r = atapi_read_sector512s(	ATA_BUS_SECONDARY,
									ATA_DRIVE_MASTER,
									pos,
									count,
									buffer);
	else if(strcmp(symbol, "CD") == 0)
		r = atapi_read_sector512s(	ATA_BUS_SECONDARY,
									ATA_DRIVE_SLAVE,
									pos,
									count,
									buffer);
	else 
		return FALSE;
	if(r)
	{
		uint32 idx = _disk_index(symbol);
		_INC_DISK_RBYTES(idx, count * DISK_BYTES_PER_SECTOR);
	}
	return r;
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
	BOOL r = FALSE;
	if(strcmp(symbol, "VA") == 0 || strcmp(symbol, "VB") == 0)
		r = write_sectors_v(symbol, pos, count, buffer);
	else if(strcmp(symbol, "DA") == 0 || strcmp(symbol, "DB") == 0)
		r = write_sectors_h(symbol, pos, count, buffer);
	else if(strcmp(symbol, "CA") == 0
			|| strcmp(symbol, "CB") == 0
			|| strcmp(symbol, "CC") == 0
			|| strcmp(symbol, "CD") == 0)
		return FALSE;
	else
		return FALSE;
	if(r)
	{
		uint32 idx = _disk_index(symbol);
		_INC_DISK_WBYTES(idx, count * DISK_BYTES_PER_SECTOR);
	}
	return r;
}
