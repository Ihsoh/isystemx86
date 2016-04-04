/**
	@File:			ata.c
	@Author:		Ihsoh
	@Date:			2016-04-04
	@Description:
		提供支持ATA协议的硬盘的操作功能。
*/

#include "ata.h"
#include "types.h"
#include "386.h"
#include "atapi.h"
#include "cmlock.h"
#include "tasks.h"

#include <ilib/string.h>

#define	ATA_SECTOR_SIZE	512

#define	READ(port, buffer, sectors)	\
	asm volatile ("cld;rep;insw"::"d"(port), "D"(buffer), "c"(sectors));

#define	WRITE(port, buffer, sectors)	\
	asm volatile ("cld;rep;outsw"::"d"(port), "S"(buffer), "c"(sectors));

typedef struct
{
	int32		io_base;
	int32		control;
	int32		slave;
	ATAIdentify	identity;
	BOOL		existed;
} ATADevice, * ATADevicePtr;

#define	MAX_RETRY		0x100
#define	MAX_LOCK_RETRY	0x00100000

#define	_LOCK_TID_NONE		-2	// 没有任何任务获取到锁。
#define	_LOCK_TID_KERNEL	-1	// 内核获取了锁。

// 如果为TRUE则说明当前有任务占用ATA驱动。
static BOOL _lock		= FALSE;

// 指示哪个任务正在占用ATA驱动。
static int32 _lock_tid	= _LOCK_TID_NONE;

static ATADevice _ata_primary_master	= {.io_base = 0x1F0, .control = 0x3F6, .slave = 0, .existed = FALSE};
static ATADevice _ata_primary_slave		= {.io_base = 0x1F0, .control = 0x3F6, .slave = 1, .existed = FALSE};
static ATADevice _ata_secondary_master	= {.io_base = 0x170, .control = 0x376, .slave = 0, .existed = FALSE};
static ATADevice _ata_secondary_slave	= {.io_base = 0x170, .control = 0x376, .slave = 1, .existed = FALSE};

/**
	@Function:		_ata_lock
	@Access:		Private
	@Description:
		锁住ATA驱动。
	@Parameters:
	@Return:
		BOOL
			返回TRUE则获取锁成功，否则获取锁失败。
*/
static
BOOL
_ata_lock(void)
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
	@Function:		_ata_unlock
	@Access:		Private
	@Description:
		解锁ATA驱动。
	@Parameters:
	@Return:
*/
static
void
_ata_unlock(void)
{
	_lock = FALSE;
	_lock_tid = _LOCK_TID_NONE;
}

/**
	@Function:		ata_locked
	@Access:		Public
	@Description:
		获取ATA驱动的锁的状态。
	@Parameters:
	@Return:
		BOOL
			返回TRUE则代表ATA驱动的锁被锁定中。
*/
BOOL
ata_locked(void)
{
	return _lock;
}

/**
	@Function:		ata_lock_tid
	@Access:		Public
	@Description:
		获取正在占用ATA驱动的任务ID。
	@Parameters:
	@Return:
		int32
			正在占用ATA驱动的任务ID。
*/
int32
ata_lock_tid(void)
{
	return _lock_tid;
}

/**
	@Function:		ata_attempt_to_unlock
	@Access:		Public
	@Description:
		尝试解除ATA驱动的锁。
	@Parameters:
		tid, int32, IN
			任务ID。如果该值等于当前正在占用ATA驱动的锁的任务ID，则解除锁。
	@Return:
*/
void
ata_attempt_to_unlock(IN int32 tid)
{
	if(tid == _lock_tid)
		_ata_unlock();
}

/**
	@Function:		_ata_sector_count
	@Access:		Private
	@Description:
		获取ATA设备的扇区数。
	@Parameters:
		dev, ATADevicePtr, IN
			指向ATA设备对象的指针。
	@Return:
		uint64
			ATA设备的扇区数。
*/
static
uint64
_ata_sector_count(IN ATADevicePtr dev)
{
	uint64 sectors = dev->identity.sectors_48;
	if(sectors == 0)
		sectors = dev->identity.sectors_28;
	return sectors;
}

