/**
	@File:			hdisk.c
	@Author:		Ihsoh
	@Date:			2014-7-14
	@Description:
		提供支持ATA协议的硬盘的操作功能。
*/

#include "hdisk.h"
#include "types.h"
#include "386.h"
#include "log.h"
#include "atapi.h"
#include "cmlock.h"

#include <ilib/string.h>

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
#define	MAX_LOCK_RETRY	0x00100000

#define	_LOCK_TID_NONE		-2	// 没有任何任务获取到锁。
#define	_LOCK_TID_KERNEL	-1	// 内核获取了锁。

// 如果为TRUE则说明当前有任务占用ATA驱动。
static BOOL _lock		= FALSE;

// 指示哪个任务正在占用ATA驱动。
static int32 _lock_tid	= _LOCK_TID_NONE;

/**
	@Function:		lock_hdisk
	@Access:		Private
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
	// 在规定重试次数内尝试获取锁。
	int32 i;
	for(i = 0; i < MAX_LOCK_RETRY; i++)
	{
		if(!_lock)
			break;
		int32 i1;
		for(i1 = 0; i1 < 0x1000; i1++)
			asm volatile ("pause");
	}

	// 检测是否获取到锁，如果未获取则返回FALSE。
	if(_lock)
		return FALSE;

	common_lock();
	_lock = TRUE;
	_lock_tid = get_running_tid();
	common_unlock();
	return TRUE;
}

/**
	@Function:		lock_hdisk
	@Access:		Private
	@Description:
		解锁硬盘。
	@Parameters:
	@Return:
*/
static
void
unlock_hdisk(void)
{
	_lock = FALSE;
	_lock_tid = _LOCK_TID_NONE;
}

/**
	@Function:		hdisk_locked
	@Access:		Public
	@Description:
		获取ATA驱动的锁的状态。
	@Parameters:
	@Return:
		BOOL
			返回TRUE则代表ATA驱动的锁被锁定中。
*/
BOOL
hdisk_locked(void)
{
	return _lock;
}

/**
	@Function:		hdisk_lock_tid
	@Access:		Public
	@Description:
		获取正在占用ATA驱动的任务ID。
	@Parameters:
	@Return:
		int32
			正在占用ATA驱动的任务ID。
*/
int32
hdisk_lock_tid(void)
{
	return _lock_tid;
}

/**
	@Function:		hdisk_attempt_to_unlock
	@Access:		Public
	@Description:
		尝试解除ATA驱动的锁。
	@Parameters:
		tid, int32, IN
			任务ID。如果该值等于当前正在占用ATA驱动的锁的任务ID相等，则解除锁。
	@Return:
*/
void
hdisk_attempt_to_unlock(IN int32 tid)
{
	if(tid == _lock_tid)
		unlock_hdisk();
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
	memcpy(&hdisk0, (uint8 *)(0x7c00 + 512 - 2 - 0x10 * 2), sizeof(struct HardDiskArguments));
	
	//初始化第二个硬盘
	memcpy(&hdisk1, (uint8 *)(0x7c00 + 512 - 2 - 0x10 * 1), sizeof(struct HardDiskArguments));

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
		while(inb(HD_STATUS) & STAT_BUSY)
			asm volatile ("pause");
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
		while(inb(HD_STATUS) & STAT_BUSY)
			asm volatile ("pause");
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
	if(atapi_locked())
		return FALSE;
	if(!lock_hdisk())
		return FALSE;
	uint32 ui;
	for(ui = 0; ui < MAX_RETRY; ui++)
	{
		BOOL r = _read_sector_h(symbol, pos, buffer);
		if(r)
		{
			unlock_hdisk();
			return TRUE;
		}
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
	if(atapi_locked())
		return FALSE;
	if(!lock_hdisk())
		return FALSE;
	uint32 ui;
	for(ui = 0; ui < MAX_RETRY; ui++)
	{
		BOOL r = _write_sector_h(symbol, pos, buffer);
		if(r)
		{
			unlock_hdisk();
			return TRUE;
		}
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
	if(atapi_locked())
		return FALSE;
	if(!lock_hdisk())
		return FALSE;
	uint32 ui;
	for(ui = 0; ui < MAX_RETRY; ui++)
	{
		BOOL r = _read_sectors_h(symbol, pos, count, buffer);
		if(r)
		{
			unlock_hdisk();
			return TRUE;
		}
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
	if(atapi_locked())
		return FALSE;
	if(!lock_hdisk())
		return FALSE;
	uint32 ui;
	for(ui = 0; ui < MAX_RETRY; ui++)
	{
		BOOL r = _write_sectors_h(symbol, pos, count, buffer);
		if(r)
		{
			unlock_hdisk();
			return TRUE;
		}
	}
	unlock_hdisk();
	return FALSE;
}
