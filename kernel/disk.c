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
#include "atapi.h"
#include "ahci.h"
#include "ata.h"
#include "system.h"
#include <ilib/string.h>

static int8 disk_list[MAX_DISK_COUNT][3];
static uint64 _disk_rbytes[MAX_DISK_COUNT];
static uint64 _disk_wbytes[MAX_DISK_COUNT];
static uint32 disk_count = 0;

uint32 disk_hard_disk_type = DISK_TYPE_ATA;

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
	@Function:		_is_ahcisym
	@Access:		Private
	@Description:
		检测盘符是否为AHCI符号。
	@Parameters:
		symbol, CASCTEXT, IN
			盘符。
			盘符缓冲区必须大于或等于DISK_SYMBOL_BUFFER_SIZE。
	@Return:
		BOOL
			返回TRUE则代表为AHCI符号。
*/
static
BOOL
_is_ahcisym(IN CASCTEXT symbol)
{
	return	symbol[0] == 'S'
			&& (symbol[1] >= 'A' && symbol[1] <= 'Z')
			&& symbol[2] == '\0';
}

/**
	@Function:		_ahcisym2portno
	@Access:		Private
	@Description:
		AHCI设备符号转换为端口号。
	@Parameters:
		symbol, CASCTEXT, IN
			AHCI设备符号（SA、SB、SC、...、SZ）。
			盘符缓冲区必须大于或等于DISK_SYMBOL_BUFFER_SIZE。
	@Return:
		uint32
			如果AHCI符号不合法则返回0xffffffff，否则：
			SA => 0
			SB => 1
			SC => 2
			...
			SZ => 25
*/
static
uint32
_ahcisym2portno(IN CASCTEXT symbol)
{
	if(_is_ahcisym(symbol))
		return symbol[1] - 'A';
	else
		return 0xffffffff;
}

