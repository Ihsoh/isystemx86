/**
	@File:			ifs1hdisk.c
	@Author:		Ihsoh
	@Date:			2014-7-14
	@Description:
		提供支持ATA协议的硬盘的操作功能。
*/

#include "ifs1hdisk.h"
#include "types.h"
#include "386.h"
#include "log.h"

#include <string.h>

struct HardDiskArguments
{
	uint16	cly;
	uint8	head;
	uint16	unused0;
	uint16	wpc;
	uint8	unused1;
	uint8	control_byte;
	uint8	unused2;
	uint8	unused3;
	uint8	unused4;
	uint16	lzone;
	uint8	spt;
	uint8	unused5;
} __attribute__((packed));

static struct HardDiskArguments hdisk0;
static struct HardDiskArguments hdisk1;

#define	HD_DATA		0x1f0
#define	HD_CMD		0x3f6
#define	HD_STATUS	0x1f7
#define	HD_COMMAND	HD_STATUS

#define	STAT_ERR	0x01
#define	STAT_INDEX	0x02
#define	STAT_ECC	0x04
#define	STAT_DRQ	0x08
#define	STAT_SEEK	0x10
#define	STAT_WRERR	0x20
#define	STAT_READY	0x40
#define	STAT_BUSY	0x80

#define	WIN_RESTORE		0x10
#define	WIN_READ		0x20
#define	WIN_WRITE		0x30
#define	WIN_VERIFY		0x40
#define	WIN_FORMAT		0x50
#define	WIN_INIT		0x60
#define	WIN_SEEK		0x70
#define	WIN_DIAGNOSE	0x90
#define	WIN_SPECIFY		0x91

#define	HDRead(port, buffer, sectors)	\
	asm volatile ("cld;rep;insw"::"d"(port), "D"(buffer), "c"(sectors));

#define	HDWrite(port, buffer, sectors)	\
	asm volatile ("cld;rep;outsw"::"d"(port), "S"(buffer), "c"(sectors));

#define	MAX_TIMEOUT	0x0fffffff
#define	MAX_RETRY	0x100

static volatile BOOL lock = FALSE;

/**
	@Function:		lock_hdisk
	@Access:		Public
	@Description:
		锁住硬盘。
	@Parameters:
	@Return:
		BOOL
			返回TRUE则获取锁成功，否则获取锁失败。
*/
static
BOOL
lock_hdisk(void)
{
	if(lock)
		return FALSE;
	lock = TRUE;
	return TRUE;
}

/**
	@Function:		lock_hdisk
	@Access:		Public
	@Description:
		解锁硬盘。
	@Parameters:
	@Return:
*/
static
void
unlock_hdisk(void)
{
	lock = FALSE;
}

/**
	@Function:		init_hdisk
	@Access:		Public
	@Description:
		初始化硬盘。
	@Parameters:
	@Return:
		uint32
			返回值的低两位有效，其它位均为0。位0为1时表示硬盘1初始化成功，
			位1为1时表示硬盘2初始化成功。
*/
uint32
init_hdisk(void)
{
	uint32 r = 0x3;
	
	//初始化第一个硬盘
	memcpy(&hdisk0, (uchar *)(0x7c00 + 512 - 2 - 0x10 * 2), sizeof(struct HardDiskArguments));
	
	//初始化第二个硬盘
	memcpy(&hdisk1, (uchar *)(0x7c00 + 512 - 2 - 0x10 * 1), sizeof(struct HardDiskArguments));

	return r;	
}

/**
	@Function:		dargs
	@Access:		Private
	@Description:
		获取指定硬盘标识符的硬盘参数表。
	@Parameters:
		symbol, int8 *, IN
			硬盘标识符。
	@Return:
		struct HardDiskArguments *
			硬盘参数表。
*/
static
struct HardDiskArguments * 
dargs(IN int8 * symbol)
{
	if(symbol == NULL)
		return NULL;
	else if(strcmp(symbol, "DA") == 0)
		return &hdisk0;
	else if(strcmp(symbol, "DB") == 0)
		return &hdisk1;
	else
		return NULL;
}

/**
	@Function:		get_hdisk_size
	@Access:		Public
	@Description:
		获取指定的硬盘的容量，单位是KB。
	@Parameters:
		symbol, int8 *, IN
			硬盘标识符。
	@Return:
		uint32
			硬盘的容量，单位是KB。		
*/
uint32
get_hdisk_size(IN int8 * symbol)
{
	uint32 size = 0;
	struct HardDiskArguments * hdisk = dargs(symbol);
	if(hdisk == NULL)
		return 0;
	size = hdisk->cly * hdisk->head * hdisk->spt * 512 / 1024;
	return size;
}

