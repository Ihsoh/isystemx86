//Filename:		ifs1vdisk.h
//Author:		Ihsoh
//Date:			2014-6-5
//Descriptor:	IFS1 Virtual Disk

#ifndef	_IFS1VDISK_H_
#define	_IFS1VDISK_H_

#include "types.h"

#define	VDISK_CLY				((uint)203)
#define	VDISK_HEAD				((uint)16)
#define	VDISK_SPT				((uint)63)
#define VDISK_SECTORS			(VDISK_CLY * VDISK_HEAD * VDISK_SPT)
#define VDISK_SECTOR_SIZE		512
#define VDISK_BUFFER_SIZE		(VDISK_SECTORS * VDISK_SECTOR_SIZE)

extern void init_vdisk(char * symbol);
extern void destroy_vdisk(char * symbol);
extern uint sector_count_v(void);
extern uint get_vdisk_size(char * symbol);
extern int read_sector_v(char * symbol, uint pos, uchar * buffer);
extern int write_sector_v(char * symbol, uint pos, uchar * buffer);
extern int read_sectors_v(char * symbol, uint pos, uchar count, uchar * buffer);
extern int write_sectors_v(char * symbol, uint pos, uchar count, uchar * buffer);

//Extensions:
extern uchar * get_vdiska(void);

#endif

