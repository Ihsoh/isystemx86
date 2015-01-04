//Filename:		ifs1disk.h
//Author:		Ihsoh
//Date:			2014-6-5
//Descriptor:	Emulate IFS1 Disk

#ifndef	_IFS1DISK_H_
#define	_IFS1DISK_H_

#include "types.h"

extern void get_disk_symbol(uint index, char * symbol);
extern uint get_disk_count(void);
extern uint get_disk_size(char * symbol);
extern int is_system_disk(char * symbol);
extern void init_disk(char * symbol);
extern void destroy_disk(char * symbol);
extern uint sector_count(char * symbol);
extern int read_sector(char * symbol, uint pos, uchar * buffer);
extern int write_sector(char * symbol, uint pos, uchar * buffer);
extern int read_sectors_h(char * symbol, uint pos, uchar count, uchar * buffer);
extern int write_sectors_h(char * symbol, uint pos, uchar count, uchar * buffer);

#endif