/**
	@Function:		sector_count_h
	@Access:		Public
	@Description:
		获取指定的硬盘的扇区数量。
	@Parameters:
		symbol, int8 *, IN
			硬盘标识符。
	@Return:
		uint32
			硬盘的扇区数量		
*/
uint32
sector_count_h(IN int8 * symbol)
{
	uint32 sector_count = 0;
	struct HardDiskArguments * hdisk = dargs(symbol);
	if(hdisk == NULL)
		return 0;
	sector_count = hdisk->cly * hdisk->head * hdisk->spt;
	return sector_count;
}

/**
	@Function:		chs
	@Access:		Private
	@Description:
		把硬盘的LBA表示法转换为硬盘的CHS表示法。
	@Parameters:
		symbol, int8 *, IN
			硬盘标识符。
		pos, uint32, IN
			LBA表示法的逻辑地址。
		c, uint32, OUT
			CHS表示法的磁道号。
		h, uint32, OUT
			CHS表示法的磁头号。
		s, uint32, OUT
			CHS表示法的扇区号。
	@Return:	
*/
static
void
chs(IN int8 * symbol, 
	IN uint32 pos, 
	OUT uint32 * c, 
	OUT uint32 * h, 
	OUT uint32 * s)
{
	struct HardDiskArguments * hdisk = dargs(symbol);
	if(hdisk == NULL)
		return;
	uint32 sector_count = sector_count_h(symbol);
	*s = pos % hdisk->spt + 1;
	pos /= hdisk->spt;
	*h = pos % hdisk->head;
	*c = pos / hdisk->head;
}

/**
	@Function:		wait_ready
	@Access:		Private
	@Description:
		等待硬盘控制器进入READY状态。
	@Parameters:
	@Return:
		BOOL
			返回TRUE则等待成功，否则失败。		
*/
static
BOOL
wait_ready(void)
{
	uint32 timeout_counter = 0;
	while(1)
	{
		while(inb(HD_STATUS) & STAT_BUSY);
		uint8 state = inb(HD_STATUS);
		if((state & STAT_ERR) || (state & STAT_ECC) || (state & STAT_WRERR))
		{
			/*log(LOG_ERROR,
				"ATA Driver causes a error when the driver is waiting ready state.");*/
			return FALSE;
		}
		if(state & STAT_READY)
			break;
		if(++timeout_counter == MAX_TIMEOUT)
		{
			/*log(LOG_WARNING,
				"ATA Driver causes a timeout when the driver is waiting ready state.");*/
			return FALSE;
		}
	}
	return TRUE;
}

/**
	@Function:		wait_data
	@Access:		Private
	@Description:
		等待数据准备就绪。
	@Parameters:
	@Return:
		BOOL
			返回TRUE则等待成功，否则失败。		
*/
static
BOOL
wait_data(void)
{
	uint32 timeout_counter = 0;
	while(1)
	{
		while(inb(HD_STATUS) & STAT_BUSY);
		uint8 state = inb(HD_STATUS);
		if((state & STAT_ERR) || (state & STAT_ECC) || (state & STAT_WRERR))
		{
			/*log(LOG_ERROR,
				"ATA Driver causes a error when the driver is waiting data.");*/
			return FALSE;
		}
		if(state & STAT_DRQ)
			break;
		if(++timeout_counter == MAX_TIMEOUT)
		{
			/*log(LOG_WARNING,
				"ATA Driver causes a timeout when the driver is waiting data.");*/
			return FALSE;
		}
	}
	return TRUE;
}