/**
	@Function:		_ata_io_wait
	@Access:		Private
	@Description:
		等待ATA设备的I/O。
	@Parameters:
		dev, ATADevicePtr, IN
			指向ATA设备对象的指针。
	@Return:
*/
static
void
_ata_io_wait(IN ATADevicePtr dev)
{
	inb(dev->io_base + ATA_REG_ALTSTATUS);
	inb(dev->io_base + ATA_REG_ALTSTATUS);
	inb(dev->io_base + ATA_REG_ALTSTATUS);
	inb(dev->io_base + ATA_REG_ALTSTATUS);
}

/**
	@Function:		_ata_status_wait
	@Access:		Private
	@Description:
		获取ATA设备的状态。
	@Parameters:
		dev, ATADevicePtr, IN
			指向ATA设备对象的指针。
		timeout, int32, IN
			如果设备处于忙状态，这个指定重试次数。
	@Return:
		int32
			ATA设备的状态。
*/
static
int32
_ata_status_wait(	IN ATADevicePtr dev,
					IN int32 timeout)
{
	int32 status;
	if(timeout > 0)
	{
		int32 i = 0;
		while(	((status = inb(dev->io_base + ATA_REG_STATUS)) & ATA_SR_BSY)
				&& i++ < timeout);
	}
	else
		while((status = inb(dev->io_base + ATA_REG_STATUS)) & ATA_SR_BSY);
	return status;
}

/**
	@Function:		_ata_wait
	@Access:		Private
	@Description:
		等待ATA设备。
	@Parameters:
		dev, ATADevicePtr, IN
			指向ATA设备对象的指针。
		advanced, BOOL, IN
			指示是否检查ATA设备的状态。
	@Return:
		BOOL
			如果参数advanced为FALSE，恒为TRUE。
			如果参数advanced为TRUE并且设备未出错，则返回TRUE。
*/
static
BOOL
_ata_wait(	IN ATADevicePtr dev,
			IN BOOL advanced)
{
	uint8 status = 0;
	_ata_io_wait(dev);
	status = _ata_status_wait(dev, 0);
	if(advanced)
	{
		status = inb(dev->io_base + ATA_REG_STATUS);
		if(	status & ATA_SR_ERR
			|| status & ATA_SR_DF
			|| !(status & ATA_SR_DRQ))
			return FALSE;
	}
	return TRUE;
}

/**
	@Function:		_ata_soft_reset
	@Access:		Private
	@Description:
		软重置ATA设备。
	@Parameters:
		dev, ATADevicePtr, IN
			指向ATA设备对象的指针。
	@Return:
*/
static
void
_ata_soft_reset(IN ATADevicePtr dev)
{
	outb(dev->control, 0x04);
	_ata_io_wait(dev);
	outb(dev->control, 0x00);
}

/**
	@Function:		_ata_device_init
	@Access:		Private
	@Description:
		初始化ATA设备。
	@Parameters:
		dev, ATADevicePtr, IN OUT
			指向ATA设备对象的指针。
	@Return:
*/
static
void
_ata_device_init(IN OUT ATADevicePtr dev)
{
	int32 i;

	outb(dev->io_base + 1, 1);
	outb(dev->control, 0);

	outb(dev->io_base + ATA_REG_HDDEVSEL, 0xa0 | dev->slave << 4);
	_ata_io_wait(dev);

	outb(dev->io_base + ATA_REG_COMMAND, ATA_CMD_IDENTIFY);
	_ata_io_wait(dev);

	int32 status = inb(dev->io_base + ATA_REG_COMMAND);

	_ata_wait(dev, FALSE);

	uint16 * buf = (uint16 *)&dev->identity;
	for(i = 0; i < 256; i++)
		buf[i] = inw(dev->io_base);

	uint8 * ptr = (uint8 *)&dev->identity.model;
	for(i = 0; i < 39; i += 2)
	{
		uint8 tmp = ptr[i + 1];
		ptr[i + 1] = ptr[i];
		ptr[i] = tmp;
	}

	outb(dev->io_base + ATA_REG_CONTROL, 0x02);
}