/**
	@Function:		_is_atasym
	@Access:		Private
	@Description:
		检测盘符是否为ATA符号。
	@Parameters:
		symbol, CASCTEXT, IN
			盘符。
			盘符缓冲区必须大于或等于DISK_SYMBOL_BUFFER_SIZE。
	@Return:
		BOOL
			返回TRUE则代表为ATA符号。
*/
static
BOOL
_is_atasym(IN CASCTEXT symbol)
{
	return 	strcmp(symbol, "DA") == 0
			|| strcmp(symbol, "DB") == 0
			|| strcmp(symbol, "DC") == 0
			|| strcmp(symbol, "DD") == 0;
}

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
	if(symbol == NULL)
		return;
	if(index < 0 || index >= MAX_DISK_COUNT)
		UtlCopyString(symbol, DISK_SYMBOL_BUFFER_SIZE, "");
	else
		UtlCopyString(symbol, DISK_SYMBOL_BUFFER_SIZE, disk_list[index]);
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
	if(symbol == NULL)
		return 0;
	if(strcmp(symbol, "VA") == 0)
		return get_vdisk_size("VA");
	else if(strcmp(symbol, "VB") == 0)
		return get_vdisk_size("VB");
	else if(strcmp(symbol, "VS") == 0)
		return get_disk_size(EXPLICIT_SYSTEM_DISK);
	else if(_is_atasym(symbol))
		return AtaGetSectorCount(symbol);
	else if(strcmp(symbol, "CA") == 0
			|| strcmp(symbol, "CB") == 0
			|| strcmp(symbol, "CC") == 0
			|| strcmp(symbol, "CD") == 0)
		return ATAPI_MAX_SIZE / 1024;
	else if(_is_ahcisym(symbol))
	{
		uint32 portno = _ahcisym2portno(symbol);
		return (uint32)((uint64)AhciGetSectorCount(portno) * 512UL / 1024UL);
	}
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
	CASCTEXT sign_str = SYSTEM_SIGNATURE_STRING;
	if(symbol == NULL)
		return FALSE;
	if(strcmp(symbol, "VS") == 0)
		return is_system_disk(EXPLICIT_SYSTEM_DISK);
	uint32 disk_size = get_disk_size(symbol);
	if(disk_size != 0)
	{
		uchar buffer[512];
		if(!read_sector(symbol, 0, buffer))
			return FALSE;
		if(	// 验证MBR的合法性。
			buffer[510] == 0x55
			&& buffer[511] == 0xaa
			// 验证磁盘的系统是否为ISystem。
			&& buffer[509 - 64] == SYSTEM_SIGNATURE_STRING[5]
			&& buffer[508 - 64] == SYSTEM_SIGNATURE_STRING[4]
			&& buffer[507 - 64] == SYSTEM_SIGNATURE_STRING[3]
			&& buffer[506 - 64] == SYSTEM_SIGNATURE_STRING[2]
			&& buffer[505 - 64] == SYSTEM_SIGNATURE_STRING[1]
			&& buffer[504 - 64] == SYSTEM_SIGNATURE_STRING[0])
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
		UtlCopyString(disk_list[disk_count++], DISK_SYMBOL_BUFFER_SIZE, symbol);
	}
	else if(strcmp(symbol, "VS") == 0)
		init_disk(EXPLICIT_SYSTEM_DISK);
	else if(strcmp(symbol, "HD") == 0)
	{
		// ATA。
		uint32 r = AtaInit();
		if(r & ATA_DISK0)
		{
			_INIT_DISK_RWBYTES(disk_count);
			UtlCopyString(disk_list[disk_count++], DISK_SYMBOL_BUFFER_SIZE, ATA_DISK0_SYMBOL);
		}
		if(r & ATA_DISK1)
		{
			_INIT_DISK_RWBYTES(disk_count);
			UtlCopyString(disk_list[disk_count++], DISK_SYMBOL_BUFFER_SIZE, ATA_DISK1_SYMBOL);
		}
		if(r & ATA_DISK2)
		{
			_INIT_DISK_RWBYTES(disk_count);
			UtlCopyString(disk_list[disk_count++], DISK_SYMBOL_BUFFER_SIZE, ATA_DISK2_SYMBOL);
		}
		if(r & ATA_DISK3)
		{
			_INIT_DISK_RWBYTES(disk_count);
			UtlCopyString(disk_list[disk_count++], DISK_SYMBOL_BUFFER_SIZE, ATA_DISK3_SYMBOL);
		}

		// AHCI。
		if(AhciInit())
		{
			ASCCHAR sym[3] = {'S', 'A', '\0'};
			uint32 ui;
			for(ui = 0; ui < AhciGetPortCount() && ui < 26; ui++, sym[1]++)
			{
				_INIT_DISK_RWBYTES(disk_count);
				UtlCopyString(disk_list[disk_count++], DISK_SYMBOL_BUFFER_SIZE, sym);
			}
		}

		// 检查ATA与SATA的主盘是否为系统盘。
		if(is_system_disk("SA"))
			disk_hard_disk_type = DISK_TYPE_SATA;
		else if(is_system_disk("DA"))
			disk_hard_disk_type = DISK_TYPE_ATA;
		else
			disk_hard_disk_type = DISK_TYPE_ATA;
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
	if(symbol == NULL)
		return;
	if(strcmp(symbol, "VA") == 0 || strcmp(symbol, "VB") == 0)
		destroy_vdisk(symbol);
	else if(strcmp(symbol, "VS") == 0)
		destroy_disk(EXPLICIT_SYSTEM_DISK);
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
	if(symbol == NULL)
		return 0;
	if(strcmp(symbol, "VA") == 0 || strcmp(symbol, "VB") == 0)
		return sector_count_v();
	else if(strcmp(symbol, "VS") == 0)
		return sector_count(EXPLICIT_SYSTEM_DISK);
	else if(_is_atasym(symbol))
		return AtaGetSectorCount(symbol);
	else if(strcmp(symbol, "CA") == 0
			|| strcmp(symbol, "CB") == 0
			|| strcmp(symbol, "CC") == 0
			|| strcmp(symbol, "CD") == 0)
		return ATAPI_SECTOR512_COUNT;
	else if(_is_ahcisym(symbol))
	{
		uint32 portno = _ahcisym2portno(symbol);
		return AhciGetSectorCount(portno);
	}
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
	if(strcmp(symbol, "VS") == 0)
		return disk_wbytes(EXPLICIT_SYSTEM_DISK);
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
	if(strcmp(symbol, "VS") == 0)
		return disk_rbytes(EXPLICIT_SYSTEM_DISK);
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
	if(symbol == NULL || buffer == NULL)
		return FALSE;
	BOOL r = FALSE;
	if(strcmp(symbol, "VA") == 0 || strcmp(symbol, "VB") == 0)
		r = read_sector_v(symbol, pos, buffer);
	else if(strcmp(symbol, "VS") == 0)
		r = read_sector(EXPLICIT_SYSTEM_DISK, pos, buffer);
	else if(_is_atasym(symbol))
		r = AtaReadSector(symbol, pos, buffer);
	else if(strcmp(symbol, "CA") == 0)
		r = AtapiReadSector512(	ATA_BUS_PRIMARY,
									ATA_DRIVE_MASTER,
									pos,
									buffer);
	else if(strcmp(symbol, "CB") == 0)
		r = AtapiReadSector512(	ATA_BUS_PRIMARY,
									ATA_DRIVE_SLAVE,
									pos,
									buffer);
	else if(strcmp(symbol, "CC") == 0)
		r = AtapiReadSector512(	ATA_BUS_SECONDARY,
									ATA_DRIVE_MASTER,
									pos,
									buffer);
	else if(strcmp(symbol, "CD") == 0)
		r = AtapiReadSector512(	ATA_BUS_SECONDARY,
									ATA_DRIVE_SLAVE,
									pos,
									buffer);
	else if(_is_ahcisym(symbol))
	{
		uint32 portno = _ahcisym2portno(symbol);
		r = AhciRead(portno, pos, 0, 1, (WORD *)buffer);
	}
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
	if(symbol == NULL || buffer == NULL)
		return FALSE;
	BOOL r = FALSE;
	if(strcmp(symbol, "VA") == 0 || strcmp(symbol, "VB") == 0)
		r = write_sector_v(symbol, pos, buffer);
	else if(strcmp(symbol, "VS") == 0)
		r = write_sector(EXPLICIT_SYSTEM_DISK, pos, buffer);
	else if(_is_atasym(symbol))
		r = AtaWriteSector(symbol, pos, buffer);
	else if(strcmp(symbol, "CA") == 0
			|| strcmp(symbol, "CB") == 0
			|| strcmp(symbol, "CC") == 0
			|| strcmp(symbol, "CD") == 0)
		return FALSE;
	else if(_is_ahcisym(symbol))
	{
		uint32 portno = _ahcisym2portno(symbol);
		r = AhciWrite(portno, pos, 0, 1, (WORD *)buffer);
	}
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
	if(symbol == NULL || buffer == NULL)
		return FALSE;
	BOOL r = FALSE;
	if(strcmp(symbol, "VA") == 0 || strcmp(symbol, "VB") == 0)
		r = read_sectors_v(symbol, pos, count, buffer);
	else if(strcmp(symbol, "VS") == 0)
		r = read_sectors(EXPLICIT_SYSTEM_DISK, pos, count, buffer);
	else if(_is_atasym(symbol))
		r = AtaReadSectors(symbol, pos, count, buffer);
	else if(strcmp(symbol, "CA") == 0)
		r = AtapiReadSector512s(	ATA_BUS_PRIMARY,
									ATA_DRIVE_MASTER,
									pos,
									count,
									buffer);
	else if(strcmp(symbol, "CB") == 0)
		r = AtapiReadSector512s(	ATA_BUS_PRIMARY,
									ATA_DRIVE_SLAVE,
									pos,
									count,
									buffer);
	else if(strcmp(symbol, "CC") == 0)
		r = AtapiReadSector512s(	ATA_BUS_SECONDARY,
									ATA_DRIVE_MASTER,
									pos,
									count,
									buffer);
	else if(strcmp(symbol, "CD") == 0)
		r = AtapiReadSector512s(	ATA_BUS_SECONDARY,
									ATA_DRIVE_SLAVE,
									pos,
									count,
									buffer);
	else if(_is_ahcisym(symbol))
	{
		uint32 portno = _ahcisym2portno(symbol);
		r = AhciRead(portno, pos, 0, count, (WORD *)buffer);
	}
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
	if(symbol == NULL || buffer == NULL)
		return FALSE;
	BOOL r = FALSE;
	if(strcmp(symbol, "VA") == 0 || strcmp(symbol, "VB") == 0)
		r = write_sectors_v(symbol, pos, count, buffer);
	else if(strcmp(symbol, "VS") == 0)
		r = write_sectors(EXPLICIT_SYSTEM_DISK, pos, count, buffer);
	else if(_is_atasym(symbol))
		r = AtaWriteSectors(symbol, pos, count, buffer);
	else if(strcmp(symbol, "CA") == 0
			|| strcmp(symbol, "CB") == 0
			|| strcmp(symbol, "CC") == 0
			|| strcmp(symbol, "CD") == 0)
		return FALSE;
	else if(_is_ahcisym(symbol))
	{
		uint32 portno = _ahcisym2portno(symbol);
		r = AhciWrite(portno, pos, 0, count, (WORD *)buffer);
	}
	else
		return FALSE;
	if(r)
	{
		uint32 idx = _disk_index(symbol);
		_INC_DISK_WBYTES(idx, count * DISK_BYTES_PER_SECTOR);
	}
	return r;
}