/**
	@Function:		_read_sector_h
	@Access:		Private
	@Description:
		读取一个扇区的数据。
	@Parameters:
		symbol, int8 *, IN
			硬盘标识符。
		pos, uint32, IN
			LBA表示法的逻辑地址。
		buffer, uint8 *, OUT
			指向用于储存扇区数据的缓冲区。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
static
BOOL
_read_sector_h(	IN int8 * symbol, 
				IN uint32 pos, 
				OUT uint8 * buffer)
{
	if(buffer == NULL)
		return FALSE;
	struct HardDiskArguments * hdisk = dargs(symbol);
	if(hdisk == NULL)
		return FALSE;
	uint32 c, h, s;
	chs(symbol, pos, &c, &h, &s);
	outb(HD_CMD, hdisk->control_byte);
	if(!wait_ready())
		return FALSE;
	uint16 port = HD_DATA;
	outb(++port, hdisk->wpc >> 2);
	outb(++port, 1);
	outb(++port, s);
	outb(++port, c);
	outb(++port, c >> 8);
	if(strcmp(symbol, "DA") == 0)
		outb(++port, 0xa0 | 0x00 | 0x0f & h);
	else if(strcmp(symbol, "DB") == 0)
		outb(++port, 0xa0 | 0x10 | 0x0f & h);
	else 
		return FALSE;
	if(!wait_ready())
		return FALSE;
	outb(++port, WIN_READ);
	if(!wait_data())
		return FALSE;
	HDRead(HD_DATA, buffer, 256);
	return TRUE;
}

/**
	@Function:		read_sector_h
	@Access:		Public
	@Description:
		读取一个扇区的数据。
	@Parameters:
		symbol, int8 *, IN
			硬盘标识符。
		pos, uint32, IN
			LBA表示法的逻辑地址。
		buffer, uint8 *, OUT
			指向用于储存扇区数据的缓冲区。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
read_sector_h(	IN int8 * symbol, 
				IN uint32 pos, 
				OUT uint8 * buffer)
{
	if(!lock_hdisk())
		return FALSE;
	uint32 ui;
	for(ui = 0; ui < MAX_RETRY; ui++)
		if(_read_sector_h(symbol, pos, buffer))
		{
			unlock_hdisk();
			return TRUE;
		}
	unlock_hdisk();
	return FALSE;
}

/**
	@Function:		_write_sector_h
	@Access:		Private
	@Description:
		写入数据到一个扇区。
	@Parameters:
		symbol, int8 *, IN
			硬盘标识符。
		pos, uint32, IN
			LBA表示法的逻辑地址。
		buffer, uint8 *, IN
			指向包含将要写入扇区的数据的缓冲区。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。	
*/
static
BOOL
_write_sector_h(IN int8 * symbol, 
				IN uint32 pos, 
				IN uint8 * buffer)
{
	if(buffer == NULL)
		return FALSE;
	struct HardDiskArguments * hdisk = dargs(symbol);
	if(hdisk == NULL)
		return FALSE;
	uint32 c, h, s;
	chs(symbol, pos, &c, &h, &s);
	outb(HD_CMD, hdisk->control_byte);
	if(!wait_ready())
		return FALSE;
	uint16 port = HD_DATA;
	outb(++port, hdisk->wpc >> 2);
	outb(++port, 1);
	outb(++port, s);
	outb(++port, c);
	outb(++port, c >> 8);
	if(strcmp(symbol, "DA") == 0)
		outb(++port, 0xa0 | 0x00 | 0x0f & h);
	else if(strcmp(symbol, "DB") == 0)
		outb(++port, 0xa0 | 0x10 | 0x0f & h);
	else
		return FALSE;
	if(!wait_ready())
		return FALSE;
	outb(++port, WIN_WRITE);
	if(!wait_data())
		return FALSE;
	HDWrite(HD_DATA, buffer, 256);
	return TRUE;
}

/**
	@Function:		write_sector_h
	@Access:		Public
	@Description:
		写入数据到一个扇区。
	@Parameters:
		symbol, int8 *, IN
			硬盘标识符。
		pos, uint32, IN
			LBA表示法的逻辑地址。
		buffer, uint8 *, IN
			指向包含将要写入扇区的数据的缓冲区。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。	
*/
BOOL
write_sector_h(	IN int8 * symbol, 
				IN uint32 pos, 
				IN uint8 * buffer)
{
	if(!lock_hdisk())
		return FALSE;
	uint32 ui;
	for(ui = 0; ui < MAX_RETRY; ui++)
		if(_write_sector_h(symbol, pos, buffer))
		{
			unlock_hdisk();
			return TRUE;
		}
	unlock_hdisk();
	return FALSE;
}