/**
	@Function:		_ata_device_detect
	@Access:		Private
	@Description:
		探测ATA设备。如果ATA设备存在，则初始化ATA设备。
	@Parameters:
		dev, ATADevicePtr, IN OUT
			指向ATA设备对象的指针。
	@Return:
		BOOL
			如果设备存在则返回TRUE。
*/
static
BOOL
_ata_device_detect(IN OUT ATADevicePtr dev)
{
	_ata_soft_reset(dev);
	_ata_io_wait(dev);
	outb(dev->io_base + ATA_REG_HDDEVSEL, 0xa0 | dev->slave << 4);
	_ata_io_wait(dev);
	_ata_status_wait(dev, 10000);

	uint8 cl = inb(dev->io_base + ATA_REG_LBA1);
	uint8 ch = inb(dev->io_base + ATA_REG_LBA2);

	// 没有设备。
	if(cl == 0xff && ch == 0xff)
		return FALSE;

	// 并行ATA设备或模拟的SATA设备。
	if(	(cl == 0x00 && ch == 0x00)
		|| (cl == 0x3c && ch == 0xc3))
	{
		dev->existed = TRUE;
		_ata_device_init(dev);
		return TRUE;
	}

	return FALSE;
}

/**
	@Function:		_ata_device_read_sectors
	@Access:		Private
	@Description:
		读取ATA设备的扇区。
		该函数不允许在一次调用未结束时再次调用，以及当ATAPI驱动正在进行I/O时，也不允许调用该函数。
	@Parameters:
		dev, ATADevicePtr, IN
			指向ATA设备对象的指针。
		lba, uint32, IN
			LBA地址。
		count, uint8, IN
			读取的扇区数。如果等于0，则代表读取256个扇区。
		buf, uint8 *, OUT
			指向用于保存数据的缓冲区的指针。
	@Return:
		BOOL
			返回TRUE则读取成功。
*/
static
BOOL
_ata_device_read_sectors(	IN ATADevicePtr dev,
							IN uint32 lba,
							IN uint8 count,
							OUT uint8 * buf)
{
	uint16 bus = dev->io_base;
	uint8 slave = dev->slave;

	outb(bus + ATA_REG_CONTROL, 0x02);

	_ata_wait(dev, FALSE);

	outb(bus + ATA_REG_HDDEVSEL, 0xe0 | slave << 4 | (lba & 0x0f000000) >> 24);
	outb(bus + ATA_REG_FEATURES, 0x00);
	outb(bus + ATA_REG_SECCOUNT0, count);
	outb(bus + ATA_REG_LBA0, (lba & 0x000000ff) >>  0);
	outb(bus + ATA_REG_LBA1, (lba & 0x0000ff00) >>  8);
	outb(bus + ATA_REG_LBA2, (lba & 0x00ff0000) >> 16);
	outb(bus + ATA_REG_COMMAND, ATA_CMD_READ_PIO);

	int32 count_i32 = count == 0 ? 256 : count;
	int32 size = ATA_SECTOR_SIZE / 2;
	int32 i;
	for(i = 0; i < count_i32; i++)
	{
		if(!_ata_wait(dev, TRUE))
			goto err;
		READ(bus, buf, size);
		buf += ATA_SECTOR_SIZE;
	}
	_ata_wait(dev, FALSE);

	return TRUE;
err:
	return FALSE;
}

