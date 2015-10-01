/**
	@File:			atapi.c
	@Author:		Ihsoh
	@Date:			2015-02-26
	@Description:
		ATAPI。
*/

#include "atapi.h"
#include "types.h"
#include "386.h"
#include "hdisk.h"
#include "kernel.h"

#define	MAX_RETRY	0x100

static BOOL _lock = FALSE;

/**
	@Function:		atapi_init
	@Access:		Public
	@Description:
		初始化ATAPI。
	@Parameters:
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
atapi_init(void)
{
	return TRUE;
}

/**
	@Function:		_atapi_lock
	@Access:		Private
	@Description:
		锁住ATAPI驱动。
	@Parameters:
	@Return:
		BOOL
			返回TRUE则获取锁成功，否则获取锁失败。
*/
static
BOOL
_atapi_lock(void)
{
	if(_lock)
		return FALSE;
	_lock = TRUE;
	return TRUE;
}

/**
	@Function:		_atapi_unlock
	@Access:		Private
	@Description:
		解锁ATAPI驱动。
	@Parameters:
	@Return:
*/
static
void
_atapi_unlock(void)
{
	_lock = FALSE;
}

/**
	@Function:		atapi_locked
	@Access:		Public
	@Description:
		获取ATAPI驱动的锁的状态。
	@Parameters:
	@Return:
		BOOL
			返回TRUE则代表ATAPI驱动的锁被锁定中。
*/
BOOL
atapi_locked(void)
{
	return _lock;
}