/**
	@Function:		_read_sectors_h
	@Access:		Private
	@Description:
		读取一个或多个扇区的数据。最多一次可以读取256个扇区的数据。
	@Parameters:
		symbol, int8 *, IN
			硬盘标识符。
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
static
BOOL
_read_sectors_h(IN int8 * symbol,
				IN uint32 pos, 
				IN uint8 count, 
				OUT uint8 * buffer)
{
	if(buffer == NULL)
		return FALSE;
	struct HardDiskArguments * hdisk = dargs(symbol);
	if(hdisk == NULL)
		return FALSE;
	uint32 c, h, s;
	chs(symbol, pos, &c, &h, &s);
	outb(HD_CMD, hdisk->control_byte);
	if(!wait_ready())
		return FALSE;
	uint16 port = HD_DATA;
	outb(++port, hdisk->wpc >> 2);
	outb(++port, count);
	outb(++port, s);
	outb(++port, c);
	outb(++port, c >> 8);
	if(strcmp(symbol, "DA") == 0)
		outb(++port, 0xa0 | 0x00 | 0x0f & h);
	else if(strcmp(symbol, "DB") == 0)
		outb(++port, 0xa0 | 0x10 | 0x0f & h);
	else
		return FALSE;
	if(!wait_ready())
		return FALSE;
	outb(++port, WIN_READ);
	int32 count_i32 = count == 0 ? 256 : count;
	int32 i;
	for(i = 0; i < count_i32; i++)
	{
		if(!wait_data())
			return FALSE;
		HDRead(HD_DATA, buffer, 256);
		buffer += 512;
	}
	return TRUE;
}

/**
	@Function:		read_sectors_h
	@Access:		Public
	@Description:
		读取一个或多个扇区的数据。最多一次可以读取256个扇区的数据。
	@Parameters:
		symbol, int8 *, IN
			硬盘标识符。
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
read_sectors_h(	IN int8 * symbol,
				IN uint32 pos,
				IN uint8 count,
				OUT uint8 * buffer)
{
	if(!lock_hdisk())
		return FALSE;
	uint32 ui;
	for(ui = 0; ui < MAX_RETRY; ui++)
		if(_read_sectors_h(symbol, pos, count, buffer))
		{
			unlock_hdisk();
			return TRUE;
		}
	unlock_hdisk();
	return FALSE;
}

/**
	@Function:		_write_sectors_h
	@Access:		Private
	@Description:
		写入数据到一个或多个扇区。最多一次可以写入256个扇区的数据。
	@Parameters:
		symbol, int8 *, IN
			硬盘标识符。
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
static
BOOL
_write_sectors_h(	IN int8 * symbol, 
					IN uint32 pos, 
					IN uint8 count, 
					IN uint8 * buffer)
{
	if(buffer == NULL)
		return FALSE;
	struct HardDiskArguments * hdisk = dargs(symbol);
	if(hdisk == NULL)
		return FALSE;
	uint32 c, h, s;
	chs(symbol, pos, &c, &h, &s);
	outb(HD_CMD, hdisk->control_byte);
	if(!wait_ready())
		return FALSE;
	uint16 port = HD_DATA;
	outb(++port, hdisk->wpc >> 2);
	outb(++port, count);
	outb(++port, s);
	outb(++port, c);
	outb(++port, c >> 8);
	if(strcmp(symbol, "DA") == 0)
		outb(++port, 0xa0 | 0x00 | 0x0f & h);
	else if(strcmp(symbol, "DB") == 0)
		outb(++port, 0xa0 | 0x10 | 0x0f & h);
	else
		return FALSE;
	if(!wait_ready())
		return FALSE;
	outb(++port, WIN_WRITE);
	int32 count_i32 = count == 0 ? 256 : count;
	int32 i;
	for(i = 0; i < count_i32; i++)
	{
		if(!wait_data())
			return FALSE;
		HDWrite(HD_DATA, buffer, 256);
		buffer += 512;
	}
	return TRUE;
}

/**
	@Function:		write_sectors_h
	@Access:		Public
	@Description:
		写入数据到一个或多个扇区。最多一次可以写入256个扇区的数据。
	@Parameters:
		symbol, int8 *, IN
			硬盘标识符。
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
write_sectors_h(IN int8 * symbol, 
				IN uint32 pos, 
				IN uint8 count, 
				IN uint8 * buffer)
{
	if(!lock_hdisk())
		return FALSE;
	uint32 ui;
	for(ui = 0; ui < MAX_RETRY; ui++)
		if(_write_sectors_h(symbol, pos, count, buffer))
		{
			unlock_hdisk();
			return TRUE;
		}
	unlock_hdisk();
	return FALSE;
}