/**
	@Function:		_ata_device_write_sectors
	@Access:		Private
	@Description:
		写入ATA设备的扇区。
		该函数不允许在一次调用未结束时再次调用，以及当ATAPI驱动正在进行I/O时，也不允许调用该函数。
	@Parameters:
		dev, ATADevicePtr, IN
			指向ATA设备对象的指针。
		lba, uint32, IN
			LBA地址。
		count, uint8, IN
			写入的扇区数。如果等于0，则代表写入256个扇区。
		buf, uint8 *, IN
			指向用于保存数据的缓冲区的指针。
	@Return:
		BOOL
			返回TRUE则写入成功。
*/
static
BOOL
_ata_device_write_sectors(	IN ATADevicePtr dev,
							IN uint32 lba,
							IN uint8 count,
							IN uint8 * buf)
{
	uint16 bus = dev->io_base;
	uint8 slave = dev->slave;

	outb(bus + ATA_REG_CONTROL, 0x02);

	_ata_wait(dev, FALSE);
	outb(bus + ATA_REG_HDDEVSEL, 0xe0 | slave << 4 | (lba & 0x0f000000) >> 24);
	_ata_wait(dev, FALSE);

	outb(bus + ATA_REG_FEATURES, 0x00);
	outb(bus + ATA_REG_SECCOUNT0, count);
	outb(bus + ATA_REG_LBA0, (lba & 0x000000ff) >>  0);
	outb(bus + ATA_REG_LBA1, (lba & 0x0000ff00) >>  8);
	outb(bus + ATA_REG_LBA2, (lba & 0x00ff0000) >> 16);
	outb(bus + ATA_REG_COMMAND, ATA_CMD_WRITE_PIO);

	int32 count_i32 = count == 0 ? 256 : count;
	int32 size = ATA_SECTOR_SIZE / 2;
	int32 i;
	for(i = 0; i < count_i32; i++)
	{
		if(!_ata_wait(dev, TRUE))
			goto err;
		WRITE(bus, buf, size);
		outb(bus + 0x07, ATA_CMD_CACHE_FLUSH);
		buf += ATA_SECTOR_SIZE;
	}
	_ata_wait(dev, FALSE);

	return TRUE;
err:
	return FALSE;
}

/**
	@Function:		_symbol2dev
	@Access:		Private
	@Description:
		根据ATA设备符号获取ATA设备对象。
	@Parameters:
		symbol, CASCTEXT, IN
			ATA设备符号。
	@Return:
		ATADevicePtr
			指向ATA设备对象的指针。
*/
static
ATADevicePtr
_symbol2dev(IN CASCTEXT symbol)
{
	if(strcmp(symbol, ATA_DISK0_SYMBOL) == 0)
		return &_ata_primary_master;
	else if(strcmp(symbol, ATA_DISK1_SYMBOL) == 0)
		return &_ata_primary_slave;
	else if(strcmp(symbol, ATA_DISK2_SYMBOL) == 0)
		return &_ata_secondary_master;
	else if(strcmp(symbol, ATA_DISK3_SYMBOL) == 0)
		return &_ata_secondary_slave;
	else
		return NULL;
}

/**
	@Function:		ata_init
	@Access:		Public
	@Description:
		初始化ATA驱动。
	@Parameters:
	@Return:
		uint32
			这些值的组合：ATA_DISK0、ATA_DISK1、ATA_DISK2、ATA_DISK3。
			如果返回值的指定位被设置，则代表对应的ATA设备存在。
*/
uint32
ata_init(void)
{
	uint32 r = 0;

	if(_ata_device_detect(&_ata_primary_master))
		r |= ATA_DISK0;
	if(_ata_device_detect(&_ata_primary_slave))
		r |= ATA_DISK1;
	if(_ata_device_detect(&_ata_secondary_master))
		r |= ATA_DISK2;
	if(_ata_device_detect(&_ata_secondary_slave))
		r |= ATA_DISK3;

	return r;
}

/**
	@Function:		ata_is_existed
	@Access:		Public
	@Description:
		确认ATA设备是否存在。
	@Parameters:
		symbol, CASCTEXT, IN
			ATA设备符号。
	@Return:
		BOOL
			返回TRUE则存在，否则不存在。
*/
BOOL
ata_is_existed(IN CASCTEXT symbol)
{
	ATADevicePtr dev = _symbol2dev(symbol);
	if(dev != NULL)
		return dev->existed;
	else
		return FALSE;
}

