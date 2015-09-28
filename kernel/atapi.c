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

BOOL
atapi_init(void)
{
	return TRUE;
}

/* Use the ATAPI protocol to read a single sector from the given
 * bus/drive into the buffer using logical block address lba. */
int32
atapi_read_sector(	IN uint32 bus,
					IN uint32 drive,
					IN uint32 lba,
					OUT uint8 * buffer)
{
	/* 0xA8 is READ SECTORS command byte. */
	uint8 read_cmd[12] = { 0xA8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	uint8 status;
	int32 size;
	
	/* Tell the scheduler that this process is using the ATA subsystem. */
	//??? ata_grab();

	/* Select drive (only the slave-bit is set) */
	outb(ATA_DRIVE_SELECT(bus), drive & (1 << 4));      

	ATA_SELECT_DELAY(bus);       /* 400ns delay */

	outb(ATA_FEATURES(bus), 0x0);       /* PIO mode */
	outb(ATA_ADDRESS2(bus), ATAPI_SECTOR_SIZE & 0xFF);
	outb(ATA_ADDRESS3(bus), ATAPI_SECTOR_SIZE >> 8);
	outb(ATA_COMMAND(bus), 0xA0);       /* ATA PACKET command */

	while((status = inb(ATA_COMMAND(bus))) & 0x80)     /* BUSY */
		asm volatile ("pause");

	while (!((status = inb(ATA_COMMAND(bus))) & 0x8) && !(status & 0x1))
		asm volatile ("pause");

	/* DRQ or ERROR set */
	if(status & 0x1)
	{
		size = -1;
		goto cleanup;
	}
	read_cmd[9] = 1;              /* 1 sector */
	read_cmd[2] = (lba >> 0x18) & 0xFF;   /* most sig. byte of LBA */
	read_cmd[3] = (lba >> 0x10) & 0xFF;
	read_cmd[4] = (lba >> 0x08) & 0xFF;
	read_cmd[5] = (lba >> 0x00) & 0xFF;   /* least sig. byte of LBA */
	/* Send ATAPI/SCSI command */
	outsw(ATA_DATA(bus), (uint16 *)read_cmd, 6);

	/* Wait for IRQ that says the data is ready. */
	//???schedule();
	
	/* Read actual size */
	size =
		(((int32)inb(ATA_ADDRESS3(bus))) << 8) |
		(int32)(inb(ATA_ADDRESS2(bus)));
	
	/* This example code only supports the case where the data transfer
	* of one sector is done in one step. */
	//???ASSERT(size == ATAPI_SECTOR_SIZE);
	
	/* Read data. */
	insw(ATA_DATA(bus), buffer, size / 2);
	
	/* The controller will send another IRQ even though we've read all
	* the data we want.  Wait for it -- so it doesn't interfere with
	* subsequent operations: */
	//???schedule();
	
	/* Wait for BSY and DRQ to clear, indicating Command Finished */
	//while((status = inb(ATA_COMMAND(bus))) & 0x88) 
	//	asm volatile ("pause");

cleanup:
	
	/* Exit the ATA subsystem */
	//???ata_release();
	
	return size;
}