/**
	@Function:		atapi_read_sector
	@Access:		Private
	@Description:
		从ATAPI设备读取一个扇区。
	@Parameters:
		bus, uint32, IN
			总线。ATA_BUS_PRIMARY或ATA_BUS_SECONDARY。
		drive, uint32, IN
			驱动器。ATA_DRIVE_MASTER或ATA_DRIVE_SLAVE。
		lba, uint32, IN
			LBA地址。
		buffer, uint8 *, OUT
			指向用于储存从ATAPI设备读取扇区的数据的缓冲区的指针。
	@Return:
		int32
			实际读取的字节数。
*/
static
int32
_atapi_read_sector(	IN uint32 bus,
					IN uint32 drive,
					IN uint32 lba,
					OUT uint8 * buffer)
{
	// 0xA8是读取扇区的命令字节。
	uint8 read_cmd[12] = { 0xA8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	uint8 status;
	int32 size;

	// 清除IDE信号。
	RESET_ALL_IDE_SIGNAL();

	// 选择驱动器，并且选择是Master还是Slave。
	outb(ATA_DRIVE_SELECT(bus), drive & (1 << 4));      

	// 延迟400ns。
	ATA_SELECT_DELAY(bus);

	// PIO模式.
	outb(ATA_FEATURES(bus), 0x0);
	outb(ATA_ADDRESS2(bus), ATAPI_SECTOR_SIZE & 0xFF);
	outb(ATA_ADDRESS3(bus), ATAPI_SECTOR_SIZE >> 8);

	// ATA Packet命令。
	outb(ATA_COMMAND(bus), 0xA0);

	// 如果驱动器处于BUSY状态，等待。
	while((status = inb(ATA_COMMAND(bus))) & 0x80)
		asm volatile ("pause");

	// 等待数据。
	while (!((status = inb(ATA_COMMAND(bus))) & 0x8) && !(status & 0x1))
		asm volatile ("pause");

	// 发生错误则退出。
	if(status & 0x1)
		return -1;

	// 读取一个扇区。
	read_cmd[9] = 1;

	read_cmd[2] = (lba >> 0x18) & 0xFF;		// LBA最高位。
	read_cmd[3] = (lba >> 0x10) & 0xFF;
	read_cmd[4] = (lba >> 0x08) & 0xFF;
	read_cmd[5] = (lba >> 0x00) & 0xFF;		// LBA最低位。

	// 发送ATAPI/SCSI命令
	outsw(ATA_DATA(bus), (uint16 *)read_cmd, 6);

	// 等待驱动器发送IRQ命令。
	while(!GET_ALL_IDE_SIGNAL())
		asm volatile ("pause");
	CLEAR_ALL_IDE_SIGNAL();
	
	// 获取实际读取的字节数。
	size = 	(((int32)inb(ATA_ADDRESS3(bus))) << 8)
			| (int32)(inb(ATA_ADDRESS2(bus)));
	
	if(size != ATAPI_SECTOR_SIZE)
		return -1;
	
	// 读取数据。
	/*int32 i;
	for(i = 0; i < size / 2; i++)
	{
		while (!((status = inb(ATA_COMMAND(bus))) & 0x8) && !(status & 0x1))
			asm volatile ("pause");
		*(uint16 *)buffer = inw(ATA_DATA(bus));
		buffer += 2;
	}*/
	while (!((status = inb(ATA_COMMAND(bus))) & 0x8) && !(status & 0x1))
		asm volatile ("pause");
	insw(ATA_DATA(bus), buffer, size / 2);

	/* The controller will send another IRQ even though we've read all
	* the data we want.  Wait for it -- so it doesn't interfere with
	* subsequent operations: */
	while(!GET_ALL_IDE_SIGNAL())
		asm volatile ("pause");
	CLEAR_ALL_IDE_SIGNAL();
	
	// 等待BUSY和DRQ标志被清除，表示命令结束。
	while((status = inb(ATA_COMMAND(bus))) & 0x88)
		asm volatile ("pause");
	
	return size;
}

/**
	@Function:		atapi_read_sector
	@Access:		Public
	@Description:
		从ATAPI设备读取一个扇区。
	@Parameters:
		bus, uint32, IN
			总线。ATA_BUS_PRIMARY或ATA_BUS_SECONDARY。
		drive, uint32, IN
			驱动器。ATA_DRIVE_MASTER或ATA_DRIVE_SLAVE。
		lba, uint32, IN
			LBA地址。
		buffer, uint8 *, OUT
			指向用于储存从ATAPI设备读取扇区的数据的缓冲区的指针。
	@Return:
		int32
			实际读取的字节数。
*/
int32
atapi_read_sector(	IN uint32 bus,
					IN uint32 drive,
					IN uint32 lba,
					OUT uint8 * buffer)
{
	if(hdisk_locked())
		return -1;
	if(!_atapi_lock())
		return -1;
	uint32 ui;
	for(ui = 0; ui < MAX_RETRY; ui++)
	{
		int32 size = _atapi_read_sector(bus, drive, lba, buffer);
		if(size == ATAPI_SECTOR_SIZE)
		{
			_atapi_unlock();
			return size;
		}
	}
	_atapi_unlock();
	return -1;
}

/**
	@Function:		atapi_read_sector512
	@Access:		Public
	@Description:
		从ATAPI设备读取512字节的数据，模拟一个扇区为512字节。
		把一个2048字节的扇区分成4个512字节的模拟扇区。
	@Parameters:
		bus, uint32, IN
			总线。ATA_BUS_PRIMARY或ATA_BUS_SECONDARY。
		drive, uint32, IN
			驱动器。ATA_DRIVE_MASTER或ATA_DRIVE_SLAVE。
		pos, uint32, IN
			模拟扇区的地址。
		buffer, uint8 *, OUT
			指向用于储存从ATAPI设备读取的数据的缓冲区的指针。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
atapi_read_sector512(	IN uint32 bus,
						IN uint32 drive,
						IN uint32 pos, 
						OUT uint8 * buffer)
{
	uint32 lba = pos / 4;
	uint32 part = pos % 4;
	uint8 tmp[ATAPI_SECTOR_SIZE];
	if(atapi_read_sector(bus, drive, lba, tmp) != ATAPI_SECTOR_SIZE)
		return FALSE;
	memcpy(buffer, tmp + part * 512, 512);
	return TRUE;
}

/**
	@Function:		atapi_read_sector512s
	@Access:		Public
	@Description:
		从ATAPI设备读取指定数量的模拟扇区的数据。
		把一个2048字节的扇区分成4个512字节的模拟扇区。
	@Parameters:
		bus, uint32, IN
			总线。ATA_BUS_PRIMARY或ATA_BUS_SECONDARY。
		drive, uint32, IN
			驱动器。ATA_DRIVE_MASTER或ATA_DRIVE_SLAVE。
		pos, uint32, IN
			模拟扇区的地址。
		count, uint32, IN
			模拟扇区的数量。
		buffer, uint8 *, OUT
			指向用于储存从ATAPI设备读取的数据的缓冲区的指针。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
atapi_read_sector512s(	IN uint32 bus,
						IN uint32 drive,
						IN uint32 pos,
						IN uint32 count,
						OUT uint8 * buffer)
{
	uint32 ui;
	for(ui = 0; ui < count; ui++, pos++, buffer += 512)
		if(!atapi_read_sector512(bus, drive, pos, buffer))
			return FALSE;
	return TRUE;
}