/**
	@Function:		ata_sector_count
	@Access:		Public
	@Description:
		获取ATA设备的扇区数。
	@Parameters:
		symbol, CASCTEXT, IN
			ATA设备符号。
	@Return:
		uint64
			ATA设备的扇区数。
*/
uint64
ata_sector_count(IN CASCTEXT symbol)
{
	ATADevicePtr dev = _symbol2dev(symbol);
	if(dev != NULL)
		return _ata_sector_count(dev);
	else
		return 0UL;
}

/**
	@Function:		ata_device_read_sectors
	@Access:		Public
	@Description:
		读取ATA设备的扇区。
	@Parameters:
		symbol, CASCTEXT, IN
			ATA设备符号。
		lba, uint32, IN
			LBA地址。
		count, uint8, IN
			读取的扇区数。如果等于0，则代表读取256个扇区。
		buf, uint8 *, OUT
			指向用于保存数据的缓冲区的指针。
	@Return:
		BOOL
			返回TRUE则读取成功。
*/
BOOL
ata_device_read_sectors(IN CASCTEXT symbol,
						IN uint32 lba,
						IN uint8 count,
						OUT uint8 * buf)
{
	if(atapi_locked())
		return FALSE;
	if(!_ata_lock())
		return FALSE;
	ATADevicePtr dev = _symbol2dev(symbol);
	uint32 ui;
	for(ui = 0; ui < MAX_RETRY; ui++)
	{
		BOOL r = _ata_device_read_sectors(dev, lba, count, buf);
		if(r)
		{
			_ata_unlock();
			return TRUE;
		}
	}
	_ata_unlock();
	return FALSE;
}

/**
	@Function:		ata_device_write_sectors
	@Access:		Public
	@Description:
		写入ATA设备的扇区。
	@Parameters:
		symbol, CASCTEXT, IN
			ATA设备符号。
		lba, uint32, IN
			LBA地址。
		count, uint8, IN
			写入的扇区数。如果等于0，则代表写入256个扇区。
		buf, uint8 *, IN
			指向用于保存数据的缓冲区的指针。
	@Return:
		BOOL
			返回TRUE则写入成功。
*/
BOOL
ata_device_write_sectors(	IN CASCTEXT symbol,
							IN uint32 lba,
							IN uint8 count,
							IN uint8 * buf)
{
	if(atapi_locked())
		return FALSE;
	if(!_ata_lock())
		return FALSE;
	ATADevicePtr dev = _symbol2dev(symbol);
	uint32 ui;
	for(ui = 0; ui < MAX_RETRY; ui++)
	{
		BOOL r = _ata_device_write_sectors(dev, lba, count, buf);
		if(r)
		{
			_ata_unlock();
			return TRUE;
		}
	}
	_ata_unlock();
	return FALSE;
}

/**
	@Function:		ata_device_read_sector
	@Access:		Public
	@Description:
		读取ATA设备的一个扇区。
	@Parameters:
		symbol, CASCTEXT, IN
			ATA设备符号。
		lba, uint32, IN
			LBA地址。
		buf, uint8 *, OUT
			指向用于保存数据的缓冲区的指针。
	@Return:
		BOOL
			返回TRUE则读取成功。
*/
BOOL
ata_device_read_sector(	IN CASCTEXT symbol,
						IN uint32 lba,
						OUT uint8 * buf)
{
	return ata_device_read_sectors(symbol, lba, 1, buf);
}

/**
	@Function:		ata_device_write_sector
	@Access:		Public
	@Description:
		写入ATA设备的一个扇区。
	@Parameters:
		symbol, CASCTEXT, IN
			ATA设备符号。
		lba, uint32, IN
			LBA地址。
		buf, uint8 *, IN
			指向用于保存数据的缓冲区的指针。
	@Return:
		BOOL
			返回TRUE则写入成功。
*/
BOOL
ata_device_write_sector(IN CASCTEXT symbol,
						IN uint32 lba,
						IN uint8 * buf)
{
	return ata_device_write_sectors(symbol, lba, 1, buf);
}
